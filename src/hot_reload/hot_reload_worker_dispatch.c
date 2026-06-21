#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>

dcc_status_t dcc_hot_reload_worker_dispatch_event(
    dcc_hot_reload_t *hot_reload,
    const dcc_event_t *event
) {
    if (hot_reload == NULL || event == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (event->raw.json == NULL || event->raw.json_len == 0U) {
        return DCC_OK;
    }

    dcc_hot_reload_lock(hot_reload);
    dcc_status_t enter_status = dcc_hot_reload_enter_active_call_locked(hot_reload);
    if (enter_status != DCC_OK) {
        dcc_hot_reload_unlock(hot_reload);
        return enter_status;
    }
    dcc_hot_reload_worker_process_t *active = hot_reload->active_worker;
    dcc_hot_reload_worker_process_t *last_good = hot_reload->last_good_worker;
    dcc_hot_reload_worker_process_t *candidate = hot_reload->candidate_worker;
    uint8_t route_candidate = candidate != NULL &&
        hot_reload->worker_canary.active != 0U &&
        dcc_hot_reload_canary_should_route(
            &hot_reload->worker_canary,
            dcc_hot_reload_worker_canary_event_hash(event)
        ) ? 1U : 0U;
    dcc_hot_reload_unlock(hot_reload);

    dcc_hot_reload_worker_process_t *retire_failed = NULL;
    dcc_hot_reload_worker_process_t *retire_retry = NULL;
    dcc_hot_reload_worker_process_t *retire_canary = NULL;
    dcc_hot_reload_worker_process_t *retire_promoted_last_good = NULL;
    uint8_t active_worker_failed = 0U;
    dcc_hot_reload_worker_process_t *primary = route_candidate ? candidate : active;
    dcc_status_t status = primary != NULL
        ? dcc_hot_reload_worker_dispatch_try_event(hot_reload, primary, event, &active_worker_failed)
        : DCC_ERR_STATE;
    if (route_candidate) {
        dcc_hot_reload_worker_canary_record(hot_reload, candidate, status == DCC_OK ? 1U : 0U);
        dcc_hot_reload_lock(hot_reload);
        if (hot_reload->candidate_worker == candidate && hot_reload->worker_canary.active != 0U) {
            dcc_hot_reload_canary_decision_t decision =
                dcc_hot_reload_canary_decide(&hot_reload->worker_canary, dcc_hot_reload_now_ms());
            if (decision == DCC_HOT_RELOAD_CANARY_PROMOTE) {
                retire_promoted_last_good = dcc_hot_reload_worker_canary_promote_locked(hot_reload);
            } else if (decision == DCC_HOT_RELOAD_CANARY_ROLLBACK) {
                retire_canary = dcc_hot_reload_worker_canary_rollback_locked(
                    hot_reload,
                    hot_reload->worker_canary.rollback_reason
                );
            }
        }
        dcc_hot_reload_unlock(hot_reload);

        if (status != DCC_OK && active != NULL && active != candidate) {
            uint8_t fallback_failed = 0U;
            status = dcc_hot_reload_worker_dispatch_try_event(
                hot_reload,
                active,
                event,
                &fallback_failed
            );
            active_worker_failed = fallback_failed;
        } else {
            active_worker_failed = 0U;
        }
    }
    if (status != DCC_OK && active_worker_failed && last_good != NULL && last_good != active) {
        (void)dcc_hot_reload_worker_dispatch_promote_last_good_after_failure(
            hot_reload,
            active,
            last_good,
            &retire_failed
        );
        uint8_t last_good_worker_failed = 0U;
        status = dcc_hot_reload_worker_dispatch_try_event(
            hot_reload,
            last_good,
            event,
            &last_good_worker_failed
        );
        if (status != DCC_OK && last_good_worker_failed) {
            (void)dcc_hot_reload_worker_dispatch_retire_active_after_failure(
                hot_reload,
                last_good,
                &retire_retry
            );
        }
    }
    if (status != DCC_OK && active_worker_failed && retire_failed == NULL) {
        (void)dcc_hot_reload_worker_dispatch_retire_active_after_failure(
            hot_reload,
            active,
            &retire_failed
        );
    }
    if (retire_failed != NULL) {
        dcc_hot_reload_worker_process_stop(retire_failed, hot_reload->worker_drain_timeout_ms);
        free(retire_failed);
    }
    if (retire_retry != NULL) {
        dcc_hot_reload_worker_process_stop(retire_retry, hot_reload->worker_drain_timeout_ms);
        free(retire_retry);
    }
    if (retire_canary != NULL) {
        dcc_hot_reload_worker_process_stop(retire_canary, hot_reload->worker_drain_timeout_ms);
        free(retire_canary);
    }
    if (retire_promoted_last_good != NULL) {
        dcc_hot_reload_worker_process_stop(retire_promoted_last_good, hot_reload->worker_drain_timeout_ms);
        free(retire_promoted_last_good);
    }
    if (status != DCC_OK) {
        dcc_status_t fallback_status =
            dcc_hot_reload_worker_dispatch_send_temp_interaction_error(hot_reload, event);
        dcc_hot_reload_worker_dispatch_record_unrecovered_failure(
            hot_reload,
            status,
            active_worker_failed,
            fallback_status == DCC_OK ? 1U : 0U
        );
    }
    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_leave_active_call_locked(hot_reload);
    dcc_hot_reload_unlock(hot_reload);
    return status;
}
