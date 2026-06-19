#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_emoji_voice_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_EMOJI_NAME) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_emoji_name,
            sizeof(out->gateway_emoji_name),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_gateway_apply_emoji_name(
            out,
            has_value ? out->gateway_emoji_name : NULL
        );
    } else if (key_id == DCC_JSON_KEY_ENDPOINT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_endpoint,
            sizeof(out->gateway_endpoint),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.endpoint = has_value ? out->gateway_endpoint : NULL;
        out->voice_server_update.endpoint = has_value ? out->gateway_endpoint : NULL;
        out->has_voice_server_update = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
