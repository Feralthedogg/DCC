#ifndef DCC_REST_RUNTIME_H
#define DCC_REST_RUNTIME_H

#include <dcc/client.h>
#include <dcc/export.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  DCC_REST_DEFAULT_MAX_QUEUED_REQUESTS = 4096,
  DCC_REST_DEFAULT_MAX_REQUEST_BYTES = 32 * 1024 * 1024,
  DCC_REST_DEFAULT_MAX_QUEUED_BYTES = 128 * 1024 * 1024,
  DCC_REST_DEFAULT_MAX_ACTIVE_BYTES = 128 * 1024 * 1024,
  DCC_REST_DEFAULT_MAX_RESPONSE_BYTES = 32 * 1024 * 1024,
  DCC_INTERACTION_DEFAULT_MAX_LIVE_QUEUES = 1024,
  DCC_INTERACTION_DEFAULT_MAX_TOMBSTONES = 65536,
  DCC_INTERACTION_DEFAULT_TOMBSTONE_TTL_MS = 20 * 60 * 1000,
  DCC_INTERACTION_DEFAULT_MAX_ACTIONS_PER_QUEUE = 32,
  DCC_INTERACTION_DEFAULT_MAX_RESERVED_BYTES_PER_QUEUE = 32 * 1024 * 1024,
  DCC_INTERACTION_DEFAULT_MAX_RESERVED_BYTES_TOTAL = 128 * 1024 * 1024,
  DCC_REST_RUNTIME_STATS_VERSION = 1
};

typedef struct dcc_rest_runtime_stats {
  size_t size;
  uint32_t version;
  uint64_t queued_requests;
  uint64_t queued_requests_high_water;
  uint64_t active_requests;
  uint64_t active_requests_high_water;
  uint64_t completed_requests;
  uint64_t canceled_requests;
  uint64_t retried_requests;
  uint64_t rate_limited_requests;
  uint64_t admission_rejections;
  uint64_t queued_request_bytes;
  uint64_t queued_request_bytes_high_water;
  uint64_t active_request_bytes;
  uint64_t active_request_bytes_high_water;
  uint64_t response_bytes;
  uint64_t response_bytes_high_water;
  uint64_t rest_rejected_queued_requests;
  uint64_t rest_rejected_request_bytes;
  uint64_t rest_rejected_queued_bytes;
  uint64_t rest_rejected_active_bytes;
  uint64_t rest_rejected_response_bytes;
  uint64_t interaction_live_queues;
  uint64_t interaction_live_queues_high_water;
  uint64_t interaction_tombstones;
  uint64_t interaction_tombstones_high_water;
  uint64_t interaction_duplicate_deliveries;
  uint64_t interaction_actions;
  uint64_t interaction_actions_high_water;
  uint64_t interaction_reserved_bytes;
  uint64_t interaction_reserved_bytes_high_water;
  uint64_t interaction_rejected_live_queues;
  uint64_t interaction_rejected_tombstones;
  uint64_t interaction_rejected_actions_per_queue;
  uint64_t interaction_rejected_bytes_per_queue;
  uint64_t interaction_rejected_bytes_total;
} dcc_rest_runtime_stats_t;

#define DCC_REST_RUNTIME_STATS_INIT                                            \
  {sizeof(dcc_rest_runtime_stats_t), DCC_REST_RUNTIME_STATS_VERSION, 0U}

DCC_API dcc_status_t dcc_rest_runtime_stats(dcc_client_t *client,
                                            dcc_rest_runtime_stats_t *out);

#ifdef __cplusplus
}
#endif

#endif
