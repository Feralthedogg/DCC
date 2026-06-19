#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_attachment_numeric_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_SIZE ||
        key_id == DCC_JSON_KEY_WIDTH ||
        key_id == DCC_JSON_KEY_HEIGHT) {
        uint32_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u32_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_SIZE) {
            attachment->size = has_value ? value : 0U;
        } else if (key_id == DCC_JSON_KEY_WIDTH) {
            attachment->width = has_value ? value : 0U;
        } else {
            attachment->height = has_value ? value : 0U;
        }
    } else if (key_id == DCC_JSON_KEY_FLAGS) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        attachment->flags = value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
