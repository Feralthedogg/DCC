#ifndef DCC_JSON_PARSE_CORE_HELPERS_H
#define DCC_JSON_PARSE_CORE_HELPERS_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_keys.h"

void dcc_json_skip_ws(dcc_json_parser_t *parser);
int dcc_json_array_first_value_is_object(const dcc_json_parser_t *parser);
int dcc_json_parse_gateway_key(dcc_json_parser_t *parser, dcc_json_key_id_t *out);
dcc_status_t dcc_json_parse_snowflake_map_key(dcc_json_parser_t *parser, dcc_snowflake_t *out);
int dcc_json_match_literal(dcc_json_parser_t *parser, const char *literal);

#endif
