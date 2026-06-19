#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>
#include <string.h>

int dcc_json_parse_string_arena(dcc_json_parser_t *parser, char **out) {
    const char *start = NULL;
    const char *end = NULL;
    bool escaped = false;
    if (dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
        return -1;
    }

    size_t len = (size_t)(end - start);
    if (len == (size_t)-1) {
        return -1;
    }
    char *copy = (char *)dcc_json_arena_alloc(&parser->doc->arena, len + 1U);
    if (copy == NULL) {
        return -1;
    }
    if (!escaped) {
        memcpy(copy, start, len);
        copy[len] = '\0';
    } else if (dcc_json_decode_string_span(start, end, copy, len + 1U) != 0) {
        return -1;
    }

    parser->cur = end + 1;
    *out = copy;
    return 0;
}

int dcc_json_parse_string_buffer(dcc_json_parser_t *parser, char *out, size_t out_cap) {
    const char *start = NULL;
    const char *end = NULL;
    bool escaped = false;
    if (out == NULL || out_cap == 0 || dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
        return -1;
    }

    size_t len = (size_t)(end - start);
    if (!escaped) {
        if (len >= out_cap) {
            return -1;
        }
        memcpy(out, start, len);
        out[len] = '\0';
    } else if (dcc_json_decode_string_span(start, end, out, out_cap) != 0) {
        return -1;
    }

    parser->cur = end + 1;
    return 0;
}
