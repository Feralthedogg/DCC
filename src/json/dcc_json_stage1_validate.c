#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_stage1_validate(const char *input, size_t len) {
    if (input == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_json_stage1_t stage1 = {0};
    dcc_status_t status = dcc_json_stage1_scan(&stage1, input, len);
    dcc_json_stage1_deinit(&stage1);
    return status;
}
