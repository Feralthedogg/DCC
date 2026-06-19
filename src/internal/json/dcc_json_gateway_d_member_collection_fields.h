#ifndef DCC_JSON_GATEWAY_D_MEMBER_COLLECTION_FIELDS_H
#define DCC_JSON_GATEWAY_D_MEMBER_COLLECTION_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"
#include "internal/json/dcc_json_keys.h"

dcc_status_t dcc_json_gateway_parse_d_member_collection_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
);
dcc_status_t dcc_json_gateway_parse_d_member_array_collection(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
);
dcc_status_t dcc_json_gateway_parse_d_member_id_collection(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id
);

#endif
