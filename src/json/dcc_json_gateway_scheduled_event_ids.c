#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_id_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint64_t value = 0;
    int has_value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_ID) {
        dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
        if (status != DCC_OK) {
            return status;
        }
        event->id = value;
    } else if (key_id == DCC_JSON_KEY_GUILD_ID ||
               key_id == DCC_JSON_KEY_CHANNEL_ID ||
               key_id == DCC_JSON_KEY_CREATOR_ID ||
               key_id == DCC_JSON_KEY_ENTITY_ID) {
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (key_id == DCC_JSON_KEY_GUILD_ID) {
            event->guild_id = has_value ? value : 0;
        } else if (key_id == DCC_JSON_KEY_CHANNEL_ID) {
            event->channel_id = has_value ? value : 0;
        } else if (key_id == DCC_JSON_KEY_CREATOR_ID) {
            event->creator_id = has_value ? value : 0;
        } else {
            event->entity_id = has_value ? value : 0;
        }
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
