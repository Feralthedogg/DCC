#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_thread_runtime_internal.h"

dcc_status_t dcc_gateway_emit_thread_member(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_THREAD_MEMBER_UPDATE, payload, raw_json, raw_json_len);
    event.data.thread_member = payload->thread_member;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_thread_members_update(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_THREAD_MEMBERS_UPDATE, payload, raw_json, raw_json_len);
    event.data.thread_members_update = payload->thread_members_update;
    return dcc_gateway_dispatch_event(session, &event);
}

dcc_status_t dcc_gateway_emit_thread_list_sync(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_THREAD_LIST_SYNC, payload, raw_json, raw_json_len);
    event.data.thread_list_sync = payload->thread_list_sync;
    return dcc_gateway_dispatch_event(session, &event);
}
