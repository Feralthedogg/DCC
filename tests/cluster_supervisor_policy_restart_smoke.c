#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "internal/client/dcc_cluster_internal.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct restart_policy_state {
    atomic_uint calls;
    atomic_uint callback_calls;
    atomic_uint bad_event;
    atomic_uint last_callback_decision;
    atomic_uint last_callback_scheduled;
    atomic_uint last_callback_suppressed;
} restart_policy_state_t;

static dcc_status_t restart_then_suppress_policy(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_event_t *event,
    dcc_cluster_supervisor_action_t *action,
    void *user_data
) {
    restart_policy_state_t *state = (restart_policy_state_t *)user_data;
    unsigned call = atomic_fetch_add(&state->calls, 1U) + 1U;
    if (cluster == NULL ||
        event == NULL ||
        event->size < sizeof(*event) ||
        event->shard_index != 0 ||
        event->shard_id != 0 ||
        event->restart_policy.max_restarts != 0 ||
        action == NULL ||
        action->size < sizeof(*action)) {
        atomic_store(&state->bad_event, 1U);
        return DCC_ERR_INVALID_ARG;
    }

    if (call == 1U) {
        if (event->gateway_status != DCC_ERR_NETWORK) {
            atomic_store(&state->bad_event, 1U);
            return DCC_ERR_INVALID_ARG;
        }
        dcc_cluster_supervisor_action_init(action);
        action->decision = DCC_CLUSTER_SUPERVISOR_RESTART;
        action->delay_ms = 0;
        return DCC_OK;
    }

    if (event->gateway_status != DCC_ERR_INVALID_ARG) {
        atomic_store(&state->bad_event, 1U);
        return DCC_ERR_INVALID_ARG;
    }
    dcc_cluster_supervisor_action_init(action);
    action->decision = DCC_CLUSTER_SUPERVISOR_SUPPRESS;
    action->delay_ms = 0;
    return DCC_OK;
}

static void supervisor_decision_callback(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_decision_event_t *event,
    void *user_data
) {
    restart_policy_state_t *state = (restart_policy_state_t *)user_data;
    unsigned call = atomic_fetch_add(&state->callback_calls, 1U) + 1U;
    if (cluster == NULL ||
        event == NULL ||
        event->size < sizeof(*event) ||
        event->shard_index != 0U ||
        event->shard_id != 0U ||
        event->action_status != DCC_OK ||
        event->status.decisions != call ||
        event->status.last_shard_index != 0U ||
        event->shard.shard_id != 0U ||
        event->restart_policy.max_restarts != 0U) {
        atomic_store(&state->bad_event, 1U);
        return;
    }
    char event_json[4096];
    size_t event_json_len = 0;
    if (dcc_cluster_supervisor_decision_event_json(
            event,
            event_json,
            sizeof(event_json),
            &event_json_len
        ) != DCC_OK ||
        event_json_len == 0U ||
        strstr(event_json, "\"action\":{") == NULL ||
        strstr(event_json, "\"restart_policy\":{\"max_restarts\":0") == NULL ||
        strstr(event_json, "\"status\":{\"decisions\":") == NULL ||
        strstr(event_json, "\"shard\":{\"shard_id\":0") == NULL) {
        atomic_store(&state->bad_event, 1U);
        return;
    }
    if (call == 1U &&
        (event->gateway_status != DCC_ERR_NETWORK ||
         event->action.decision != DCC_CLUSTER_SUPERVISOR_RESTART ||
         event->restart_scheduled != 1U ||
         event->restart_suppressed != 0U ||
         strstr(event_json, "\"decision\":\"restart\"") == NULL ||
         strstr(event_json, "\"restart_scheduled\":true") == NULL)) {
        atomic_store(&state->bad_event, 1U);
    }
    if (call == 2U &&
        (event->gateway_status != DCC_ERR_INVALID_ARG ||
         event->action.decision != DCC_CLUSTER_SUPERVISOR_SUPPRESS ||
         event->restart_scheduled != 0U ||
         event->restart_suppressed != 1U ||
         strstr(event_json, "\"decision\":\"suppress\"") == NULL ||
         strstr(event_json, "\"restart_suppressed\":true") == NULL)) {
        atomic_store(&state->bad_event, 1U);
    }
    atomic_store(&state->last_callback_decision, (unsigned)event->action.decision);
    atomic_store(&state->last_callback_scheduled, event->restart_scheduled);
    atomic_store(&state->last_callback_suppressed, event->restart_suppressed);
}

static void *supervisor_main(void *arg) {
    dcc_cluster_supervisor_task(arg);
    return NULL;
}

