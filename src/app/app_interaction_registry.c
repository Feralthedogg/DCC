#include "internal/app/dcc_app_interaction_registry_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/rest/dcc_rest_runtime_internal.h"
#include "internal/rest/dcc_rest_sensitive_internal.h"

#include <openssl/sha.h>

#include <stdlib.h>
#include <string.h>

typedef struct dcc_interaction_live_entry {
  dcc_interaction_flow_t *flow;
} dcc_interaction_live_entry_t;

typedef struct dcc_interaction_tombstone {
  dcc_snowflake_t application_id;
  dcc_snowflake_t interaction_id;
  unsigned char token_digest[SHA256_DIGEST_LENGTH];
  uint64_t expiry_ms;
  uint8_t used;
} dcc_interaction_tombstone_t;

static void dcc_registry_lock(dcc_client_t *client) {
  while (atomic_flag_test_and_set_explicit(&client->interaction_registry_lock,
                                           memory_order_acquire)) {
  }
}

static void dcc_registry_unlock(dcc_client_t *client) {
  atomic_flag_clear_explicit(&client->interaction_registry_lock,
                             memory_order_release);
}

static void dcc_registry_resource_lock(dcc_client_t *client) {
  while (atomic_flag_test_and_set_explicit(&client->rest_resource_lock,
                                           memory_order_acquire)) {
  }
}

static void dcc_registry_resource_unlock(dcc_client_t *client) {
  atomic_flag_clear_explicit(&client->rest_resource_lock, memory_order_release);
}

static void dcc_registry_increment(uint64_t *value) {
  if (*value != UINT64_MAX)
    ++*value;
}

static void dcc_registry_digest(const char *token,
                                unsigned char digest[SHA256_DIGEST_LENGTH]) {
  (void)SHA256((const unsigned char *)token, strlen(token), digest);
}

dcc_status_t dcc_app_interaction_registry_init(dcc_client_t *client) {
  if (client == NULL || client->interaction_max_live_queues == 0U ||
      client->interaction_max_tombstones == 0U ||
      client->interaction_max_live_queues >
          SIZE_MAX / sizeof(dcc_interaction_live_entry_t) ||
      client->interaction_max_tombstones >
          SIZE_MAX / sizeof(dcc_interaction_tombstone_t))
    return DCC_ERR_INVALID_ARG;
  client->interaction_live_entries =
      calloc(client->interaction_max_live_queues,
             sizeof(dcc_interaction_live_entry_t));
  client->interaction_tombstones = calloc(client->interaction_max_tombstones,
                                          sizeof(dcc_interaction_tombstone_t));
  if (client->interaction_live_entries == NULL ||
      client->interaction_tombstones == NULL) {
    free(client->interaction_live_entries);
    free(client->interaction_tombstones);
    client->interaction_live_entries = NULL;
    client->interaction_tombstones = NULL;
    return DCC_ERR_NOMEM;
  }
  atomic_flag_clear(&client->interaction_registry_lock);
  return DCC_OK;
}

void dcc_app_interaction_registry_deinit(dcc_client_t *client) {
  if (client == NULL)
    return;
  free(client->interaction_live_entries);
  free(client->interaction_tombstones);
  client->interaction_live_entries = NULL;
  client->interaction_tombstones = NULL;
}

static void dcc_registry_expire_locked(dcc_client_t *client, uint64_t now_ms) {
  dcc_interaction_tombstone_t *items = client->interaction_tombstones;
  for (size_t i = 0U; i < client->interaction_max_tombstones; ++i) {
    if (items[i].used != 0U && now_ms >= items[i].expiry_ms) {
      memset(&items[i], 0, sizeof(items[i]));
      dcc_registry_resource_lock(client);
      if (client->rest_runtime_stats.interaction_tombstones != 0U)
        --client->rest_runtime_stats.interaction_tombstones;
      dcc_registry_resource_unlock(client);
    }
  }
}

