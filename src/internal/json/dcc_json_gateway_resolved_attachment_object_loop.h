#ifndef DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_OBJECT_LOOP_H
#define DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_OBJECT_LOOP_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resolved_buffers.h"

dcc_status_t dcc_json_gateway_parse_attachment_object_fields(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    const dcc_json_gateway_attachment_buffers_t *buffers
);

#endif
