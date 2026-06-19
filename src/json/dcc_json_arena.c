#include "internal/json/dcc_json_internal.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define DCC_JSON_ARENA_ALIGNMENT DCC_MAX_ALIGNMENT
#define DCC_JSON_ARENA_MASK (DCC_JSON_ARENA_ALIGNMENT - 1U)
#define DCC_JSON_ARENA_MAX_ALIGNED ((size_t)-1 - DCC_JSON_ARENA_MASK)

_Static_assert(
    DCC_JSON_ARENA_ALIGNMENT != 0U,
    "DCC maximum alignment must be non-zero");

_Static_assert(
    (DCC_JSON_ARENA_ALIGNMENT & DCC_JSON_ARENA_MASK) == 0U,
    "DCC maximum alignment must be a power of two");

static inline size_t dcc_json_align_up_unchecked(size_t value) {
    return (value + DCC_JSON_ARENA_MASK) & ~DCC_JSON_ARENA_MASK;
}

static inline int dcc_json_align_up(size_t value, size_t *out) {
    if (value > DCC_JSON_ARENA_MAX_ALIGNED) {
        return 0;
    }
    *out = dcc_json_align_up_unchecked(value);
    return 1;
}

void dcc_json_arena_deinit(dcc_json_arena_t *arena) {
    if (arena == NULL) {
        return;
    }
    dcc_json_arena_chunk_t *chunk = arena->head;
    while (chunk != NULL) {
        dcc_json_arena_chunk_t *next = chunk->next;
        free(chunk);
        chunk = next;
    }
    arena->head = NULL;
}

void *dcc_json_arena_alloc(dcc_json_arena_t *arena, size_t size) {
    if (arena == NULL || size == 0) {
        return NULL;
    }

    size_t aligned = 0;
    if (!dcc_json_align_up(size, &aligned)) {
        return NULL;
    }

    if (aligned > (size_t)-1 - sizeof(dcc_json_arena_chunk_t)) {
        return NULL;
    }
    dcc_json_arena_chunk_t *chunk = arena->head;
    if (chunk == NULL || aligned > chunk->cap - chunk->len) {
        size_t cap = DCC_JSON_ARENA_CHUNK_SIZE;
        if (aligned > cap) {
            cap = aligned;
        }
        chunk = (dcc_json_arena_chunk_t *)malloc(sizeof(*chunk) + cap);
        if (chunk == NULL) {
            return NULL;
        }
        chunk->next = arena->head;
        chunk->len = 0;
        chunk->cap = cap;
        arena->head = chunk;
    }

    void *ptr = chunk->data + chunk->len;
    chunk->len += aligned;
    return ptr;
}

void *dcc_json_arena_calloc(dcc_json_arena_t *arena, size_t size) {
    void *ptr = dcc_json_arena_alloc(arena, size);
    if (ptr != NULL) {
        memset(ptr, 0, size);
    }
    return ptr;
}

char *dcc_json_arena_copy(dcc_json_arena_t *arena, const char *data, size_t len) {
    if (len == (size_t)-1) {
        return NULL;
    }
    char *copy = (char *)dcc_json_arena_alloc(arena, len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, data, len);
    copy[len] = '\0';
    return copy;
}
