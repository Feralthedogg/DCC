#include "internal/json/dcc_json_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_json_stage1_deinit(dcc_json_stage1_t *stage1) {
    if (stage1 == NULL) {
        return;
    }
    free(stage1->marks);
    memset(stage1, 0, sizeof(*stage1));
}

int dcc_json_stage1_push(dcc_json_stage1_t *stage1, size_t mark) {
    if (stage1->len == stage1->cap) {
        size_t next_cap = 0;
        if (dcc_json_next_cap(stage1->cap, 128U, sizeof(*stage1->marks), &next_cap) != 0) {
            return -1;
        }
        size_t *next = (size_t *)realloc(stage1->marks, next_cap * sizeof(*next));
        if (next == NULL) {
            return -1;
        }
        stage1->marks = next;
        stage1->cap = next_cap;
    }
    stage1->marks[stage1->len++] = mark;
    return 0;
}
