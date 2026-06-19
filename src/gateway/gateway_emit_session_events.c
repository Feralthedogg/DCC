#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_session_internal.h"

#include <limits.h>
#include <string.h>

dcc_status_t dcc_gateway_emit_ready(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t ready;
    dcc_gateway_init_event(&ready, DCC_EVENT_READY, payload, raw_json, raw_json_len);
    memset(&ready.data.ready, 0, sizeof(ready.data.ready));
    ready.data.ready.shard_id = session->client->shard_id;
    ready.data.ready.shard_count = session->client->shard_count;
    ready.data.ready.session_id = session->client->gateway_session_id != NULL ? session->client->gateway_session_id : "";
    ready.data.ready.guild_ids = payload != NULL ? payload->ready_guild_ids : NULL;
    ready.data.ready.guild_ids_count = payload != NULL ? payload->ready_guild_ids_count : 0;
    ready.data.ready.guild_ids_truncated = payload != NULL ? payload->ready_guild_ids_truncated : 0;
    ready.data.ready.guild_count = payload != NULL && payload->ready_guild_count <= UINT32_MAX
        ? (uint32_t)payload->ready_guild_count
        : UINT32_MAX;
    return dcc_gateway_dispatch_event(session, &ready);
}

dcc_status_t dcc_gateway_emit_resumed(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t resumed;
    dcc_gateway_init_event(&resumed, DCC_EVENT_RESUMED, payload, raw_json, raw_json_len);
    memset(&resumed.data.ready, 0, sizeof(resumed.data.ready));
    resumed.data.ready.shard_id = session->client->shard_id;
    resumed.data.ready.shard_count = session->client->shard_count;
    resumed.data.ready.session_id = session->client->gateway_session_id != NULL ? session->client->gateway_session_id : "";
    return dcc_gateway_dispatch_event(session, &resumed);
}
