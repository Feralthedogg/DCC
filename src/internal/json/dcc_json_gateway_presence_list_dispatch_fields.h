#ifndef DCC_JSON_GATEWAY_PRESENCE_LIST_DISPATCH_FIELDS_H
#define DCC_JSON_GATEWAY_PRESENCE_LIST_DISPATCH_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_presence_buffers.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_presence_list_identity_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
);
dcc_status_t dcc_json_gateway_parse_presence_list_status_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
);
dcc_status_t dcc_json_gateway_parse_presence_list_activity_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
);
dcc_status_t dcc_json_gateway_parse_presence_list_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
);

#endif
