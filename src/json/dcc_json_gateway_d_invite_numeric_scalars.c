#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_invite_numeric_scalar_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_TARGET_TYPE:
        case DCC_JSON_KEY_MAX_AGE:
        case DCC_JSON_KEY_MAX_USES:
        case DCC_JSON_KEY_USES:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_TARGET_TYPE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.target_type = value;
        out->invite.target_type = value;
        out->has_invite = 1;
    } else if (key_id == DCC_JSON_KEY_MAX_AGE) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.max_age = value;
        out->invite.max_age = value;
        out->has_invite = 1;
    } else if (key_id == DCC_JSON_KEY_MAX_USES) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.max_uses = value;
        out->invite.max_uses = value;
        out->has_invite = 1;
    } else if (key_id == DCC_JSON_KEY_USES) {
        uint32_t value = 0;
        dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.uses = value;
        out->invite.uses = value;
        out->has_invite = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
