#ifndef DCC_JSON_GATEWAY_RESOLVED_MESSAGE_TEXT_FIELDS_H
#define DCC_JSON_GATEWAY_RESOLVED_MESSAGE_TEXT_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_resolved_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_resolved_message_text_field(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_resolved_message_buffers_t *buffers,
    int *handled
);

#endif
