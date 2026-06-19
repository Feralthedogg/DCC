#ifndef DCC_JSON_PARSE_NESTED_USER_ID_H
#define DCC_JSON_PARSE_NESTED_USER_ID_H

#include "internal/json/dcc_json_core.h"

#include <stdint.h>

dcc_status_t dcc_json_parse_nested_user_id(
    dcc_json_parser_t *parser,
    uint64_t *out,
    int *has_value
);

#endif
