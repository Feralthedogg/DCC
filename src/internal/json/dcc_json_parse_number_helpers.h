#ifndef DCC_JSON_PARSE_NUMBER_HELPERS_H
#define DCC_JSON_PARSE_NUMBER_HELPERS_H

#include "internal/json/dcc_json_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

dcc_status_t dcc_json_parse_number_span(
    dcc_json_parser_t *parser,
    const char **start_out,
    size_t *len_out,
    bool *integer_out
);
int dcc_json_parse_i64_ascii(const char *data, size_t len, int64_t *out);
int dcc_json_parse_u64_ascii(const char *data, size_t len, uint64_t *out);
dcc_status_t dcc_json_parse_i64_direct(dcc_json_parser_t *parser, int64_t *out);
dcc_status_t dcc_json_parse_u64_direct(dcc_json_parser_t *parser, uint64_t *out);
dcc_status_t dcc_json_parse_bool_direct(dcc_json_parser_t *parser, int *out);
dcc_status_t dcc_json_parse_u64_token(dcc_json_parser_t *parser, uint64_t *out);
dcc_status_t dcc_json_parse_nullable_u64_token(
    dcc_json_parser_t *parser,
    uint64_t *out,
    int *has_value
);
dcc_status_t dcc_json_parse_u32_direct(dcc_json_parser_t *parser, uint32_t *out);
dcc_status_t dcc_json_parse_nullable_u32_token(
    dcc_json_parser_t *parser,
    uint32_t *out,
    int *has_value
);
dcc_status_t dcc_json_parse_double_direct(dcc_json_parser_t *parser, double *out);
dcc_status_t dcc_json_parse_u16_token(dcc_json_parser_t *parser, uint16_t *out);

#endif
