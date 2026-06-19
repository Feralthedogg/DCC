#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_numeric_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    int *handled
) {
    uint32_t value = 0;

    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    if (key_id != DCC_JSON_KEY_PRIVACY_LEVEL &&
        key_id != DCC_JSON_KEY_ENTITY_TYPE &&
        key_id != DCC_JSON_KEY_STATUS &&
        key_id != DCC_JSON_KEY_USER_COUNT) {
        *handled = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_u32_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }

    if (key_id == DCC_JSON_KEY_PRIVACY_LEVEL) {
        event->privacy_level = value;
    } else if (key_id == DCC_JSON_KEY_ENTITY_TYPE) {
        event->entity_type = value;
    } else if (key_id == DCC_JSON_KEY_STATUS) {
        event->status = value;
    } else {
        event->user_count = value;
    }

    *handled = 1;
    return DCC_OK;
}
