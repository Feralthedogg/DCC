#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>
#include <stddef.h>

dcc_status_t dcc_json_skip_value(dcc_json_parser_t *parser) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }

    char c = *parser->cur;
    if (c == '{') {
        return dcc_json_skip_object(parser);
    }
    if (c == '[') {
        return dcc_json_skip_array(parser);
    }
    if (c == '"') {
        const char *start = NULL;
        const char *end = NULL;
        bool escaped = false;
        if (dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
            return DCC_ERR_JSON;
        }
        (void)start;
        (void)escaped;
        parser->cur = end + 1;
        return DCC_OK;
    }
    if (c == '-' || (c >= '0' && c <= '9')) {
        const char *start = NULL;
        size_t len = 0;
        bool integer = true;
        return dcc_json_parse_number_span(parser, &start, &len, &integer);
    }
    if (c == 't') {
        return dcc_json_match_literal(parser, "true") == 0 ? DCC_OK : DCC_ERR_JSON;
    }
    if (c == 'f') {
        return dcc_json_match_literal(parser, "false") == 0 ? DCC_OK : DCC_ERR_JSON;
    }
    if (c == 'n') {
        return dcc_json_match_literal(parser, "null") == 0 ? DCC_OK : DCC_ERR_JSON;
    }
    return DCC_ERR_JSON;
}
