#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_core_message_scheduled_object_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id == DCC_JSON_KEY_ENTITY_METADATA) {
        const char *location = NULL;
        dcc_status_t status = dcc_json_parse_entity_metadata_location(
            parser,
            out->scheduled_event_location,
            sizeof(out->scheduled_event_location),
            &location
        );
        if (status != DCC_OK) {
            return status;
        }
        out->scheduled_event.location = location;
        out->has_scheduled_event = 1;
    } else if (key_id == DCC_JSON_KEY_CREATOR) {
        dcc_status_t status = dcc_json_gateway_parse_user_object(
            parser,
            &out->scheduled_event.creator,
            out->scheduled_event_creator_username,
            sizeof(out->scheduled_event_creator_username),
            out->scheduled_event_creator_global_name,
            sizeof(out->scheduled_event_creator_global_name),
            out->scheduled_event_creator_avatar,
            sizeof(out->scheduled_event_creator_avatar)
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->scheduled_event.creator_id == 0 && out->scheduled_event.creator.id != 0) {
            out->scheduled_event.creator_id = out->scheduled_event.creator.id;
        }
        out->has_scheduled_event = 1;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