int main(void) {
    restart_policy_state_t policy_state;
    atomic_init(&policy_state.calls, 0U);
    atomic_init(&policy_state.callback_calls, 0U);
    atomic_init(&policy_state.bad_event, 0U);
    atomic_init(&policy_state.last_callback_decision, 0U);
    atomic_init(&policy_state.last_callback_scheduled, 0U);
    atomic_init(&policy_state.last_callback_suppressed, 0U);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
        },
        .shard_count = 1,
        .total_shard_count = 1,
        .shard_restart_max = 0,
        .supervisor_policy = restart_then_suppress_policy,
        .supervisor_policy_user_data = &policy_state,
        .supervisor_decision_callback = supervisor_decision_callback,
        .supervisor_decision_user_data = &policy_state,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "cluster create failed: %s\n", dcc_status_string(status));
        return 1;
    }

    dcc_client_t *client = dcc_cluster_shard(cluster, 0);
    atomic_store_explicit(&client->gateway_last_status, DCC_ERR_NETWORK, memory_order_release);
    atomic_store_explicit(&client->gateway_task_completed, true, memory_order_release);
    atomic_store_explicit(&client->gateway_task_running, false, memory_order_release);
    atomic_store_explicit(&client->stopping, false, memory_order_release);

    pthread_t supervisor;
    if (pthread_create(&supervisor, NULL, supervisor_main, cluster) != 0) {
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_shard_info_t info = {
        .size = sizeof(info),
    };
    int observed = 0;
    for (unsigned i = 0; i < 200U; ++i) {
        status = dcc_cluster_shard_info(cluster, 0, &info);
        if (status != DCC_OK) {
            break;
        }
        if (info.restart_attempts == 1U && info.restart_suppressed == 1U) {
            observed = 1;
            break;
        }
        (void)usleep(10000U);
    }

    atomic_store_explicit(&cluster->supervisor_stop, true, memory_order_release);
    (void)pthread_join(supervisor, NULL);

    dcc_cluster_supervisor_status_t supervisor_status = {
        .size = sizeof(supervisor_status),
    };
    dcc_cluster_supervisor_decision_event_t invalid_event = {
        .size = sizeof(invalid_event) - 1U,
    };
    char invalid_json[8] = {0};
    dcc_status_t supervisor_status_result =
        dcc_cluster_supervisor_status(cluster, &supervisor_status);
    unsigned completed =
        atomic_load_explicit(&client->gateway_task_completed, memory_order_acquire) ? 1U : 0U;
    int ok = observed &&
        status == DCC_OK &&
        supervisor_status_result == DCC_OK &&
        supervisor_status.decisions == 2U &&
        supervisor_status.restart_scheduled == 1U &&
        supervisor_status.restart_suppressed == 1U &&
        supervisor_status.policy_errors == 0U &&
        supervisor_status.last_shard_index == 0U &&
        supervisor_status.last_shard_id == 0U &&
        supervisor_status.last_gateway_status == DCC_ERR_INVALID_ARG &&
        supervisor_status.last_action_status == DCC_OK &&
        supervisor_status.last_decision == DCC_CLUSTER_SUPERVISOR_SUPPRESS &&
        supervisor_status.last_delay_ms == 0U &&
        supervisor_status.last_restart_scheduled == 0U &&
        dcc_cluster_supervisor_decision_event_json(NULL, invalid_json, sizeof(invalid_json), NULL) ==
            DCC_ERR_INVALID_ARG &&
        dcc_cluster_supervisor_decision_event_json(&invalid_event, invalid_json, sizeof(invalid_json), NULL) ==
            DCC_ERR_INVALID_ARG &&
        atomic_load(&policy_state.calls) == 2U &&
        atomic_load(&policy_state.callback_calls) == 2U &&
        atomic_load(&policy_state.bad_event) == 0U &&
        atomic_load(&policy_state.last_callback_decision) == DCC_CLUSTER_SUPERVISOR_SUPPRESS &&
        atomic_load(&policy_state.last_callback_scheduled) == 0U &&
        atomic_load(&policy_state.last_callback_suppressed) == 1U &&
        completed == 0U &&
        info.status == DCC_CLUSTER_SHARD_ERROR &&
        info.restart_attempts == 1U &&
        info.restarts == 0U &&
        info.restart_suppressed == 1U &&
        info.restart_budget_used == 1U &&
        info.log_errors == 1U &&
        strcmp(info.last_error, "restart suppressed by supervisor policy") == 0;

    if (!ok) {
        fprintf(
            stderr,
            "supervisor restart policy smoke failed: observed=%d status=%s calls=%u bad=%u completed=%u "
            "callbacks=%u callback_last=%u/%u/%u health=%s attempts=%llu restarts=%llu suppressed=%llu budget=%llu errors=%llu "
            "last=%s supervisor=%s/%llu/%llu/%llu/%llu/%s\n",
            observed,
            dcc_status_string(status),
            atomic_load(&policy_state.calls),
            atomic_load(&policy_state.bad_event),
            completed,
            atomic_load(&policy_state.callback_calls),
            atomic_load(&policy_state.last_callback_decision),
            atomic_load(&policy_state.last_callback_scheduled),
            atomic_load(&policy_state.last_callback_suppressed),
            dcc_cluster_shard_status_string(info.status),
            (unsigned long long)info.restart_attempts,
            (unsigned long long)info.restarts,
            (unsigned long long)info.restart_suppressed,
            (unsigned long long)info.restart_budget_used,
            (unsigned long long)info.log_errors,
            info.last_error,
            dcc_status_string(supervisor_status_result),
            (unsigned long long)supervisor_status.decisions,
            (unsigned long long)supervisor_status.restart_scheduled,
            (unsigned long long)supervisor_status.restart_suppressed,
            (unsigned long long)supervisor_status.policy_errors,
            dcc_cluster_supervisor_decision_string(supervisor_status.last_decision)
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
