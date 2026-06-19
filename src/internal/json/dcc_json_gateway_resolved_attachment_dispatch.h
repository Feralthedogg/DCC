#ifndef DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_DISPATCH_H
#define DCC_JSON_GATEWAY_RESOLVED_ATTACHMENT_DISPATCH_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resolved_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_attachment_field(
    dcc_json_parser_t *parser,
    dcc_attachment_t *attachment,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_attachment_buffers_t *buffers
);

#endif
