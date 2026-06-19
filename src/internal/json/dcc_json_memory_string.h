#ifndef DCC_JSON_MEMORY_STRING_H
#define DCC_JSON_MEMORY_STRING_H

#include "internal/json/dcc_json_core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void dcc_json_arena_deinit(dcc_json_arena_t *arena);
void *dcc_json_arena_alloc(dcc_json_arena_t *arena, size_t size);
void *dcc_json_arena_calloc(dcc_json_arena_t *arena, size_t size);
char *dcc_json_arena_copy(dcc_json_arena_t *arena, const char *data, size_t len);

int dcc_json_parse_hex4_at(const char *p, uint32_t *out);
int dcc_json_utf8_scan_one(const char **cursor, const char *end);
int dcc_json_utf8_append(char **out, const char *limit, uint32_t codepoint);
int dcc_json_find_string_end(
    dcc_json_parser_t *parser,
    const char **raw_start,
    const char **raw_end,
    bool *escaped
);
int dcc_json_decode_string_span(const char *start, const char *end, char *out, size_t out_cap);
int dcc_json_parse_string_arena(dcc_json_parser_t *parser, char **out);
int dcc_json_parse_string_buffer(dcc_json_parser_t *parser, char *out, size_t out_cap);

#endif
