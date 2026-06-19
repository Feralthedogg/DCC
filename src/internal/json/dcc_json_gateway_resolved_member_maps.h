#ifndef DCC_JSON_GATEWAY_RESOLVED_MEMBER_MAPS_H
#define DCC_JSON_GATEWAY_RESOLVED_MEMBER_MAPS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_gateway_parse_resolved_member_map(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
);
dcc_status_t dcc_json_gateway_parse_resolved_member_map_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated,
    uint8_t *permissions_truncated
);
dcc_status_t dcc_json_gateway_parse_resolved_member_entry(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
);
void dcc_json_gateway_finish_resolved_member_map(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated,
    uint8_t permissions_truncated
);

#endif
