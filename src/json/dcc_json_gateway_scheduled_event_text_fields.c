#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_text_field(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || event == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_status_t status = dcc_json_gateway_parse_scheduled_event_identity_text_field(
        parser,
        event,
        key_id,
        buffers,
        handled
    );
    if (status != DCC_OK || *handled) {
        return status;
    }

    return dcc_json_gateway_parse_scheduled_event_time_text_field(
        parser,
        event,
        key_id,
        buffers,
        handled
    );
}
