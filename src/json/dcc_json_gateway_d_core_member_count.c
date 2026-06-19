#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_member_count_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_MEMBER_COUNT) {
        *handled = 0;
        return DCC_OK;
    }

    uint32_t value = 0;
    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    out->guild.member_count = value;
    out->gateway.count = value;
    out->thread_members_update.count = value;
    out->has_guild = 1;
    out->has_thread_members_update = 1;
    out->cache_fields |= DCC_CACHE_FIELD_GUILD_MEMBER_COUNT;
    *handled = 1;
    return DCC_OK;
}
