#ifndef DCC_JSON_OBJECT_CORE_H
#define DCC_JSON_OBJECT_CORE_H

#include "internal/json/dcc_json_types.h"

#include <stddef.h>

typedef struct dcc_json_object_entry {
    char *key;
    dcc_json_t *value;
} dcc_json_object_entry_t;

typedef struct dcc_json_object_data {
    dcc_json_object_entry_t *entries;
    size_t len;
    size_t cap;
} dcc_json_object_data_t;

#endif
