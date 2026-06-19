#ifndef DCC_JSON_GATEWAY_SCHEDULED_EVENT_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_SCHEDULED_EVENT_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resource_buffers.h"

dcc_status_t dcc_json_gateway_parse_scheduled_event_object_fields(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    const dcc_json_gateway_scheduled_event_buffers_t *buffers
);

#endif
