#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_user_field(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_USER) {
        dcc_status_t status = dcc_json_gateway_parse_user(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        out->member.user = out->gateway.user;
        *handled = 1;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_ROLES) {
        dcc_status_t status = dcc_json_parse_snowflake_array(
            parser,
            out->gateway_role_ids,
            DCC_JSON_GATEWAY_ID_LIST_CAP,
            &out->gateway.role_ids,
            &out->gateway.role_ids_count,
            &out->gateway.role_ids_truncated
        );
        if (status != DCC_OK) {
            return status;
        }
        out->member.role_ids = out->gateway.role_ids;
        out->member.role_ids_count = out->gateway.role_ids_count;
        out->member.role_ids_truncated = out->gateway.role_ids_truncated;
        out->cache_fields |= DCC_CACHE_FIELD_MEMBER_ROLES;
        *handled = 1;
        return DCC_OK;
    }

    *handled = 0;
    return DCC_OK;
}
