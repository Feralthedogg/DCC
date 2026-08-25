#include "internal/app/dcc_app_interaction_registry_internal.h"
#include "internal/client/dcc_client_lifecycle_internal.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/rest/dcc_rest_async_wait_internal.h"
#include "internal/rest/dcc_rest_error_observer_internal.h"
#include "internal/rest/dcc_rest_state_internal.h"

#include <dcc/rest/core/async.h>

#include <llam/runtime.h>

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_client_create(const dcc_client_options_t *options,
                               dcc_client_t **out) {
  if (options == NULL || out == NULL ||
      options->size <
          offsetof(dcc_client_options_t, token) + sizeof(options->token)) {
    return DCC_ERR_INVALID_ARG;
  }

#define DCC_LIMIT_VALUE(field_, default_)                                      \
  (options->size >= offsetof(dcc_client_options_t, field_) +                   \
                        sizeof(options->field_) &&                             \
           options->field_ != 0U                                               \
       ? options->field_                                                       \
       : (default_))
  size_t rest_max_queued_requests = DCC_LIMIT_VALUE(
      rest_max_queued_requests, DCC_REST_DEFAULT_MAX_QUEUED_REQUESTS);
  size_t rest_max_request_bytes = DCC_LIMIT_VALUE(
      rest_max_request_bytes, DCC_REST_DEFAULT_MAX_REQUEST_BYTES);
  size_t rest_max_queued_bytes =
      DCC_LIMIT_VALUE(rest_max_queued_bytes, DCC_REST_DEFAULT_MAX_QUEUED_BYTES);
  size_t rest_max_active_bytes =
      DCC_LIMIT_VALUE(rest_max_active_bytes, DCC_REST_DEFAULT_MAX_ACTIVE_BYTES);
  size_t rest_max_response_bytes = DCC_LIMIT_VALUE(
      rest_max_response_bytes, DCC_REST_DEFAULT_MAX_RESPONSE_BYTES);
  size_t interaction_max_live_queues = DCC_LIMIT_VALUE(
      interaction_max_live_queues, DCC_INTERACTION_DEFAULT_MAX_LIVE_QUEUES);
  size_t interaction_max_tombstones = DCC_LIMIT_VALUE(
      interaction_max_tombstones, DCC_INTERACTION_DEFAULT_MAX_TOMBSTONES);
  uint64_t interaction_tombstone_ttl_ms = DCC_LIMIT_VALUE(
      interaction_tombstone_ttl_ms, DCC_INTERACTION_DEFAULT_TOMBSTONE_TTL_MS);
  size_t interaction_max_actions_per_queue =
      DCC_LIMIT_VALUE(interaction_max_actions_per_queue,
                      DCC_INTERACTION_DEFAULT_MAX_ACTIONS_PER_QUEUE);
  size_t interaction_max_reserved_bytes_per_queue =
      DCC_LIMIT_VALUE(interaction_max_reserved_bytes_per_queue,
                      DCC_INTERACTION_DEFAULT_MAX_RESERVED_BYTES_PER_QUEUE);
  size_t interaction_max_reserved_bytes_total =
      DCC_LIMIT_VALUE(interaction_max_reserved_bytes_total,
                      DCC_INTERACTION_DEFAULT_MAX_RESERVED_BYTES_TOTAL);
#undef DCC_LIMIT_VALUE
  if (rest_max_queued_bytes < rest_max_request_bytes ||
      rest_max_request_bytes > SIZE_MAX - rest_max_response_bytes ||
      rest_max_active_bytes <
          rest_max_request_bytes + rest_max_response_bytes ||
      interaction_max_reserved_bytes_total <
          interaction_max_reserved_bytes_per_queue) {
    return DCC_ERR_INVALID_ARG;
  }

  dcc_client_t *client = (dcc_client_t *)calloc(1, sizeof(*client));
  if (client == NULL) {
    return DCC_ERR_NOMEM;
  }

  client->token = dcc_strdup(options->token != NULL ? options->token : "");
  if (client->token == NULL) {
    free(client);
    return DCC_ERR_NOMEM;
  }

#define DCC_OPTION_HAS(field)                                                  \
  (options->size >=                                                            \
   offsetof(dcc_client_options_t, field) + sizeof(options->field))
  if (DCC_OPTION_HAS(gateway_url)) {
    client->gateway_url =
        dcc_strdup(options->gateway_url != NULL ? options->gateway_url : "");
    if (client->gateway_url == NULL) {
      free(client->token);
      free(client);
      return DCC_ERR_NOMEM;
    }
  }

  client->intents = DCC_OPTION_HAS(intents) ? options->intents : 0;
  client->shard_id = DCC_OPTION_HAS(shard_id) ? options->shard_id : 0;
  client->shard_count = DCC_OPTION_HAS(shard_count) && options->shard_count != 0
                            ? options->shard_count
                            : 1;
  client->rest_concurrency =
      DCC_OPTION_HAS(rest_concurrency) && options->rest_concurrency != 0
          ? options->rest_concurrency
          : 4;
  client->enable_cache =
      DCC_OPTION_HAS(enable_cache) ? options->enable_cache : 0;
  client->infer_guild_id_from_channel =
      DCC_OPTION_HAS(infer_guild_id_from_channel)
          ? options->infer_guild_id_from_channel
          : 0;
  client->rest_max_queued_requests = rest_max_queued_requests;
  client->rest_max_request_bytes = rest_max_request_bytes;
  client->rest_max_queued_bytes = rest_max_queued_bytes;
  client->rest_max_active_bytes = rest_max_active_bytes;
  client->rest_max_response_bytes = rest_max_response_bytes;
  client->interaction_max_live_queues = interaction_max_live_queues;
  client->interaction_max_tombstones = interaction_max_tombstones;
  client->interaction_tombstone_ttl_ms = interaction_tombstone_ttl_ms;
  client->interaction_max_actions_per_queue = interaction_max_actions_per_queue;
  client->interaction_max_reserved_bytes_per_queue =
      interaction_max_reserved_bytes_per_queue;
  client->interaction_max_reserved_bytes_total =
      interaction_max_reserved_bytes_total;
  client->cache_policy.size = sizeof(client->cache_policy);
  client->cache_policy.max_guilds =
      DCC_OPTION_HAS(cache_max_guilds) ? options->cache_max_guilds : 0;
  client->cache_policy.max_channels =
      DCC_OPTION_HAS(cache_max_channels) ? options->cache_max_channels : 0;
  client->cache_policy.max_roles =
      DCC_OPTION_HAS(cache_max_roles) ? options->cache_max_roles : 0;
  client->cache_policy.max_members =
      DCC_OPTION_HAS(cache_max_members) ? options->cache_max_members : 0;
  client->cache_policy.max_users =
      DCC_OPTION_HAS(cache_max_users) ? options->cache_max_users : 0;
  client->cache_policy.max_messages =
      DCC_OPTION_HAS(cache_max_messages) ? options->cache_max_messages : 0;
  client->cache_policy.max_voice_states = DCC_OPTION_HAS(cache_max_voice_states)
                                              ? options->cache_max_voice_states
                                              : 0;
  client->gateway_recommended_shards = client->shard_count;
  client->gateway_max_concurrency =
      DCC_OPTION_HAS(gateway_max_concurrency) &&
              options->gateway_max_concurrency != 0
          ? options->gateway_max_concurrency
          : 1;
  client->gateway_stop_runtime_on_error = 1U;
  client->log_fn = DCC_OPTION_HAS(log_fn) ? options->log_fn : NULL;
  client->log_user_data =
      DCC_OPTION_HAS(log_user_data) ? options->log_user_data : NULL;
#undef DCC_OPTION_HAS
  atomic_init(&client->inferred_channel_id, 0U);
  atomic_init(&client->inferred_guild_id, 0U);
  atomic_init(&client->started, false);
  atomic_init(&client->stopping, false);
  atomic_init(&client->start_admission, 0U);
  atomic_init(&client->wait_admission, 0U);
  atomic_init(&client->gateway_task_running, false);
  atomic_init(&client->gateway_task_completed, false);
  atomic_init(&client->gateway_last_status, DCC_OK);
  atomic_flag_clear(&client->gateway_control_lock);
  atomic_init(&client->gateway_stop_requested, false);
  atomic_init(&client->gateway_reconnect_requested, false);
  atomic_init(&client->gateway_reconnect_resume, false);
  atomic_flag_clear(&client->dm_channel_cache_lock);
  atomic_flag_clear(&client->voice_clients_lock);
  dcc_event_bus_init(&client->events);
  atomic_flag_clear(&client->interaction_listeners_lock);
  atomic_flag_clear(&client->rest_resource_lock);
  memset(&client->rest_runtime_stats, 0, sizeof(client->rest_runtime_stats));
  client->rest_runtime_stats.size = sizeof(client->rest_runtime_stats);
  client->rest_runtime_stats.version = DCC_REST_RUNTIME_STATS_VERSION;
  dcc_cache_init(&client->cache);

  dcc_status_t st = dcc_app_interaction_registry_init(client);
  if (st != DCC_OK) {
    dcc_cache_deinit(&client->cache);
    dcc_event_bus_deinit(&client->events);
    free(client->gateway_url);
    free(client->token);
    free(client);
    return st;
  }
  st = dcc_rest_init(client);
  if (st != DCC_OK) {
    dcc_cache_deinit(&client->cache);
    dcc_event_bus_deinit(&client->events);
    dcc_app_interaction_registry_deinit(client);
    free(client->gateway_url);
    free(client->token);
    free(client);
    return st;
  }

  *out = client;
  return DCC_OK;
}

