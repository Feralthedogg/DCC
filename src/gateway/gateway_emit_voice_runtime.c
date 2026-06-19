#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_voice_runtime_internal.h"

dcc_status_t dcc_gateway_emit_voice_channel_effect(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_VOICE_CHANNEL_EFFECT_SEND, payload, raw_json, raw_json_len);
    event.data.voice_channel_effect = payload->voice_channel_effect;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_voice_server_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_VOICE_SERVER_UPDATE, payload, raw_json, raw_json_len);
    event.data.voice_server_update = payload->voice_server_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_voice_channel_start_time_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_VOICE_CHANNEL_START_TIME_UPDATE, payload, raw_json, raw_json_len);
    event.data.voice_channel_start_time_update = payload->voice_channel_start_time_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_voice_channel_status_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_VOICE_CHANNEL_STATUS_UPDATE, payload, raw_json, raw_json_len);
    event.data.voice_channel_status_update = payload->voice_channel_status_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_voice_state(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_VOICE_STATE_UPDATE, payload, raw_json, raw_json_len);
    event.data.voice_state = payload->voice_state;
    return dcc_gateway_dispatch_event(session, &event);
}
