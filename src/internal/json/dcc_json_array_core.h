#ifndef DCC_JSON_ARRAY_CORE_H
#define DCC_JSON_ARRAY_CORE_H

#include "internal/json/dcc_json_types.h"

#include <stddef.h>

typedef struct dcc_json_array_data {
    dcc_json_t **items;
    size_t len;
    size_t cap;
} dcc_json_array_data_t;

#endif