dcc_status_t dcc_client_prepare_destroy(dcc_client_t *client) {
  if (client == NULL) {
    return DCC_ERR_INVALID_ARG;
  }
  dcc_rest_lock(client);
  uint8_t already_quiesced = client->rest_quiesced;
  dcc_rest_unlock(client);
  if (already_quiesced) {
    return DCC_OK;
  }

  /* Stop new REST work before any teardown wait. Then close long-running
   * wait registration and drain the short start path before requesting stop.
   * A start admitted before this point must finish publishing its runtime
   * tasks before stop/quiesce. */
  dcc_rest_close_admission(client);
  dcc_client_lifecycle_admission_close(&client->wait_admission);
  dcc_client_lifecycle_admission_close(&client->start_admission);
  while (dcc_client_lifecycle_admission_count(&client->start_admission) != 0U) {
    dcc_rest_sleep_ms(1U);
  }

  dcc_voice_client_stop_owned(client);
  dcc_status_t status = dcc_client_stop(client);
  dcc_voice_client_unbind_owner(client);
  if (status != DCC_OK) {
    return status;
  }

  (void)dcc_rest_async_cancel_all(client, NULL);
  dcc_rest_operations_wait(client);
  (void)dcc_rest_async_cancel_all(client, NULL);

  status = dcc_runtime_quiesce(&client->runtime);
  if (status != DCC_OK) {
    return status;
  }
  while (dcc_client_lifecycle_admission_count(&client->wait_admission) != 0U) {
    dcc_rest_sleep_ms(1U);
  }
  if (client->runtime.initialized) {
    status = dcc_rest_async_wait(client, 0U);
    if (status != DCC_OK) {
      return status;
    }
  }
  dcc_rest_lock(client);
  uint8_t rest_workers_quiesced =
      client->rest_async_active == 0U &&
      client->rest_async_active_head == NULL &&
      dcc_rest_async_pending_count_locked(client) == 0U;
  dcc_rest_unlock(client);
  if (!rest_workers_quiesced) {
    return DCC_ERR_RUNTIME;
  }
  dcc_runtime_shutdown(&client->runtime);

  dcc_rest_lock(client);
  client->rest_quiesced = 1U;
  dcc_rest_unlock(client);
  return DCC_OK;
}

