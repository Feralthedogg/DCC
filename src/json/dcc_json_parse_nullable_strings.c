#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

dcc_status_t dcc_json_parse_nullable_string_buffer(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *has_value
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || out == NULL || out_cap == 0 || has_value == NULL) {
        return DCC_ERR_JSON;
    }

    if (*parser->cur == 'n') {
        if (dcc_json_match_literal(parser, "null") != 0) {
            return DCC_ERR_JSON;
        }
        out[0] = '\0';
        *has_value = 0;
        return DCC_OK;
    }

    if (dcc_json_parse_string_buffer(parser, out, out_cap) != 0) {
        return DCC_ERR_JSON;
    }
    *has_value = 1;
    return DCC_OK;
}

dcc_status_t dcc_json_parse_nullable_string_or_number_buffer(
    dcc_json_parser_t *parser,
    char *out,
    size_t out_cap,
    int *has_value
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || out == NULL || out_cap == 0 || has_value == NULL) {
        return DCC_ERR_JSON;
    }

    if (*parser->cur == 'n' || *parser->cur == '"') {
        return dcc_json_parse_nullable_string_buffer(parser, out, out_cap, has_value);
    }

    if (*parser->cur == '-' || (*parser->cur >= '0' && *parser->cur <= '9')) {
        const char *start = NULL;
        size_t len = 0;
        bool integer = true;
        dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
        (void)integer;
        if (status != DCC_OK) {
            return status;
        }
        if (len >= out_cap) {
            return DCC_ERR_JSON;
        }
        memcpy(out, start, len);
        out[len] = '\0';
        *has_value = 1;
        return DCC_OK;
    }

    return DCC_ERR_JSON;
}
