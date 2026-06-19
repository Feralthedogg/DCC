#ifndef DCC_JSON_PARSER_FUNCTIONS_H
#define DCC_JSON_PARSER_FUNCTIONS_H

#include <dcc/error.h>

#include "internal/json/dcc_json_parser_state.h"

dcc_json_t *dcc_json_new(dcc_json_parser_t *parser, dcc_json_type_t type);
dcc_status_t dcc_json_parse_value(dcc_json_parser_t *parser, dcc_json_t **out);
dcc_status_t dcc_json_parse_array(dcc_json_parser_t *parser, dcc_json_t **out);
dcc_status_t dcc_json_parse_object(dcc_json_parser_t *parser, dcc_json_t **out);

#endif
