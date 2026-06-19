#include "internal/events/dcc_event_state_internal.h"
#include "internal/gateway/dcc_gateway_cache_internal.h"
#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/voice/dcc_voice_state_internal.h"
#include "internal/ws/dcc_ws.h"

#include <limits.h>
#include <string.h>

void dcc_gateway_init_event(
    dcc_event_t *event,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    event->type = type;
    event->shard_id = 0;
    event->cancelled = 0;
    event->raw.type = type;
    event->raw.sequence = payload != NULL && payload->has_seq ? payload->seq : 0;
    event->raw.name = payload != NULL && payload->has_event_name ? payload->event_name : dcc_event_type_name(type);
    event->raw.json = raw_json;
    event->raw.json_len = raw_json_len;
    event->gateway = payload != NULL ? &payload->gateway : NULL;
    event->cache_fields = payload != NULL ? payload->cache_fields : 0;
}
dcc_status_t dcc_gateway_dispatch_event(dcc_gateway_session_t *session, const dcc_event_t *event) {
    dcc_gateway_cache_apply_event(session != NULL ? session->client : NULL, event);
    dcc_voice_client_apply_gateway_event(session != NULL ? session->client : NULL, event);
    dcc_status_t status = dcc_event_bus_dispatch(&session->client->events, session->client, event);
    if (status != DCC_OK || event->type == DCC_EVENT_RAW || event->type == DCC_EVENT_SOCKET_CLOSE) {
        return status;
    }

    dcc_event_t raw;
    raw.type = DCC_EVENT_RAW;
    raw.shard_id = 0;
    raw.cancelled = 0;
    raw.raw = event->raw;
    raw.gateway = event->gateway;
    raw.cache_fields = event->cache_fields;
    return dcc_event_bus_dispatch(&session->client->events, session->client, &raw);
}
dcc_status_t dcc_gateway_emit_raw_dispatch(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    return dcc_gateway_dispatch_event(session, &event);
}
dcc_status_t dcc_gateway_emit_socket_close(
    dcc_gateway_session_t *session,
    uint16_t code,
    const char *reason
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_SOCKET_CLOSE, NULL, NULL, 0);
    event.data.socket_close.fd = dcc_ws_fd(session != NULL ? session->ws : NULL);
    event.data.socket_close.code = code;
    event.data.socket_close.reason = reason != NULL ? reason : "";
    return dcc_gateway_dispatch_event(session, &event);
}
