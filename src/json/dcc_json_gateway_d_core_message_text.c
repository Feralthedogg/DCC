#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_CONTENT) {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        out->message_content,
        sizeof(out->message_content),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    out->message.content = has_value ? out->message_content : NULL;
    out->gateway.content = has_value ? out->message_content : NULL;
    out->auto_moderation_action.content = has_value ? out->message_content : NULL;
    out->has_message = 1;
    out->has_auto_moderation_action = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_CONTENT;
    *handled = 1;
    return DCC_OK;
}
