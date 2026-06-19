#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_channel_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (key_id) {
        case DCC_JSON_KEY_CHANNEL_TYPE:
        case DCC_JSON_KEY_POSITION:
        case DCC_JSON_KEY_RATE_LIMIT_PER_USER:
        case DCC_JSON_KEY_BITRATE:
        case DCC_JSON_KEY_USER_LIMIT:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_json_gateway_apply_d_channel_numeric_scalar_key(out, key_id, value, handled);
}
