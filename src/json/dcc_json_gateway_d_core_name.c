#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_name_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_NAME) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        out->guild_name,
        sizeof(out->guild_name),
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    out->guild.name = has_value ? out->guild_name : NULL;
    out->channel.name = has_value ? out->guild_name : NULL;
    out->interaction.name = has_value ? out->guild_name : NULL;
    out->scheduled_event.name = has_value ? out->guild_name : NULL;
    out->soundboard_sound.name = has_value ? out->guild_name : NULL;
    out->auto_moderation_rule.name = has_value ? out->guild_name : NULL;
    out->integration.name = has_value ? out->guild_name : NULL;
    out->gateway.name = has_value ? out->guild_name : NULL;
    out->has_guild = 1;
    out->has_channel = 1;
    out->has_interaction = 1;
    out->has_scheduled_event = 1;
    out->has_soundboard_sound = 1;
    out->has_auto_moderation_rule = 1;
    out->has_integration = 1;
    out->cache_fields |= DCC_CACHE_FIELD_CHANNEL_NAME;
    *handled = 1;
    return DCC_OK;
}
