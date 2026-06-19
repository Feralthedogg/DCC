#ifndef DCC_HOT_RELOAD_WORKER_CANARY_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_CANARY_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_hot_reload_worker_canary_event_hash(const dcc_event_t *event);
uint8_t dcc_hot_reload_worker_canary_candidate_active(const dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_worker_canary_record(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *candidate,
    uint8_t success
);
dcc_hot_reload_worker_process_t *dcc_hot_reload_worker_canary_promote_locked(
    dcc_hot_reload_t *hot_reload
);
dcc_hot_reload_worker_process_t *dcc_hot_reload_worker_canary_rollback_locked(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_canary_rollback_reason_t reason
);

#ifdef __cplusplus
}
#endif

#endif
