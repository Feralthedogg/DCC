#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>

void dcc_hot_reload_worker_result_set_deinit(dcc_hot_reload_worker_result_set_t *result) {
    if (result == NULL) {
        return;
    }
    for (size_t i = 0; i < result->count; ++i) {
        free(result->items[i].method);
        free(result->items[i].path);
        free(result->items[i].content_type);
        free(result->items[i].body);
    }
    free(result->items);
    result->items = NULL;
    result->count = 0;
}
