#ifndef DCC_JSON_VALUE_CORE_H
#define DCC_JSON_VALUE_CORE_H

#include "internal/json/dcc_json_array_core.h"
#include "internal/json/dcc_json_doc_core.h"
#include "internal/json/dcc_json_object_core.h"
#include "internal/json/dcc_json_types.h"

struct dcc_json {
    dcc_json_doc_t *doc;
    dcc_json_type_t type;
    union {
        int boolean;
        char *string;
        char *number;
        dcc_json_array_data_t array;
        dcc_json_object_data_t object;
    } as;
};

#endif
