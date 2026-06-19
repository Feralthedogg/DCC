#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_author_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_AUTHOR) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_author(parser, out);
    if (status != DCC_OK) {
        return status;
    }
    out->has_message = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MESSAGE_AUTHOR |
        DCC_CACHE_FIELD_USER_PROFILE |
        DCC_CACHE_FIELD_USER_BOT |
        DCC_CACHE_FIELD_USER_FLAGS;
    *handled = 1;
    return DCC_OK;
}
