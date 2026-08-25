#include "internal/rest/dcc_rest_resource_internal.h"

static void resource_lock(dcc_client_t *client) {
  while (atomic_flag_test_and_set_explicit(&client->rest_resource_lock,
                                           memory_order_acquire)) {
  }
}

static void resource_unlock(dcc_client_t *client) {
  atomic_flag_clear_explicit(&client->rest_resource_lock, memory_order_release);
}

static void increment(uint64_t *value) {
  if (*value != UINT64_MAX)
    ++*value;
}

static void high_water(uint64_t value, uint64_t *watermark) {
  if (value > *watermark)
    *watermark = value;
}

dcc_status_t dcc_rest_resource_reserve_queued(dcc_client_t *client,
                                               size_t charge) {
  if (client == NULL || charge == 0U)
    return DCC_ERR_INVALID_ARG;
  resource_lock(client);
  dcc_rest_runtime_stats_t *stats = &client->rest_runtime_stats;
  dcc_status_t status = DCC_OK;
  if (charge > client->rest_max_request_bytes) {
    increment(&stats->rest_rejected_request_bytes);
    status = DCC_ERR_RESOURCE_LIMIT;
  } else if (stats->queued_requests >= client->rest_max_queued_requests) {
    increment(&stats->rest_rejected_queued_requests);
    status = DCC_ERR_RESOURCE_LIMIT;
  } else if (charge > client->rest_max_queued_bytes -
                          (size_t)stats->queued_request_bytes) {
    increment(&stats->rest_rejected_queued_bytes);
    status = DCC_ERR_RESOURCE_LIMIT;
  }
  if (status != DCC_OK) {
    increment(&stats->admission_rejections);
  } else {
    ++stats->queued_requests;
    stats->queued_request_bytes += charge;
    high_water(stats->queued_requests, &stats->queued_requests_high_water);
    high_water(stats->queued_request_bytes,
               &stats->queued_request_bytes_high_water);
  }
  resource_unlock(client);
  return status;
}

void dcc_rest_resource_release_unpublished(dcc_client_t *client,
                                            size_t charge) {
  if (client == NULL || charge == 0U)
    return;
  resource_lock(client);
  if (client->rest_runtime_stats.queued_requests != 0U)
    --client->rest_runtime_stats.queued_requests;
  if (client->rest_runtime_stats.queued_request_bytes >= charge)
    client->rest_runtime_stats.queued_request_bytes -= charge;
  resource_unlock(client);
}

uint8_t dcc_rest_resource_activate(dcc_rest_async_request_t *request) {
  if (request == NULL || request->resource_state != 1U)
    return 0U;
  dcc_client_t *client = request->client;
  resource_lock(client);
  dcc_rest_runtime_stats_t *stats = &client->rest_runtime_stats;
  if (request->resource_charge > client->rest_max_active_bytes -
                                     (size_t)stats->active_request_bytes) {
    increment(&stats->rest_rejected_active_bytes);
    resource_unlock(client);
    return 0U;
  }
  if (stats->queued_requests != 0U)
    --stats->queued_requests;
  if (stats->queued_request_bytes >= request->resource_charge)
    stats->queued_request_bytes -= request->resource_charge;
  ++stats->active_requests;
  stats->active_request_bytes += request->resource_charge;
  high_water(stats->active_requests, &stats->active_requests_high_water);
  high_water(stats->active_request_bytes,
             &stats->active_request_bytes_high_water);
  request->resource_state = 2U;
  resource_unlock(client);
  return 1U;
}

void dcc_rest_resource_requeue(dcc_rest_async_request_t *request) {
  if (request == NULL || request->resource_state != 2U)
    return;
  dcc_client_t *client = request->client;
  resource_lock(client);
  dcc_rest_runtime_stats_t *stats = &client->rest_runtime_stats;
  if (stats->active_requests != 0U)
    --stats->active_requests;
  if (stats->active_request_bytes >= request->resource_charge)
    stats->active_request_bytes -= request->resource_charge;
  ++stats->queued_requests;
  stats->queued_request_bytes += request->resource_charge;
  increment(&stats->retried_requests);
  request->resource_state = 1U;
  resource_unlock(client);
}

void dcc_rest_resource_release_request(dcc_rest_async_request_t *request) {
  if (request == NULL || request->resource_state == 0U)
    return;
  dcc_client_t *client = request->client;
  resource_lock(client);
  dcc_rest_runtime_stats_t *stats = &client->rest_runtime_stats;
  if (request->resource_state == 1U) {
    if (stats->queued_requests != 0U)
      --stats->queued_requests;
    if (stats->queued_request_bytes >= request->resource_charge)
      stats->queued_request_bytes -= request->resource_charge;
  } else {
    if (stats->active_requests != 0U)
      --stats->active_requests;
    if (stats->active_request_bytes >= request->resource_charge)
      stats->active_request_bytes -= request->resource_charge;
  }
  if (request->resource_count_completion != 0U) {
    increment(&stats->completed_requests);
    if (atomic_load_explicit(&request->cancel_requested,
                             memory_order_acquire))
      increment(&stats->canceled_requests);
  }
  request->resource_state = 0U;
  resource_unlock(client);
}

dcc_status_t dcc_rest_resource_reserve_response(dcc_client_t *client,
                                                size_t bytes) {
  if (client == NULL || bytes == 0U)
    return DCC_OK;
  resource_lock(client);
  dcc_rest_runtime_stats_t *stats = &client->rest_runtime_stats;
  dcc_status_t status = DCC_OK;
  if (bytes > client->rest_max_response_bytes ||
      bytes > client->rest_max_active_bytes -
                  (size_t)stats->active_request_bytes) {
    increment(&stats->rest_rejected_response_bytes);
    status = DCC_ERR_RESOURCE_LIMIT;
  } else {
    stats->response_bytes += bytes;
    stats->active_request_bytes += bytes;
    high_water(stats->response_bytes, &stats->response_bytes_high_water);
    high_water(stats->active_request_bytes,
               &stats->active_request_bytes_high_water);
  }
  resource_unlock(client);
  return status;
}

void dcc_rest_resource_release_response(dcc_client_t *client, size_t bytes) {
  if (client == NULL || bytes == 0U)
    return;
  resource_lock(client);
  if (client->rest_runtime_stats.response_bytes >= bytes)
    client->rest_runtime_stats.response_bytes -= bytes;
  if (client->rest_runtime_stats.active_request_bytes >= bytes)
    client->rest_runtime_stats.active_request_bytes -= bytes;
  resource_unlock(client);
}

void dcc_rest_resource_record_rate_limit(dcc_client_t *client) {
  if (client == NULL)
    return;
  resource_lock(client);
  increment(&client->rest_runtime_stats.rate_limited_requests);
  resource_unlock(client);
}
