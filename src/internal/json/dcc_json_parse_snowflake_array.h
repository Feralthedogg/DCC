#ifndef DCC_JSON_PARSE_SNOWFLAKE_ARRAY_H
#define DCC_JSON_PARSE_SNOWFLAKE_ARRAY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_parse_snowflake_array(
    dcc_json_parser_t *parser,
    dcc_snowflake_t *items,
    size_t cap,
    const dcc_snowflake_t **out_items,
    size_t *out_count,
    uint8_t *out_truncated
);

#endif
