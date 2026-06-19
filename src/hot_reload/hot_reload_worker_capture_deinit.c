#include "internal/hot_reload/dcc_hot_reload_internal.h"

void dcc_hot_reload_worker_capture_deinit(dcc_hot_reload_worker_capture_t *capture) {
    if (capture == NULL) {
        return;
    }
    dcc_hot_reload_worker_result_set_t set = {
        .items = capture->items,
        .count = capture->count,
    };
    dcc_hot_reload_worker_result_set_deinit(&set);
    capture->items = NULL;
    capture->count = 0;
    capture->cap = 0;
}
