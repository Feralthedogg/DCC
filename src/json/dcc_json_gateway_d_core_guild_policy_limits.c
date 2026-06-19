#include "internal/json/dcc_json_internal.h"

static int dcc_json_gateway_is_policy_limit_key(dcc_json_key_id_t key_id) {
    return key_id == DCC_JSON_KEY_DEFAULT_MESSAGE_NOTIFICATIONS ||
        key_id == DCC_JSON_KEY_PREMIUM_TIER ||
        key_id == DCC_JSON_KEY_VERIFICATION_LEVEL ||
        key_id == DCC_JSON_KEY_EXPLICIT_CONTENT_FILTER ||
        key_id == DCC_JSON_KEY_MFA_LEVEL ||
        key_id == DCC_JSON_KEY_NSFW_LEVEL;
}

dcc_status_t dcc_json_gateway_parse_d_core_guild_policy_limit_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!dcc_json_gateway_is_policy_limit_key(key_id)) {
        *handled = 0;
        return DCC_OK;
    }

    uint32_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u32_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    dcc_json_gateway_apply_d_core_guild_policy_limit(out, key_id, value);
    out->has_guild = 1;
    out->cache_fields |= DCC_CACHE_FIELD_GUILD_LIMITS;
    (void)has_value;
    *handled = 1;
    return DCC_OK;
}
