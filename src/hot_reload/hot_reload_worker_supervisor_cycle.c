#include "internal/hot_reload/dcc_hot_reload_internal.h"

void dcc_hot_reload_worker_supervise_once(dcc_hot_reload_t *hot_reload) {
    dcc_hot_reload_worker_process_t *retire_active = NULL;
    dcc_hot_reload_worker_process_t *retire_last_good = NULL;
    dcc_hot_reload_worker_process_t *retire_candidate = NULL;
    dcc_hot_reload_worker_process_t *retire_promoted_last_good = NULL;
    uint8_t promoted_last_good = 0U;
    uint8_t promoted_candidate = 0U;
    uint8_t rolled_back_candidate = 0U;
    uint8_t active_failed = 0U;
    dcc_hot_reload_worker_process_t *active = NULL;
    dcc_hot_reload_worker_process_t *last_good = NULL;
    dcc_hot_reload_worker_process_t *candidate = NULL;
    uint8_t active_alive = 0U;
    uint8_t last_good_alive = 0U;
    uint8_t candidate_alive = 0U;

    dcc_hot_reload_lock(hot_reload);
    if (hot_reload->active_calls != 0U) {
        dcc_hot_reload_unlock(hot_reload);
        return;
    }

    active = hot_reload->active_worker;
    last_good = hot_reload->last_good_worker;
    candidate = hot_reload->candidate_worker;
    if (active == NULL && last_good == NULL && candidate == NULL) {
        dcc_hot_reload_unlock(hot_reload);
        return;
    }
    if (dcc_hot_reload_enter_active_call_locked(hot_reload) != DCC_OK) {
        dcc_hot_reload_unlock(hot_reload);
        return;
    }
    dcc_hot_reload_unlock(hot_reload);

    active_alive = dcc_hot_reload_worker_supervisor_probe(active, hot_reload->worker_health_timeout_ms);
    last_good_alive = last_good == active
        ? active_alive
        : dcc_hot_reload_worker_supervisor_probe(last_good, hot_reload->worker_health_timeout_ms);
    candidate_alive = candidate == active || candidate == last_good
        ? (candidate == active ? active_alive : last_good_alive)
        : dcc_hot_reload_worker_supervisor_probe(candidate, hot_reload->worker_health_timeout_ms);

    dcc_hot_reload_lock(hot_reload);
    dcc_hot_reload_leave_active_call_locked(hot_reload);
    if (hot_reload->active_worker != active ||
        hot_reload->last_good_worker != last_good ||
        hot_reload->candidate_worker != candidate) {
        dcc_hot_reload_unlock(hot_reload);
        return;
    }

    if (candidate != NULL) {
        if (!candidate_alive) {
            retire_candidate = dcc_hot_reload_worker_canary_rollback_locked(
                hot_reload,
                DCC_HOT_RELOAD_CANARY_ROLLBACK_HEALTHCHECK_FAILED
            );
            rolled_back_candidate = retire_candidate != NULL ? 1U : 0U;
        } else {
            dcc_hot_reload_canary_decision_t decision =
                dcc_hot_reload_canary_decide(&hot_reload->worker_canary, dcc_hot_reload_now_ms());
            if (decision == DCC_HOT_RELOAD_CANARY_PROMOTE) {
                uint8_t promoted_last_good_alive = active_alive;
                retire_promoted_last_good = dcc_hot_reload_worker_canary_promote_locked(hot_reload);
                promoted_candidate = 1U;
                active = hot_reload->active_worker;
                last_good = hot_reload->last_good_worker;
                active_alive = candidate_alive;
                last_good_alive = promoted_last_good_alive;
            } else if (decision == DCC_HOT_RELOAD_CANARY_ROLLBACK) {
                retire_candidate = dcc_hot_reload_worker_canary_rollback_locked(
                    hot_reload,
                    hot_reload->worker_canary.rollback_reason
                );
                rolled_back_candidate = retire_candidate != NULL ? 1U : 0U;
            }
        }
    }

    if (active != NULL && !active_alive) {
        active_failed = 1U;
        retire_active = active;
        if (last_good != NULL && last_good_alive) {
            hot_reload->active_worker = last_good;
            hot_reload->last_good_worker = NULL;
            promoted_last_good = 1U;
        } else {
            hot_reload->active_worker = NULL;
            hot_reload->last_good_worker = NULL;
            retire_last_good = last_good;
        }
    } else if (last_good != NULL && !last_good_alive) {
        hot_reload->last_good_worker = NULL;
        retire_last_good = last_good;
    }

    if (active_failed) {
        hot_reload->worker_active_failures++;
        if (promoted_last_good) {
            hot_reload->worker_last_good_promotions++;
        }
        dcc_hot_reload_set_error(
            hot_reload,
            promoted_last_good ? DCC_OK : DCC_ERR_RUNTIME,
            promoted_last_good
                ? "hot reload active worker died; restored last-good worker"
                : "hot reload active worker died"
        );
    }
    dcc_hot_reload_broadcast(hot_reload);
    dcc_hot_reload_unlock(hot_reload);

    dcc_hot_reload_worker_supervisor_retire(retire_active, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_supervisor_retire(retire_last_good, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_supervisor_retire(retire_candidate, hot_reload->worker_drain_timeout_ms);
    dcc_hot_reload_worker_supervisor_retire(retire_promoted_last_good, hot_reload->worker_drain_timeout_ms);
    if (promoted_last_good) {
        dcc_hot_reload_log(hot_reload, DCC_LOG_WARN, "hot reload restored last-good worker");
    }
    if (promoted_candidate) {
        dcc_hot_reload_log(hot_reload, DCC_LOG_INFO, "hot reload canary promoted worker");
    }
    if (rolled_back_candidate) {
        dcc_hot_reload_log(hot_reload, DCC_LOG_WARN, "hot reload canary rolled back worker");
    }
}
