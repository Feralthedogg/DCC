#ifndef DCC_JSON_ARENA_CORE_H
#define DCC_JSON_ARENA_CORE_H

#include <stddef.h>

#define DCC_JSON_ARENA_CHUNK_SIZE 16384U

typedef struct dcc_json_arena_chunk {
    struct dcc_json_arena_chunk *next;
    size_t len;
    size_t cap;
    /* Ensures data[] starts at max_align_t alignment for all arena allocations. */
    max_align_t align;
    unsigned char data[];
} dcc_json_arena_chunk_t;

typedef struct dcc_json_arena {
    dcc_json_arena_chunk_t *head;
} dcc_json_arena_t;

#endif
