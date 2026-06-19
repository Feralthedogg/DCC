#ifndef DCC_JSON_PARSE_OBJECT_ID_ARRAY_H
#define DCC_JSON_PARSE_OBJECT_ID_ARRAY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_parse_object_id_array(
    dcc_json_parser_t *parser,
    int prefer_user_id,
    dcc_snowflake_t *items,
    size_t cap,
    const dcc_snowflake_t **out_items,
    size_t *out_count,
    uint8_t *out_truncated,
    size_t *out_total_count
);

#endif
