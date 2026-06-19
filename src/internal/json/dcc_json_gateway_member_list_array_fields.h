#ifndef DCC_JSON_GATEWAY_MEMBER_LIST_ARRAY_FIELDS_H
#define DCC_JSON_GATEWAY_MEMBER_LIST_ARRAY_FIELDS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_guild_member_array(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_guild_member_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t index
);
void dcc_json_gateway_finish_guild_member_array(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
);

#endif
