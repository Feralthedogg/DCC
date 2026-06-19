#ifndef DCC_JSON_PARSE_SKIP_HELPERS_H
#define DCC_JSON_PARSE_SKIP_HELPERS_H

#include "internal/json/dcc_json_core.h"

dcc_status_t dcc_json_skip_array(dcc_json_parser_t *parser);
dcc_status_t dcc_json_skip_object(dcc_json_parser_t *parser);
dcc_status_t dcc_json_skip_value(dcc_json_parser_t *parser);

#endif
