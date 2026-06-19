#ifndef DCC_JSON_PARSE_OBJECT_ID_H
#define DCC_JSON_PARSE_OBJECT_ID_H

#include "internal/json/dcc_json_core.h"

#include <stdint.h>

dcc_status_t dcc_json_parse_object_id(
    dcc_json_parser_t *parser,
    int prefer_user_id,
    uint64_t *out,
    int *has_value
);

#endif
