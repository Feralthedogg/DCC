#ifndef DCC_JSON_GATEWAY_RESOLVED_MAP_CORE_H
#define DCC_JSON_GATEWAY_RESOLVED_MAP_CORE_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_payload.h"

#include <stddef.h>
#include <stdint.h>

typedef dcc_status_t (*dcc_json_gateway_resolved_entry_parse_fn)(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_snowflake_t key,
    size_t index
);
typedef void (*dcc_json_gateway_resolved_map_finish_fn)(
    dcc_json_gateway_payload_t *out,
    size_t stored,
    uint8_t truncated
);
dcc_status_t dcc_json_gateway_parse_resolved_object_map(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_gateway_resolved_entry_parse_fn parse_entry,
    dcc_json_gateway_resolved_map_finish_fn finish_map
);
dcc_status_t dcc_json_gateway_parse_resolved_object_map_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_gateway_resolved_entry_parse_fn parse_entry,
    size_t *stored,
    uint8_t *truncated
);

#endif
