#ifndef DCC_JSON_GATEWAY_RESOLVED_MESSAGE_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_RESOLVED_MESSAGE_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resolved_buffers.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_object_fields(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    const dcc_json_gateway_resolved_message_buffers_t *buffers
);

#endif
