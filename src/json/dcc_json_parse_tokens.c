#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

dcc_status_t dcc_json_parse_bool_direct(dcc_json_parser_t *parser, int *out) {
    if (dcc_json_match_literal(parser, "true") == 0) {
        *out = 1;
        return DCC_OK;
    }
    if (dcc_json_match_literal(parser, "false") == 0) {
        *out = 0;
        return DCC_OK;
    }
    return DCC_ERR_JSON;
}
dcc_status_t dcc_json_parse_u64_token(dcc_json_parser_t *parser, uint64_t *out) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }

    if (*parser->cur != '"') {
        return dcc_json_parse_u64_direct(parser, out);
    }

    const char *start = NULL;
    const char *end = NULL;
    bool escaped = false;
    if (dcc_json_find_string_end(parser, &start, &end, &escaped) != 0) {
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
dcc_status_t dcc_json_parse_nullable_u64_token(
    dcc_json_parser_t *parser,
    uint64_t *out,
    int *has_value
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || out == NULL || has_value == NULL) {
        return DCC_ERR_JSON;
    }

    if (*parser->cur == 'n') {
        if (dcc_json_match_literal(parser, "null") != 0) {
            return DCC_ERR_JSON;
        }
        *out = 0;
        *has_value = 0;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_parse_u64_token(parser, out);
    if (status != DCC_OK) {
        return status;
    }
    *has_value = 1;
    return DCC_OK;
}
dcc_status_t dcc_json_parse_nullable_u32_token(
    dcc_json_parser_t *parser,
    uint32_t *out,
    int *has_value
) {
    uint64_t value = 0;
    int present = 0;
    dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &present);
    if (status != DCC_OK) {
        return status;
    }
    if (present && value > UINT32_MAX) {
        return DCC_ERR_JSON;
    }
    *out = present ? (uint32_t)value : 0U;
    if (has_value != NULL) {
        *has_value = present;
    }
    return DCC_OK;
}
