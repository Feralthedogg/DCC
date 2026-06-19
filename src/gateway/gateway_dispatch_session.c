#include "internal/dcc_core_internal.h"
#include "internal/gateway/dcc_gateway_control_internal.h"
#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_basic_internal.h"
#include "internal/voice/dcc_voice_state_internal.h"

uint8_t dcc_gateway_dispatch_session_event(
    dcc_gateway_session_t *session,
    dcc_event_type_t type,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len,
    dcc_status_t *out_status
) {
    if (type == DCC_EVENT_READY) {
        dcc_status_t status = dcc_gateway_store_ready(
            session,
            payload->has_session_id ? payload->session_id : NULL,
            payload->has_resume_gateway_url ? payload->resume_gateway_url : NULL
        );
        if (status != DCC_OK) {
            dcc_set_error(session->client, "gateway READY missing session state");
            *out_status = status;
            return 1U;
        }
        if (payload->gateway.user_id != 0) {
            dcc_voice_client_set_self_user_id(session->client, payload->gateway.user_id);
        }
        *out_status = dcc_gateway_emit_ready(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    if (type == DCC_EVENT_RESUMED) {
        session->client->gateway_can_resume = true;
        *out_status = dcc_gateway_emit_resumed(session, payload, raw_json, raw_json_len);
        return 1U;
    }
    return 0U;
}
