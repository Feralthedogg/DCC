#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_role_id_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (parser == NULL || out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_ROLES &&
        key_id != DCC_JSON_KEY_ROLE_IDS &&
        key_id != DCC_JSON_KEY_EXEMPT_ROLES) {
        *handled = 0;
        return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_ROLES &&
        dcc_json_array_first_value_is_object(parser)) {
        dcc_status_t status = dcc_json_gateway_parse_guild_role_array(parser, out);
        if (status != DCC_OK) {
            return status;
        }
        *handled = 1;
        return DCC_OK;
    }

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
    out->has_member = 1;
    out->cache_fields |= DCC_CACHE_FIELD_MEMBER_ROLES;
    out->auto_moderation_rule.role_ids = out->gateway.role_ids;
    out->auto_moderation_rule.role_ids_count = out->gateway.role_ids_count;
    out->auto_moderation_rule.role_ids_truncated = out->gateway.role_ids_truncated;
    out->has_auto_moderation_rule = 1;
    *handled = 1;
    return DCC_OK;
}
