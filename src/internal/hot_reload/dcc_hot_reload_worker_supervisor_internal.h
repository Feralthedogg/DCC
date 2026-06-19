#ifndef DCC_HOT_RELOAD_WORKER_SUPERVISOR_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_SUPERVISOR_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_worker_supervisor_start(dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_worker_supervisor_stop(dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_worker_supervisor_sleep_ms(uint32_t ms);
uint8_t dcc_hot_reload_worker_supervisor_probe(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
);
void dcc_hot_reload_worker_supervisor_retire(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms
);
void dcc_hot_reload_worker_supervise_once(dcc_hot_reload_t *hot_reload);

#ifdef __cplusplus
}
#endif

#endif