void dcc_client_close_rest(dcc_client_t *client) {
  if (client == NULL) {
    return;
  }
  dcc_rest_terminal_close_and_wait(client);
  dcc_rest_deinit(client);
}

void dcc_client_destroy(dcc_client_t *client) {
  if (client == NULL) {
    return;
  }
  /* A managed LLAM task cannot synchronously wait for the scheduler that is
   * currently executing it. Preserve the documented callback-local
   * stop-only behavior for every managed runtime callback, including REST
   * terminal callbacks. */
  if (dcc_rest_terminal_callback_active(client) ||
      llam_current_task() != NULL) {
    (void)dcc_client_stop(client);
    return;
  }
  dcc_status_t status = dcc_client_prepare_destroy(client);
  if (status != DCC_OK) {
    dcc_set_error(client, "client destroy could not safely quiesce runtime");
    dcc_emit_log(client, DCC_LOG_ERROR,
                 "client destroy deferred because runtime quiescence failed; "
                 "retry destroy");
    return;
  }
  dcc_client_close_rest(client);
  dcc_cache_deinit(&client->cache);
  dcc_event_bus_deinit(&client->events);
  dcc_client_interaction_listeners_deinit(client);
  dcc_app_interaction_registry_deinit(client);
  free(client->gateway_resume_url);
  free(client->gateway_session_id);
  free(client->gateway_cached_url);
  free(client->gateway_url);
  free(client->token);
  free(client);
}
