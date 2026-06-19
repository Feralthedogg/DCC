#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_guild_bool_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_UNAVAILABLE &&
        key_id != DCC_JSON_KEY_LARGE &&
        key_id != DCC_JSON_KEY_WIDGET_ENABLED &&
        key_id != DCC_JSON_KEY_PREMIUM_PROGRESS_BAR_ENABLED) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_bool_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_UNAVAILABLE) {
        out->guild.unavailable = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_GUILD_UNAVAILABLE;
    } else if (key_id == DCC_JSON_KEY_LARGE) {
        out->guild.large = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_GUILD_LARGE;
    } else if (key_id == DCC_JSON_KEY_WIDGET_ENABLED) {
        out->gateway.widget_enabled = value ? 1U : 0U;
        out->guild.widget_enabled = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_GUILD_WIDGET_ENABLED;
    } else {
        out->gateway.premium_progress_bar_enabled = value ? 1U : 0U;
        out->guild.premium_progress_bar_enabled = value ? 1U : 0U;
        out->cache_fields |= DCC_CACHE_FIELD_GUILD_PREMIUM_PROGRESS_BAR_ENABLED;
    }

    out->has_guild = 1;
    *handled = 1;
    return DCC_OK;
}
