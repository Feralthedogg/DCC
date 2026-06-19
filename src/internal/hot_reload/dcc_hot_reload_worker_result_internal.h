#ifndef DCC_HOT_RELOAD_WORKER_RESULT_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_RESULT_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_process_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_worker_result_set_read(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_result_set_t *out
);
void dcc_hot_reload_worker_result_set_deinit(dcc_hot_reload_worker_result_set_t *result);

#ifdef __cplusplus
}
#endif

#endif
