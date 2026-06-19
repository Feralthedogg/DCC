#include "internal/json/dcc_json_internal.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>

static int dcc_json_is_number_char(char c) {
    return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E';
}

dcc_status_t dcc_json_parse_number_span(
    dcc_json_parser_t *parser,
    const char **start_out,
    size_t *len_out,
    bool *integer_out
) {
    const char *start = parser->cur;
    bool integer = true;

    if (parser->cur < parser->end && *parser->cur == '-') {
        parser->cur++;
    }
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur == '0') {
        parser->cur++;
    } else if (*parser->cur >= '1' && *parser->cur <= '9') {
        while (parser->cur < parser->end && *parser->cur >= '0' && *parser->cur <= '9') {
            parser->cur++;
        }
    } else {
        return DCC_ERR_JSON;
    }
    if (parser->cur < parser->end && *parser->cur == '.') {
        integer = false;
        parser->cur++;
        if (parser->cur >= parser->end || !isdigit((unsigned char)*parser->cur)) {
            return DCC_ERR_JSON;
        }
        while (parser->cur < parser->end && isdigit((unsigned char)*parser->cur)) {
            parser->cur++;
        }
    }
    if (parser->cur < parser->end && (*parser->cur == 'e' || *parser->cur == 'E')) {
        integer = false;
        parser->cur++;
        if (parser->cur < parser->end && (*parser->cur == '+' || *parser->cur == '-')) {
            parser->cur++;
        }
        if (parser->cur >= parser->end || !isdigit((unsigned char)*parser->cur)) {
            return DCC_ERR_JSON;
        }
        while (parser->cur < parser->end && isdigit((unsigned char)*parser->cur)) {
            parser->cur++;
        }
    }
    if (parser->cur < parser->end && dcc_json_is_number_char(*parser->cur)) {
        return DCC_ERR_JSON;
    }

    *start_out = start;
    *len_out = (size_t)(parser->cur - start);
    *integer_out = integer;
    return DCC_OK;
}
