#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_guild_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_GUILD_ID) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }

    dcc_json_gateway_apply_d_core_guild_id(out, value);
    dcc_json_gateway_mark_d_core_guild_id(out);
    (void)has_value;

    *handled = 1;
    return DCC_OK;
}
