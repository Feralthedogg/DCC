#include "internal/json/dcc_json_internal.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_json_parse_i64_direct(dcc_json_parser_t *parser, int64_t *out) {
    const char *start = NULL;
    size_t len = 0;
    bool integer = true;
    dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
    if (status != DCC_OK) {
        return status;
    }
    if (!integer || len == 0 || len >= 64U) {
        return DCC_ERR_JSON;
    }
    if (dcc_json_parse_i64_ascii(start, len, out) != 0) {
        return DCC_ERR_JSON;
    }
    return DCC_OK;
}

dcc_status_t dcc_json_parse_u64_direct(dcc_json_parser_t *parser, uint64_t *out) {
    const char *start = NULL;
    size_t len = 0;
    bool integer = true;
    dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
    if (status != DCC_OK) {
        return status;
    }
    if (!integer || len == 0 || len >= 64U || start[0] == '-') {
        return DCC_ERR_JSON;
    }
    if (dcc_json_parse_u64_ascii(start, len, out) != 0) {
        return DCC_ERR_JSON;
    }
    return DCC_OK;
}

dcc_status_t dcc_json_parse_u32_direct(dcc_json_parser_t *parser, uint32_t *out) {
    uint64_t value = 0;
    dcc_status_t status = dcc_json_parse_u64_direct(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    if (value > UINT32_MAX) {
        return DCC_ERR_JSON;
    }
    *out = (uint32_t)value;
    return DCC_OK;
}

dcc_status_t dcc_json_parse_double_direct(dcc_json_parser_t *parser, double *out) {
    const char *start = NULL;
    size_t len = 0;
    bool integer = true;
    dcc_status_t status = dcc_json_parse_number_span(parser, &start, &len, &integer);
    (void)integer;
    if (status != DCC_OK) {
        return status;
    }
    if (len == 0 || len >= 64U) {
        return DCC_ERR_JSON;
    }

    char buf[64];
    memcpy(buf, start, len);
    buf[len] = '\0';
    errno = 0;
    char *end = NULL;
    double value = strtod(buf, &end);
    if (errno != 0 || end == NULL || *end != '\0') {
        return DCC_ERR_JSON;
    }

    *out = value;
    return DCC_OK;
}

dcc_status_t dcc_json_parse_u16_token(dcc_json_parser_t *parser, uint16_t *out) {
    uint64_t value = 0;
    dcc_status_t status = dcc_json_parse_u64_token(parser, &value);
    if (status != DCC_OK) {
        return status;
    }
    if (value > UINT16_MAX) {
        return DCC_ERR_JSON;
    }
    *out = (uint16_t)value;
    return DCC_OK;
}
