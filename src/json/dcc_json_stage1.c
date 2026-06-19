#include "internal/json/dcc_json_internal.h"

#include <stdbool.h>

dcc_status_t dcc_json_stage1_scan(dcc_json_stage1_t *stage1, const char *input, size_t len) {
    bool in_string = false;
    bool escaped = false;
    size_t i = 0;

    while (i < len) {
        if (!in_string && i + 16U <= len) {
            unsigned mask = dcc_json_stage1_interesting_mask_16((const unsigned char *)input + i);
            if (mask == 0) {
                i += 16U;
                continue;
            }
            unsigned skip = dcc_json_stage1_ctz32(mask);
            if (skip > 0) {
                i += skip;
                continue;
            }
        }

        unsigned char c = (unsigned char)input[i];
        if (in_string) {
            if (escaped) {
                escaped = false;
                i++;
                continue;
            }
            if (c == '\\') {
                escaped = true;
                i++;
                continue;
            }
            if (c == '"') {
                in_string = false;
                if (dcc_json_stage1_push(stage1, i) != 0) {
                    return DCC_ERR_NOMEM;
                }
                i++;
                continue;
            }
            if (c < 0x20U) {
                return DCC_ERR_JSON;
            }
            i++;
            continue;
        }

        if (c == '"') {
            in_string = true;
            if (dcc_json_stage1_push(stage1, i) != 0) {
                return DCC_ERR_NOMEM;
            }
        } else if (dcc_json_stage1_is_structural(c)) {
            if (dcc_json_stage1_push(stage1, i) != 0) {
                return DCC_ERR_NOMEM;
            }
        }
        i++;
    }

    return in_string || escaped ? DCC_ERR_JSON : DCC_OK;
}
