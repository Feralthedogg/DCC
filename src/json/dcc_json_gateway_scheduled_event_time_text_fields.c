#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_time_text_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || event == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    char *target = NULL;
    size_t target_cap = 0;
    const char **field = NULL;
    if (key_id == DCC_JSON_KEY_SCHEDULED_START_TIME ||
        key_id == DCC_JSON_KEY_STARTS_AT) {
        target = buffers->start_time;
        target_cap = buffers->start_time_cap;
        field = &event->start_time;
    } else if (key_id == DCC_JSON_KEY_SCHEDULED_END_TIME ||
               key_id == DCC_JSON_KEY_ENDS_AT) {
        target = buffers->end_time;
        target_cap = buffers->end_time_cap;
        field = &event->end_time;
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
