#ifndef DCC_JSON_GATEWAY_INTERACTION_OPTION_NUMBER_TEXT_H
#define DCC_JSON_GATEWAY_INTERACTION_OPTION_NUMBER_TEXT_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_parse_number_text(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *integer_out
);

#endif
