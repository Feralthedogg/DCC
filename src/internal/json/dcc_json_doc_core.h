#ifndef DCC_JSON_DOC_CORE_H
#define DCC_JSON_DOC_CORE_H

#include "internal/json/dcc_json_arena_core.h"
#include "internal/json/dcc_json_stage1_core.h"
#include "internal/json/dcc_json_types.h"

typedef struct dcc_json_doc {
    dcc_json_arena_t arena;
    dcc_json_stage1_t stage1;
    dcc_json_t *root;
} dcc_json_doc_t;

#endif
