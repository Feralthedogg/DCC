#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_scheduled_text_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_DESCRIPTION) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->guild_description,
            sizeof(out->guild_description),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->guild.description = has_value ? out->guild_description : NULL;
        out->scheduled_event.description = has_value ? out->guild_description : NULL;
        out->gateway.description = has_value ? out->guild_description : NULL;
        out->has_guild = 1;
        out->has_scheduled_event = 1;
    } else if (key_id == DCC_JSON_KEY_IMAGE) {
        dcc_status_t status = dcc_json_parse_nullable_string_buffer(
            parser,
            out->scheduled_event_image,
            sizeof(out->scheduled_event_image),
            &has_value
        );
        if (status != DCC_OK) {
            return status;
        }
        out->scheduled_event.image = has_value ? out->scheduled_event_image : NULL;
        out->has_scheduled_event = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
