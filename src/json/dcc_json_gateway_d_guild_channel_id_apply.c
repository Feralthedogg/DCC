#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_apply_d_guild_channel_id_key(
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    uint64_t value,
    int *handled
) {
    if (out == NULL || handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    *handled = 1;
    if (key_id == DCC_JSON_KEY_OWNER_ID) {
        out->gateway.owner_id = value;
        out->guild.owner_id = value;
    } else if (key_id == DCC_JSON_KEY_AFK_CHANNEL_ID) {
        out->gateway.afk_channel_id = value;
        out->guild.afk_channel_id = value;
    } else if (key_id == DCC_JSON_KEY_SYSTEM_CHANNEL_ID) {
        out->gateway.system_channel_id = value;
        out->guild.system_channel_id = value;
    } else if (key_id == DCC_JSON_KEY_RULES_CHANNEL_ID) {
        out->gateway.rules_channel_id = value;
        out->guild.rules_channel_id = value;
    } else if (key_id == DCC_JSON_KEY_PUBLIC_UPDATES_CHANNEL_ID) {
        out->gateway.public_updates_channel_id = value;
        out->guild.public_updates_channel_id = value;
    } else if (key_id == DCC_JSON_KEY_WIDGET_CHANNEL_ID) {
        out->gateway.widget_channel_id = value;
        out->guild.widget_channel_id = value;
    } else if (key_id == DCC_JSON_KEY_SAFETY_ALERTS_CHANNEL_ID) {
        out->gateway.safety_alerts_channel_id = value;
        out->guild.safety_alerts_channel_id = value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    out->has_guild = 1;
    return DCC_OK;
}
