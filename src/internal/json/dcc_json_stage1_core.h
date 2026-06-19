#ifndef DCC_JSON_STAGE1_CORE_H
#define DCC_JSON_STAGE1_CORE_H

#include <dcc/error.h>

#include <stddef.h>

typedef struct dcc_json_stage1 {
    size_t *marks;
    size_t len;
    size_t cap;
} dcc_json_stage1_t;

dcc_status_t dcc_json_stage1_scan(dcc_json_stage1_t *stage1, const char *input, size_t len);
void dcc_json_stage1_deinit(dcc_json_stage1_t *stage1);
int dcc_json_stage1_push(dcc_json_stage1_t *stage1, size_t mark);
int dcc_json_stage1_is_structural(unsigned char c);
unsigned dcc_json_stage1_interesting_mask_16(const unsigned char *data);
unsigned dcc_json_stage1_ctz32(unsigned value);

#endif
