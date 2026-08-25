#include "internal/client/dcc_client_state_internal.h"

#include <dcc/rest/runtime.h>

#include <stddef.h>

static void dcc_rest_resource_lock(dcc_client_t *client) {
  while (atomic_flag_test_and_set_explicit(&client->rest_resource_lock,
                                           memory_order_acquire)) {
  }
}

static void dcc_rest_resource_unlock(dcc_client_t *client) {
  atomic_flag_clear_explicit(&client->rest_resource_lock, memory_order_release);
}

dcc_status_t dcc_rest_runtime_stats(dcc_client_t *client,
                                    dcc_rest_runtime_stats_t *out) {
  if (client == NULL || out == NULL ||
      out->size < offsetof(dcc_rest_runtime_stats_t, queued_requests) ||
      out->version != DCC_REST_RUNTIME_STATS_VERSION)
    return DCC_ERR_INVALID_ARG;
  dcc_rest_resource_lock(client);
  dcc_rest_runtime_stats_t snapshot = client->rest_runtime_stats;
  dcc_rest_resource_unlock(client);
#define DCC_STATS_COPY(field_)                                                 \
  do {                                                                         \
    if (out->size >=                                                           \
        offsetof(dcc_rest_runtime_stats_t, field_) + sizeof(out->field_))      \
      out->field_ = snapshot.field_;                                           \
  } while (0)
  DCC_STATS_COPY(queued_requests);
  DCC_STATS_COPY(queued_requests_high_water);
  DCC_STATS_COPY(active_requests);
  DCC_STATS_COPY(active_requests_high_water);
  DCC_STATS_COPY(completed_requests);
  DCC_STATS_COPY(canceled_requests);
  DCC_STATS_COPY(retried_requests);
  DCC_STATS_COPY(rate_limited_requests);
  DCC_STATS_COPY(admission_rejections);
  DCC_STATS_COPY(queued_request_bytes);
  DCC_STATS_COPY(queued_request_bytes_high_water);
  DCC_STATS_COPY(active_request_bytes);
  DCC_STATS_COPY(active_request_bytes_high_water);
  DCC_STATS_COPY(response_bytes);
  DCC_STATS_COPY(response_bytes_high_water);
  DCC_STATS_COPY(rest_rejected_queued_requests);
  DCC_STATS_COPY(rest_rejected_request_bytes);
  DCC_STATS_COPY(rest_rejected_queued_bytes);
  DCC_STATS_COPY(rest_rejected_active_bytes);
  DCC_STATS_COPY(rest_rejected_response_bytes);
  DCC_STATS_COPY(interaction_live_queues);
  DCC_STATS_COPY(interaction_live_queues_high_water);
  DCC_STATS_COPY(interaction_tombstones);
  DCC_STATS_COPY(interaction_tombstones_high_water);
  DCC_STATS_COPY(interaction_duplicate_deliveries);
  DCC_STATS_COPY(interaction_actions);
  DCC_STATS_COPY(interaction_actions_high_water);
  DCC_STATS_COPY(interaction_reserved_bytes);
  DCC_STATS_COPY(interaction_reserved_bytes_high_water);
  DCC_STATS_COPY(interaction_rejected_live_queues);
  DCC_STATS_COPY(interaction_rejected_tombstones);
  DCC_STATS_COPY(interaction_rejected_actions_per_queue);
  DCC_STATS_COPY(interaction_rejected_bytes_per_queue);
  DCC_STATS_COPY(interaction_rejected_bytes_total);
#undef DCC_STATS_COPY
  return DCC_OK;
}
