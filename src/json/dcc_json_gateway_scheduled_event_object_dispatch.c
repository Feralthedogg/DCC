#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_object_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers
) {
    int handled = 0;
    dcc_status_t status = dcc_json_gateway_parse_scheduled_event_id_field(
        parser,
        event,
        key_id,
        &handled
    );
    if (status != DCC_OK) {
        return status;
    }
    if (!handled) {
        status = dcc_json_gateway_parse_scheduled_event_numeric_field(
            parser,
            event,
            key_id,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_scheduled_event_text_field(
            parser,
            event,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_gateway_parse_scheduled_event_creator_field(
            parser,
            event,
            key_id,
            buffers,
            &handled
        );
        if (status != DCC_OK) {
            return status;
        }
    }
    if (!handled) {
        status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
