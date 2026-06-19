#ifndef DCC_JSON_PARSE_STRING_HELPERS_H
#define DCC_JSON_PARSE_STRING_HELPERS_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_parse_nullable_string_buffer(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *has_value
);
dcc_status_t dcc_json_parse_nullable_string_or_number_buffer(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *has_value
);

#endif
