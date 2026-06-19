#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_sticker_list_scalar_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    uint32_t *target = NULL;
    if (key_id == DCC_JSON_KEY_TYPE) {
        target = &sticker->type;
    } else if (key_id == DCC_JSON_KEY_FORMAT_TYPE) {
        target = &sticker->format_type;
    } else if (key_id == DCC_JSON_KEY_SORT_VALUE) {
        target = &sticker->sort_value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    *target = value;
    *handled = 1;
    return DCC_OK;
}
