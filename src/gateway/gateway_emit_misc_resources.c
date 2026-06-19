#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_misc_resources_internal.h"

#include <string.h>

dcc_status_t dcc_gateway_emit_webhooks_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_WEBHOOKS_UPDATE, payload, raw_json, raw_json_len);
    event.data.webhooks_update = payload->webhooks_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_join_request_update(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.guild_join_request_update = payload->guild_join_request_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_guild_application_command_update(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, type, payload, raw_json, raw_json_len);
    event.data.guild_application_command_update = payload->guild_application_command_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_embedded_activity_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_EMBEDDED_ACTIVITY_UPDATE, payload, raw_json, raw_json_len);
    event.data.embedded_activity_update = payload->embedded_activity_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_rate_limited(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_RATE_LIMITED, payload, raw_json, raw_json_len);
    memset(&event.data.rate_limited, 0, sizeof(event.data.rate_limited));
    event.data.rate_limited.retry_after = payload != NULL ? payload->gateway.retry_after : 0.0;
    event.data.rate_limited.body = raw_json;
    event.data.rate_limited.body_len = raw_json_len;
    return dcc_gateway_dispatch_event(session, &event);
}
