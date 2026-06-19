#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_interaction_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_TOKEN) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->interaction_token,
            sizeof(out->interaction_token),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.token = has_value ? out->interaction_token : NULL;
        out->gateway.token = has_value ? out->interaction_token : NULL;
        out->voice_server_update.token = has_value ? out->interaction_token : NULL;
        out->has_interaction = 1;
        out->has_voice_server_update = 1;
    } else if (key_id == DCC_JSON_KEY_LOCALE) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->interaction_locale,
            sizeof(out->interaction_locale),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.locale = has_value ? out->interaction_locale : NULL;
        out->has_interaction = 1;
    } else if (key_id == DCC_JSON_KEY_GUILD_LOCALE) {
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->interaction_guild_locale,
            sizeof(out->interaction_guild_locale),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->interaction.guild_locale = has_value ? out->interaction_guild_locale : NULL;
        out->has_interaction = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
