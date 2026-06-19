#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_session_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_HEARTBEAT_INTERVAL) {
        int64_t value = 0;
        dcc_status_t status = dcc_json_parse_i64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->has_heartbeat_interval = 1;
        out->heartbeat_interval = value;
    } else if (key_id == DCC_JSON_KEY_SESSION_ID) {
        if (dcc_json_parse_string_buffer(parser, out->session_id, sizeof(out->session_id)) != 0) {
            return DCC_ERR_JSON;
        }
        out->has_session_id = 1;
        out->voice_state.session_id = out->session_id;
    } else if (key_id == DCC_JSON_KEY_RESUME_GATEWAY_URL) {
        if (dcc_json_parse_string_buffer(parser, out->resume_gateway_url, sizeof(out->resume_gateway_url)) != 0) {
            return DCC_ERR_JSON;
        }
        out->has_resume_gateway_url = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
