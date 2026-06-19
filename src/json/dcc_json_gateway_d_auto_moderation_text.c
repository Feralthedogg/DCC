#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_auto_moderation_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_MATCHED_KEYWORD) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_matched_keyword,
            sizeof(out->gateway_matched_keyword),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.matched_keyword = has_value ? out->gateway_matched_keyword : NULL;
        out->auto_moderation_action.matched_keyword = has_value ? out->gateway_matched_keyword : NULL;
        out->has_auto_moderation_action = 1;
    } else if (key_id == DCC_JSON_KEY_MATCHED_CONTENT) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->gateway_matched_content,
            sizeof(out->gateway_matched_content),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.matched_content = has_value ? out->gateway_matched_content : NULL;
        out->auto_moderation_action.matched_content = has_value ? out->gateway_matched_content : NULL;
        out->has_auto_moderation_action = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
