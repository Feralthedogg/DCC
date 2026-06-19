#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_sticker_list_field(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_sticker_buffers_t *buffers,
    int *handled
) {
    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_json_gateway_parse_sticker_list_id_field(parser, sticker, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_sticker_list_user_field(parser, sticker, key_id, buffers, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    status = dcc_json_gateway_parse_sticker_list_scalar_field(parser, sticker, key_id, handled);
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_sticker_list_text_field(parser, sticker, key_id, buffers, handled);
}
