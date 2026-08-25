#ifndef DCC_REST_TYPES_STRING_MAP_H
#define DCC_REST_TYPES_STRING_MAP_H

#include <stddef.h>
#include <stdint.h>

#define DCC_REST_STRING_MAP_VERSION 1U

typedef struct dcc_rest_string_map_entry {
  const char *key;
  const char *value;
} dcc_rest_string_map_entry_t;

typedef struct dcc_rest_string_map {
  size_t size;
  uint32_t version;
  uint64_t present;
  const dcc_rest_string_map_entry_t *entries;
  size_t entry_count;
} dcc_rest_string_map_t;

#define DCC_REST_STRING_MAP_INIT                                               \
  {sizeof(dcc_rest_string_map_t), DCC_REST_STRING_MAP_VERSION, UINT64_C(0),    \
   NULL, 0U}

static inline void dcc_rest_string_map_init(dcc_rest_string_map_t *value) {
  if (value != NULL) {
    const dcc_rest_string_map_t initialized = DCC_REST_STRING_MAP_INIT;
    *value = initialized;
  }
}

#endif
