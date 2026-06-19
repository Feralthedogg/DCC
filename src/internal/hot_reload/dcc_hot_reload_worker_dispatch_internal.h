#ifndef DCC_HOT_RELOAD_WORKER_DISPATCH_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_DISPATCH_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_worker_dispatch_event(
    dcc_hot_reload_t *hot_reload,
    const dcc_event_t *event
);
dcc_status_t dcc_hot_reload_worker_dispatch_try_event(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *worker,
    const dcc_event_t *event,
    uint8_t *worker_failed
);
dcc_status_t dcc_hot_reload_worker_dispatch_send_temp_interaction_error(
    dcc_hot_reload_t *hot_reload,
    const dcc_event_t *event
);
uint8_t dcc_hot_reload_worker_dispatch_promote_last_good_after_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *failed_active,
    dcc_hot_reload_worker_process_t *last_good,
    dcc_hot_reload_worker_process_t **retire_failed
);
uint8_t dcc_hot_reload_worker_dispatch_retire_active_after_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *failed_active,
    dcc_hot_reload_worker_process_t **retire_failed
);
void dcc_hot_reload_worker_dispatch_record_unrecovered_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_status_t status,
    uint8_t active_worker_failed,
    uint8_t temp_error_sent
);

#ifdef __cplusplus
}
#endif

#endif
