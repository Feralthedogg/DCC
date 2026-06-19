#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_voice_time_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_REQUEST_TO_SPEAK_TIMESTAMP) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        out->voice_state_request_to_speak_timestamp,
        sizeof(out->voice_state_request_to_speak_timestamp),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    out->voice_state.request_to_speak_timestamp = has_value
        ? out->voice_state_request_to_speak_timestamp
        : NULL;
    out->has_voice_state = 1;

    *handled = 1;
    return DCC_OK;
}
