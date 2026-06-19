#include "internal/hot_reload/dcc_hot_reload_internal.h"

uint8_t dcc_hot_reload_worker_dispatch_promote_last_good_after_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *failed_active,
    dcc_hot_reload_worker_process_t *last_good,
    dcc_hot_reload_worker_process_t **retire_failed
) {
    if (hot_reload == NULL || failed_active == NULL || last_good == NULL || failed_active == last_good) {
        return 0U;
    }

    uint8_t promoted = 0U;
    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->active_worker == failed_active &&
        hot_reload->last_good_worker == last_good &&
        dcc_hot_reload_worker_process_is_alive(last_good)) {
        hot_reload->worker_active_failures++;
        hot_reload->worker_last_good_promotions++;
        hot_reload->active_worker = last_good;
        hot_reload->last_good_worker = NULL;
        dcc_hot_reload_set_error(
            hot_reload,
            DCC_OK,
            "hot reload active worker died; restored last-good worker"
        );
        dcc_hot_reload_broadcast(hot_reload);
        if (retire_failed != NULL) {
            *retire_failed = failed_active;
        }
        promoted = 1U;
    }
    dcc_hot_reload_unlock(hot_reload);

    if (promoted) {
        dcc_hot_reload_log(hot_reload, DCC_LOG_WARN, "hot reload restored last-good worker");
    }
    return promoted;
}

uint8_t dcc_hot_reload_worker_dispatch_retire_active_after_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_worker_process_t *failed_active,
    dcc_hot_reload_worker_process_t **retire_failed
) {
    if (hot_reload == NULL || failed_active == NULL) {
        return 0U;
    }

    uint8_t retired = 0U;
    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->active_worker == failed_active) {
        hot_reload->active_worker = NULL;
        hot_reload->last_good_worker = NULL;
        dcc_hot_reload_broadcast(hot_reload);
        if (retire_failed != NULL) {
            *retire_failed = failed_active;
        }
        retired = 1U;
    }
    dcc_hot_reload_unlock(hot_reload);
    return retired;
}

void dcc_hot_reload_worker_dispatch_record_unrecovered_failure(
    dcc_hot_reload_t *hot_reload,
    dcc_status_t status,
    uint8_t active_worker_failed,
    uint8_t temp_error_sent
) {
    dcc_hot_reload_lock(hot_reload);
    if (active_worker_failed) {
        hot_reload->worker_active_failures++;
    }
    hot_reload->worker_dispatch_failures++;
    if (temp_error_sent) {
        hot_reload->worker_temp_error_responses++;
    }
    dcc_hot_reload_set_error(
        hot_reload,
        status,
        temp_error_sent
            ? "hot reload worker event dispatch failed; sent temporary interaction error"
            : "hot reload worker event dispatch failed"
    );
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
}
