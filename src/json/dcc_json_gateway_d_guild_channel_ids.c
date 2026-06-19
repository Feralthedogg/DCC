#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_guild_channel_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (key_id) {
        case DCC_JSON_KEY_OWNER_ID:
        case DCC_JSON_KEY_AFK_CHANNEL_ID:
        case DCC_JSON_KEY_SYSTEM_CHANNEL_ID:
        case DCC_JSON_KEY_RULES_CHANNEL_ID:
        case DCC_JSON_KEY_PUBLIC_UPDATES_CHANNEL_ID:
        case DCC_JSON_KEY_WIDGET_CHANNEL_ID:
        case DCC_JSON_KEY_SAFETY_ALERTS_CHANNEL_ID:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    uint64_t value = 0;
    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
    if (status != DCC_OK) {
        return status;
    }
    (void)has_value;
    return dcc_json_gateway_apply_d_guild_channel_id_key(out, key_id, value, handled);
}
