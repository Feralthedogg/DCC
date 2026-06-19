#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (parser == NULL || out == NULL) {
        return DCC_ERR_JSON;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_gateway_apply_emoji_id(out, value);
        (void)has_value;
    } else if (key_id == DCC_JSON_KEY_NAME) {
        int has_value = 0;
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
    } else if (key_id == DCC_JSON_KEY_ANIMATED) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_gateway_apply_emoji_animated(out, value);
    } else {
        *handled = 0;
    }
    return DCC_OK;
}