dcc_status_t
dcc_app_interaction_registry_register(dcc_client_t *client,
                                      dcc_interaction_flow_t *flow) {
  if (client == NULL || flow == NULL || flow->interaction == NULL ||
      flow->interaction->token == NULL)
    return DCC_ERR_INVALID_ARG;
  unsigned char digest[SHA256_DIGEST_LENGTH];
  dcc_registry_digest(flow->interaction->token, digest);
  dcc_registry_lock(client);
  dcc_registry_expire_locked(client, dcc_rest_now_ms());
  dcc_interaction_tombstone_t *tombstones = client->interaction_tombstones;
  for (size_t i = 0U; i < client->interaction_max_tombstones; ++i) {
    if (tombstones[i].used != 0U &&
        tombstones[i].application_id == flow->interaction->application_id &&
        tombstones[i].interaction_id == flow->interaction->id &&
        memcmp(tombstones[i].token_digest, digest, sizeof(digest)) == 0) {
      dcc_registry_resource_lock(client);
      dcc_registry_increment(
          &client->rest_runtime_stats.interaction_duplicate_deliveries);
      dcc_registry_resource_unlock(client);
      dcc_registry_unlock(client);
      dcc_endpoint_secure_zero(digest, sizeof(digest));
      return DCC_ERR_STATE;
    }
  }
  dcc_interaction_live_entry_t *entries = client->interaction_live_entries;
  size_t free_index = SIZE_MAX;
  for (size_t i = 0U; i < client->interaction_max_live_queues; ++i) {
    if (entries[i].flow == NULL) {
      if (free_index == SIZE_MAX)
        free_index = i;
      continue;
    }
    const dcc_interaction_t *other = entries[i].flow->interaction;
    if (other->application_id == flow->interaction->application_id &&
        other->id == flow->interaction->id &&
        strcmp(other->token, flow->interaction->token) == 0) {
      dcc_registry_resource_lock(client);
      dcc_registry_increment(
          &client->rest_runtime_stats.interaction_duplicate_deliveries);
      dcc_registry_resource_unlock(client);
      dcc_registry_unlock(client);
      dcc_endpoint_secure_zero(digest, sizeof(digest));
      return DCC_ERR_STATE;
    }
  }
  if (free_index == SIZE_MAX) {
    dcc_registry_resource_lock(client);
    dcc_registry_increment(
        &client->rest_runtime_stats.interaction_rejected_live_queues);
    dcc_registry_increment(&client->rest_runtime_stats.admission_rejections);
    dcc_registry_resource_unlock(client);
    dcc_registry_unlock(client);
    dcc_endpoint_secure_zero(digest, sizeof(digest));
    return DCC_ERR_RESOURCE_LIMIT;
  }
  entries[free_index].flow = flow;
  dcc_registry_resource_lock(client);
  ++client->rest_runtime_stats.interaction_live_queues;
  if (client->rest_runtime_stats.interaction_live_queues >
      client->rest_runtime_stats.interaction_live_queues_high_water)
    client->rest_runtime_stats.interaction_live_queues_high_water =
        client->rest_runtime_stats.interaction_live_queues;
  dcc_registry_resource_unlock(client);
  flow->registered = 1U;
  dcc_registry_unlock(client);
  dcc_endpoint_secure_zero(digest, sizeof(digest));
  return DCC_OK;
}

void dcc_app_interaction_registry_retire(dcc_client_t *client,
                                         dcc_interaction_flow_t *flow) {
  if (client == NULL || flow == NULL || flow->registered == 0U ||
      flow->interaction == NULL)
    return;
  unsigned char digest[SHA256_DIGEST_LENGTH];
  dcc_registry_digest(flow->interaction->token, digest);
  dcc_registry_lock(client);
  dcc_interaction_live_entry_t *entries = client->interaction_live_entries;
  for (size_t i = 0U; i < client->interaction_max_live_queues; ++i) {
    if (entries[i].flow == flow) {
      entries[i].flow = NULL;
      break;
    }
  }
  dcc_interaction_tombstone_t *tombstones = client->interaction_tombstones;
  for (size_t i = 0U; i < client->interaction_max_tombstones; ++i) {
    if (tombstones[i].used == 0U) {
      tombstones[i].used = 1U;
      tombstones[i].application_id = flow->interaction->application_id;
      tombstones[i].interaction_id = flow->interaction->id;
      memcpy(tombstones[i].token_digest, digest, sizeof(digest));
      uint64_t now = dcc_rest_now_ms();
      tombstones[i].expiry_ms =
          client->interaction_tombstone_ttl_ms > UINT64_MAX - now
              ? UINT64_MAX
              : now + client->interaction_tombstone_ttl_ms;
      dcc_registry_resource_lock(client);
      ++client->rest_runtime_stats.interaction_tombstones;
      if (client->rest_runtime_stats.interaction_tombstones >
          client->rest_runtime_stats.interaction_tombstones_high_water)
        client->rest_runtime_stats.interaction_tombstones_high_water =
            client->rest_runtime_stats.interaction_tombstones;
      dcc_registry_resource_unlock(client);
      break;
    }
  }
  dcc_registry_resource_lock(client);
  if (client->rest_runtime_stats.interaction_live_queues != 0U)
    --client->rest_runtime_stats.interaction_live_queues;
  dcc_registry_resource_unlock(client);
  flow->registered = 0U;
  dcc_registry_unlock(client);
  dcc_endpoint_secure_zero(digest, sizeof(digest));
}
