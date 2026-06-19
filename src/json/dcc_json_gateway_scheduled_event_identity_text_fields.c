#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_identity_text_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || event == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id == DCC_JSON_KEY_ENTITY_METADATA) {
        const char *parsed_location = NULL;
        dcc_status_t status = dcc_json_parse_entity_metadata_location(
            parser,
            buffers->location,
            buffers->location_cap,
            &parsed_location
        );
        if (status != DCC_OK) {
            return status;
        }
        event->location = parsed_location;
        *handled = 1;
        return DCC_OK;
    }

    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;
    if (key_id == DCC_JSON_KEY_NAME) {
        target = buffers->name;
        target_cap = buffers->name_cap;
        field = &event->name;
    } else if (key_id == DCC_JSON_KEY_DESCRIPTION) {
        target = buffers->description;
        target_cap = buffers->description_cap;
        field = &event->description;
    } else if (key_id == DCC_JSON_KEY_IMAGE) {
        target = buffers->image;
        target_cap = buffers->image_cap;
        field = &event->image;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    int has_value = 0;
    dcc_status_t status = dcc_json_parse_nullable_string_buffer(
        parser,
        target,
        target_cap,
        &has_value
    );
    if (status != DCC_OK) {
        return status;
    }
    *field = has_value ? target : NULL;
    *handled = 1;
    return DCC_OK;
}
