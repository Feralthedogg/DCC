#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdint.h>

uint64_t dcc_hot_reload_worker_canary_event_hash(const dcc_event_t *event) {
    uint64_t hash = UINT64_C(1469598103934665603);
    const unsigned char *data = event != NULL ? (const unsigned char *)event->raw.json : NULL;
    size_t len = event != NULL ? event->raw.json_len : 0U;
    for (size_t i = 0; data != NULL && i < len; ++i) {
        hash ^= (uint64_t)data[i];
        hash *= UINT64_C(1099511628211);
    }
    if (event != NULL) {
        hash ^= (uint64_t)event->raw.sequence;
        hash *= UINT64_C(1099511628211);
        hash ^= (uint64_t)event->type;
        hash *= UINT64_C(1099511628211);
    }
    return hash;
}

uint8_t dcc_hot_reload_worker_canary_candidate_active(const dcc_hot_reload_t *hot_reload) {
    return hot_reload != NULL &&
        hot_reload->candidate_worker != NULL &&
        hot_reload->worker_canary.active != 0U ? 1U : 0U;
}

void dcc_hot_reload_worker_canary_record(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *candidate,
    uint8_t success
) {
    if (hot_reload == NULL || candidate == NULL) {
        return;
    }
    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->candidate_worker == candidate && hot_reload->worker_canary.active != 0U) {
        (void)dcc_hot_reload_canary_record(&hot_reload->worker_canary, success);
    }
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}

dcc_hot_reload_worker_process_t *dcc_hot_reload_worker_canary_promote_locked(
    dcc_hot_reload_t *hot_reload
) {
    if (hot_reload == NULL ||
        hot_reload->candidate_worker == NULL ||
        hot_reload->worker_canary.active == 0U) {
        return NULL;
    }

    dcc_hot_reload_worker_process_t *retire_last_good = hot_reload->last_good_worker;
    hot_reload->last_good_worker = hot_reload->active_worker;
    hot_reload->active_worker = hot_reload->candidate_worker;
    hot_reload->candidate_worker = NULL;
    hot_reload->worker_canary_promotions++;
    dcc_hot_reload_canary_stop(&hot_reload->worker_canary);
    hot_reload->last_status = DCC_OK;
    hot_reload->last_error[0] = '\0';
    dcc_hot_reload_broadcast(hot_reload);
    return retire_last_good;
}

dcc_hot_reload_worker_process_t *dcc_hot_reload_worker_canary_rollback_locked(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_canary_rollback_reason_t reason
) {
    if (hot_reload == NULL || hot_reload->candidate_worker == NULL) {
        return NULL;
    }

    dcc_hot_reload_worker_process_t *retire_candidate = hot_reload->candidate_worker;
    hot_reload->candidate_worker = NULL;
    hot_reload->worker_canary.rollback_reason = reason;
    hot_reload->worker_canary_rollbacks++;
    dcc_hot_reload_canary_stop(&hot_reload->worker_canary);
    dcc_hot_reload_set_error(hot_reload, DCC_OK, "hot reload canary rolled back; kept active worker");
    dcc_hot_reload_broadcast(hot_reload);
    return retire_candidate;
}
