#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_scalar_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_PENDING) {
        int value = 0;
        dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.pending = value ? 1U : 0U;
        out->member.pending = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_MEMBER_PENDING;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_FLAGS) {
        uint64_t value = 0;
        dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.flags = value;
        out->member.flags = value;
        out->cache_fields |= DCC_CACHE_FIELD_MEMBER_FLAGS;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_PERMISSIONS) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->interaction_member_permissions = has_value ? (dcc_permission_t)value : 0;
        out->interaction_member_permissions_present = has_value ? 1U : 0U;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
