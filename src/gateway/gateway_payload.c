#include "internal/gateway/dcc_gateway_identify_internal.h"
#include "internal/gateway/dcc_gateway_payload_buffer_internal.h"
#include "internal/gateway/dcc_gateway_resume_internal.h"
#include "internal/ws/dcc_ws.h"

dcc_status_t dcc_gateway_send_identify(dcc_gateway_session_t *session) {
    dcc_gateway_payload_buffer_t payload = {0};
    dcc_status_t status = DCC_ERR_NOMEM;

    if (dcc_gateway_payload_buffer_append_cstr(&payload, "{\"op\":2,\"d\":{\"token\":") != 0 ||
        dcc_gateway_payload_buffer_append_json_string(&payload, session->client->token) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, ",\"intents\":") != 0 ||
        dcc_gateway_payload_buffer_append_u64(&payload, (uint64_t)session->client->intents) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(
            &payload,
            ",\"properties\":{\"os\":\"unknown\",\"browser\":\"dcc\",\"device\":\"dcc\"},\"shard\":["
        ) != 0 ||
        dcc_gateway_payload_buffer_append_u64(&payload, (uint64_t)session->client->shard_id) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, ",") != 0 ||
        dcc_gateway_payload_buffer_append_u64(&payload, (uint64_t)session->client->shard_count) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, "]}}") != 0) {
        goto done;
    }

    status = dcc_ws_send_text(session->ws, payload.data, payload.len);

done:
    dcc_gateway_payload_buffer_deinit(&payload);
    return status;
}

dcc_status_t dcc_gateway_send_resume(dcc_gateway_session_t *session) {
    if (!dcc_gateway_can_resume(session->client)) {
        return DCC_ERR_STATE;
    }

    dcc_gateway_payload_buffer_t payload = {0};
    dcc_status_t status = DCC_ERR_NOMEM;

    if (dcc_gateway_payload_buffer_append_cstr(&payload, "{\"op\":6,\"d\":{\"token\":") != 0 ||
        dcc_gateway_payload_buffer_append_json_string(&payload, session->client->token) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, ",\"session_id\":") != 0 ||
        dcc_gateway_payload_buffer_append_json_string(&payload, session->client->gateway_session_id) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, ",\"seq\":") != 0 ||
        dcc_gateway_payload_buffer_append_u64(&payload, session->client->gateway_seq) != 0 ||
        dcc_gateway_payload_buffer_append_cstr(&payload, "}}") != 0) {
        goto done;
    }

    status = dcc_ws_send_text(session->ws, payload.data, payload.len);

done:
    dcc_gateway_payload_buffer_deinit(&payload);
    return status;
}
