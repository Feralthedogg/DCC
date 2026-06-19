#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_creator_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers,
    int *handled
) {
    if (handled == NULL || buffers == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (key_id != DCC_JSON_KEY_CREATOR) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_gateway_parse_user_object(
        parser,
        &event->creator,
        buffers->creator_username,
        buffers->creator_username_cap,
        buffers->creator_global_name,
        buffers->creator_global_name_cap,
        buffers->creator_avatar,
        buffers->creator_avatar_cap
    );
    if (status != DCC_OK) {
        return status;
    }
    if (event->creator_id == 0 && event->creator.id != 0) {
        event->creator_id = event->creator.id;
    }

    *handled = 1;
    return DCC_OK;
}
