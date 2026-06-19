#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_guild_icon_banner_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    char *buffer = NULL;
    size_t buffer_cap = 0;
    const char **guild_field = NULL;
    const char **gateway_field = NULL;

    switch (key_id) {
        case DCC_JSON_KEY_ICON:
            buffer = out->guild_icon;
            buffer_cap = sizeof(out->guild_icon);
            guild_field = &out->guild.icon;
            gateway_field = &out->gateway.icon;
            break;
        case DCC_JSON_KEY_BANNER:
            buffer = out->guild_banner;
            buffer_cap = sizeof(out->guild_banner);
            guild_field = &out->guild.banner;
            gateway_field = &out->gateway.banner;
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        buffer,
        buffer_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }

    *guild_field = has_value ? buffer : NULL;
    *gateway_field = has_value ? buffer : NULL;
    out->has_guild = 1;
    *handled = 1;
    return DCC_OK;
}
