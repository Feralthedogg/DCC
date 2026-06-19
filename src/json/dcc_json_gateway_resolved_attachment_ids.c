#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_attachment_id_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_ID) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    attachment->id = value;
    *handled = 1;
    return DCC_OK;
}
