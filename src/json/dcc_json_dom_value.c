#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>

static dcc_status_t dcc_json_parse_number(dcc_json_parser_t *parser, dcc_json_t **out) {
    const char *start = NULL;
    size_t len = 0;
    bool integer = true;
    dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
    (void)integer;
    if (status != DCC_OK) {
        return status;
    }

    dcc_json_t *json = dcc_json_new(parser, DCC_JSON_NUMBER);
    if (json == NULL) {
        return DCC_ERR_NOMEM;
    }
    json->as.number = dcc_json_arena_copy(&parser->doc->arena, start, len);
    if (json->as.number == NULL) {
        return DCC_ERR_NOMEM;
    }
    *out = json;
    return DCC_OK;
}

dcc_status_t dcc_json_parse_value(dcc_json_parser_t *parser, dcc_json_t **out) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }

    char c = *parser->cur;
    if (c == '{') {
        return dcc_json_parse_object(parser, out);
    }
    if (c == '[') {
        return dcc_json_parse_array(parser, out);
    }
    if (c == '"') {
        dcc_json_t *json = dcc_json_new(parser, DCC_JSON_STRING);
        if (json == NULL) {
            return DCC_ERR_NOMEM;
        }
        if (dcc_json_parse_string_arena(parser, &json->as.string) != 0) {
            return DCC_ERR_JSON;
        }
        *out = json;
        return DCC_OK;
    }
    if (c == '-' || (c >= '0' && c <= '9')) {
        return dcc_json_parse_number(parser, out);
    }
    if (c == 't') {
        if (dcc_json_match_literal(parser, "true") != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_t *json = dcc_json_new(parser, DCC_JSON_BOOL);
        if (json == NULL) {
            return DCC_ERR_NOMEM;
        }
        json->as.boolean = 1;
        *out = json;
        return DCC_OK;
    }
    if (c == 'f') {
        if (dcc_json_match_literal(parser, "false") != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_t *json = dcc_json_new(parser, DCC_JSON_BOOL);
        if (json == NULL) {
            return DCC_ERR_NOMEM;
        }
        json->as.boolean = 0;
        *out = json;
        return DCC_OK;
    }
    if (c == 'n') {
        if (dcc_json_match_literal(parser, "null") != 0) {
            return DCC_ERR_JSON;
        }
        dcc_json_t *json = dcc_json_new(parser, DCC_JSON_NULL);
        if (json == NULL) {
            return DCC_ERR_NOMEM;
        }
        *out = json;
        return DCC_OK;
    }

    return DCC_ERR_JSON;
}
