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

typedef struct supervisor_policy_state {
    atomic_uint calls;
    atomic_uint bad_event;
} supervisor_policy_state_t;

static dcc_status_t suppress_policy(
    const dcc_cluster_t *cluster,
    const dcc_cluster_supervisor_event_t *event,
    dcc_cluster_supervisor_action_t *action,
    void *user_data
) {
    supervisor_policy_state_t *state = (supervisor_policy_state_t *)user_data;
    atomic_fetch_add(&state->calls, 1U);
    if (cluster == NULL ||
        event == NULL ||
        event->size < sizeof(*event) ||
        event->shard_index != 0 ||
        event->shard_id != 0 ||
        event->gateway_status != DCC_ERR_NETWORK ||
        event->shard.status != DCC_CLUSTER_SHARD_CREATED ||
        event->restart_policy.max_restarts != 1 ||
        action == NULL ||
        action->size < sizeof(*action)) {
        atomic_store(&state->bad_event, 1U);
        return DCC_ERR_INVALID_ARG;
    }
    dcc_cluster_supervisor_action_init(action);
    action->decision = DCC_CLUSTER_SUPERVISOR_SUPPRESS;
    action->delay_ms = 0;
    return DCC_OK;
}

static void *supervisor_main(void *arg) {
    dcc_cluster_supervisor_task(arg);
    return NULL;
}

int main(void) {
    supervisor_policy_state_t policy_state;
    atomic_init(&policy_state.calls, 0U);
    atomic_init(&policy_state.bad_event, 0U);

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
        .shard_restart_max = 1,
        .supervisor_policy = suppress_policy,
        .supervisor_policy_user_data = &policy_state,
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
        if (info.restart_suppressed == 1U) {
            observed = 1;
            break;
        }
        (void)usleep(10000U);
    }

    atomic_store_explicit(&cluster->supervisor_stop, true, memory_order_release);
    (void)pthread_join(supervisor, NULL);

    unsigned completed =
        atomic_load_explicit(&client->gateway_task_completed, memory_order_acquire) ? 1U : 0U;
    int ok = observed &&
        status == DCC_OK &&
        atomic_load(&policy_state.calls) == 1U &&
        atomic_load(&policy_state.bad_event) == 0U &&
        completed == 0U &&
        info.status == DCC_CLUSTER_SHARD_ERROR &&
        info.restart_attempts == 0U &&
        info.restarts == 0U &&
        info.restart_suppressed == 1U &&
        info.restart_budget_used == 0U &&
        strcmp(info.last_error, "restart suppressed by supervisor policy") == 0;

    if (!ok) {
        fprintf(
            stderr,
            "supervisor policy smoke failed: observed=%d status=%s calls=%u bad=%u completed=%u "
            "health=%s attempts=%llu restarts=%llu suppressed=%llu budget=%llu last=%s\n",
            observed,
            dcc_status_string(status),
            atomic_load(&policy_state.calls),
            atomic_load(&policy_state.bad_event),
            completed,
            dcc_cluster_shard_status_string(info.status),
            (unsigned long long)info.restart_attempts,
            (unsigned long long)info.restarts,
            (unsigned long long)info.restart_suppressed,
            (unsigned long long)info.restart_budget_used,
            info.last_error
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
