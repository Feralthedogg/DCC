#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_primary_resource_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_ROLE_ID &&
        key_id != DCC_JSON_KEY_PARENT_ID &&
        key_id != DCC_JSON_KEY_TARGET_ID) {
        *handled = 0;
        return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_ROLE_ID) {
        out->gateway.role_id = value;
        out->role.id = value;
        out->integration.role_id = value;
        out->has_integration = 1;
    } else if (key_id == DCC_JSON_KEY_PARENT_ID) {
        out->channel.parent_id = value;
        out->gateway.parent_id = value;
        out->has_channel = 1;
        out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_PARENT_ID;
    } else {
        out->gateway.target_id = value;
        out->interaction.target_id = value;
        out->invite.target_id = value;
        out->guild_audit_log_entry.target_id = value;
        out->has_interaction = 1;
        out->has_invite = 1;
        out->has_guild_audit_log_entry = 1;
    }

    (void)has_value;
    *handled = 1;
    return DCC_OK;
}
