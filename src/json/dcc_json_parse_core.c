#include "internal/json/dcc_json_internal.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void dcc_json_skip_ws(dcc_json_parser_t *parser) {
    while (parser->cur < parser->end) {
        unsigned char c = (unsigned char)*parser->cur;
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            break;
        }
        parser->cur++;
    }
}
static const char *dcc_json_skip_ws_span(const char *cur, const char *end) {
    while (cur < end) {
        unsigned char c = (unsigned char)*cur;
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            break;
        }
        cur++;
    }
    return cur;
}
int dcc_json_array_first_value_is_object(const dcc_json_parser_t *parser) {
    if (parser == NULL) {
        return 0;
    }
    const char *cur = dcc_json_skip_ws_span(parser->cur, parser->end);
    if (cur >= parser->end || *cur != '[') {
        return 0;
    }
    cur = dcc_json_skip_ws_span(cur + 1, parser->end);
    return cur < parser->end && *cur == '{';
}
int dcc_json_parse_gateway_key(dcc_json_parser_t *parser, dcc_json_key_id_t *out) {
    const char *start = NULL;
    const char *end = NULL;
    bool escaped = false;
    if (out == NULL || dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
        return -1;
    }

    dcc_json_key_id_t key_id = DCC_JSON_KEY_UNKNOWN;
    if (!escaped) {
        key_id = dcc_json_gateway_key_id(start, (size_t)(end - start));
    } else {
        char key[64];
        if (dcc_json_decode_string_span(start, end, key, sizeof(key)) != 0) {
            return -1;
        }
        key_id = dcc_json_gateway_key_id(key, strlen(key));
    }
    parser->cur = end + 1;
    *out = key_id;
    return 0;
}

dcc_status_t dcc_json_parse_snowflake_map_key(dcc_json_parser_t *parser, dcc_snowflake_t *out) {
    const char *start = NULL;
    const char *end = NULL;
    bool escaped = false;
    if (out == NULL || dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
        return DCC_ERR_JSON;
    }

    char decoded[32];
    const char *digits = start;
    size_t len = (size_t)(end - start);
    if (escaped) {
        if (dcc_json_decode_string_span(start, end, decoded, sizeof(decoded)) != 0) {
            return DCC_ERR_JSON;
        }
        digits = decoded;
        len = strlen(decoded);
    } else if (len >= sizeof(decoded)) {
        return DCC_ERR_JSON;
    }

    uint64_t value = 0;
    if (dcc_json_parse_u64_ascii(digits, len, &value) != 0) {
        return DCC_ERR_JSON;
    }

    parser->cur = end + 1;
    *out = value;
    return DCC_OK;
}
int dcc_json_match_literal(dcc_json_parser_t *parser, const char *literal) {
    size_t len = strlen(literal);
    if ((size_t)(parser->end - parser->cur) < len || memcmp(parser->cur, literal, len) != 0) {
        return -1;
    }
    parser->cur += len;
    return 0;
}
