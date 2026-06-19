#ifndef DCC_JSON_GATEWAY_D_MESSAGE_TIMESTAMP_FIELDS_H
#define DCC_JSON_GATEWAY_D_MESSAGE_TIMESTAMP_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

dcc_status_t dcc_json_gateway_parse_d_message_timestamp_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);

#endif
