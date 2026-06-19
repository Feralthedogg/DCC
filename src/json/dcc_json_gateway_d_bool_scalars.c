#include "internal/json/dcc_json_internal.h"

static int dcc_json_gateway_is_d_bool_scalar_key(dcc_json_key_id_t key_id) {
    switch (key_id) {
        case DCC_JSON_KEY_AVAILABLE:
        case DCC_JSON_KEY_BURST:
        case DCC_JSON_KEY_DISCOVERABLE_DISABLED:
        case DCC_JSON_KEY_ENABLED:
        case DCC_JSON_KEY_NEWLY_CREATED:
        case DCC_JSON_KEY_NSFW:
        case DCC_JSON_KEY_PENDING:
        case DCC_JSON_KEY_TEMPORARY:
            return 1;
        default:
            return 0;
    }
}

dcc_status_t dcc_json_gateway_parse_d_bool_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_json_gateway_is_d_bool_scalar_key(key_id)) {
        *handled = 0;
        return DCC_OK;
    }

    int value = 0;
    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_json_gateway_apply_d_bool_scalar_key(out, key_id, value, handled);
}
