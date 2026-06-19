#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_guild_media_text_key(
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
        case DCC_JSON_KEY_VANITY_URL_CODE:
            buffer = out->guild_vanity_url_code;
            buffer_cap = sizeof(out->guild_vanity_url_code);
            guild_field = &out->guild.vanity_url_code;
            gateway_field = &out->gateway.vanity_url_code;
            break;
        case DCC_JSON_KEY_PREFERRED_LOCALE:
            buffer = out->guild_preferred_locale;
            buffer_cap = sizeof(out->guild_preferred_locale);
            guild_field = &out->guild.preferred_locale;
            gateway_field = &out->gateway.preferred_locale;
            break;
        case DCC_JSON_KEY_SPLASH:
            buffer = out->guild_splash;
            buffer_cap = sizeof(out->guild_splash);
            guild_field = &out->guild.splash;
            gateway_field = &out->gateway.splash;
            break;
        case DCC_JSON_KEY_DISCOVERY_SPLASH:
            buffer = out->guild_discovery_splash;
            buffer_cap = sizeof(out->guild_discovery_splash);
            guild_field = &out->guild.discovery_splash;
            gateway_field = &out->gateway.discovery_splash;
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
