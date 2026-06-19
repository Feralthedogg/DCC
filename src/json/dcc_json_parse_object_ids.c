#include "internal/json/dcc_json_internal.h"

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
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '[' ||
        items == NULL || out_items == NULL || out_count == NULL || out_truncated == NULL) {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    size_t stored = 0;
    uint8_t truncated = 0;
    if (out_total_count != NULL) {
        *out_total_count = 0;
    }

    if (parser->cur < parser->end && *parser->cur == ']') {
        parser->cur++;
        *out_items = items;
        *out_count = 0;
        *out_truncated = 0;
        return DCC_OK;
    }

    for (;;) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_object_id(parser, prefer_user_id, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        if (has_value) {
            if (out_total_count != NULL) {
                (*out_total_count)++;
            }
            if (stored < cap) {
                items[stored++] = value;
            } else {
                truncated = 1;
            }
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == ']') {
            parser->cur++;
            *out_items = items;
            *out_count = stored;
            *out_truncated = truncated;
            if (out_total_count != NULL && *out_total_count < stored) {
                *out_total_count = stored;
            }
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
