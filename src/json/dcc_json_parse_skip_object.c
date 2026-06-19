#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>

dcc_status_t dcc_json_skip_object(dcc_json_parser_t *parser) {
    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }
    for (;;) {
        char key[2];
        if (dcc_json_parse_string_buffer(parser, key, sizeof(key)) != 0 && parser->cur < parser->end) {
            const char *start = NULL;
            const char *end = NULL;
            bool escaped = false;
            if (dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
                return DCC_ERR_JSON;
            }
            (void)escaped;
            parser->cur = end + 1;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end || *parser->cur != ':') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_status_t status = dcc_json_skip_value(parser);
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
    }
}
