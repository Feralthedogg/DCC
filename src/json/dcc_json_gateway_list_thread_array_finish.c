#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_thread_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
) {
    out->gateway.ids = out->gateway_ids;
    out->gateway.ids_count = stored;
    out->gateway.ids_truncated = truncated;
    out->thread_list_sync.thread_ids = out->gateway.ids;
    out->thread_list_sync.thread_ids_count = out->gateway.ids_count;
    out->thread_list_sync.thread_ids_truncated = out->gateway.ids_truncated;
    out->thread_list_sync.threads = out->gateway_threads;
    out->thread_list_sync.threads_count = stored;
    out->thread_list_sync.threads_truncated = truncated;
    out->guild.threads = out->gateway_threads;
    out->guild.threads_count = stored;
    out->guild.threads_truncated = truncated;
    out->has_guild = 1;
    out->has_thread_list_sync = 1;
}
