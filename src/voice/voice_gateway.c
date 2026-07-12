#include "internal/json/dcc_json.h"
#include "internal/voice/dcc_voice_internal.h"

static dcc_status_t dcc_voice_handle_parsed_frame(dcc_voice_client_t *voice_client, const dcc_json_t *root) {
    if (dcc_json_typeof(root) != DCC_JSON_OBJECT) {
        return DCC_ERR_JSON;
    }

    uint32_t op = 0;
    const dcc_json_t *op_json = dcc_json_object_get(root, "op");
    if (dcc_voice_gateway_json_u32(op_json, &op) != 0) {
        return DCC_ERR_JSON;
    }

    const dcc_json_t *seq_json = dcc_json_object_get(root, "seq");
    if (seq_json != NULL && dcc_voice_gateway_json_i32(seq_json, &voice_client->receive_sequence) != 0) {
        return DCC_ERR_JSON;
    }

    const dcc_json_t *d = dcc_json_object_get(root, "d");
    switch (op) {
        case 2:
            return dcc_voice_gateway_handle_ready(voice_client, d);
        case 4:
            return dcc_voice_gateway_handle_session_description(voice_client, d);
        case 8:
            return dcc_voice_gateway_handle_hello(voice_client, d);
        case 9:
            return dcc_voice_client_mark_session_active(voice_client);
        case 11:
            return dcc_voice_gateway_handle_clients_connect(voice_client, d);
        case 5:
            return dcc_voice_gateway_handle_speaking(voice_client, d);
        case 13:
            return dcc_voice_gateway_handle_client_disconnect(voice_client, d);
        case 20:
            return dcc_voice_gateway_handle_client_platform(voice_client, d);
        case 21:
            return dcc_voice_gateway_handle_dave_prepare_transition(voice_client, d);
        case 22:
            return dcc_voice_gateway_handle_dave_execute_transition(voice_client, d);
        case 24:
            return dcc_voice_gateway_handle_dave_session_update(voice_client, d);
        default:
            return DCC_OK;
    }
}

dcc_status_t dcc_voice_client_handle_gateway_frame(
    dcc_voice_client_t *voice_client,
    const char *json,
    size_t json_len
) {
    if (voice_client == NULL || json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_json_t *root = NULL;
    dcc_status_t status = dcc_json_parse(json, json_len, &root);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_voice_handle_parsed_frame(voice_client, root);
    dcc_json_free(root);
    return status;
}
