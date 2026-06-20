#include "support/cluster_gateway_smoke_support.h"

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct cluster_gateway_state {
    dcc_cluster_t *cluster;
    atomic_uint ready_count;
    atomic_uint rolling_status;
    atomic_uint rolling_seen;
    atomic_uint operation_completed;
    atomic_uint operation_completed_shards;
    atomic_uint operation_failed_shards;
    atomic_uint operation_total_batches;
    atomic_uint operation_last_status;
    atomic_uint wait_ready_status;
    atomic_uint wait_ready_shards;
    atomic_uint wait_health_status;
    atomic_uint wait_health_state;
    atomic_uint wait_health_action;
    atomic_uint wait_health_shards;
    atomic_uint bad;
    atomic_uint initial_seen[CLUSTER_GATEWAY_SHARDS];
    atomic_uint rolling_seen_shard[CLUSTER_GATEWAY_SHARDS];
} cluster_gateway_state_t;

typedef struct cluster_gateway_wait_state {
    dcc_cluster_t *cluster;
    dcc_status_t status;
} cluster_gateway_wait_state_t;

static void *cluster_gateway_wait_main(void *arg) {
    cluster_gateway_wait_state_t *state = (cluster_gateway_wait_state_t *)arg;
    state->status = dcc_cluster_wait(state->cluster);
    return NULL;
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    cluster_gateway_state_t *state = (cluster_gateway_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL || ready->shard_count != CLUSTER_GATEWAY_SHARDS ||
        ready->shard_id >= CLUSTER_GATEWAY_SHARDS) {
        atomic_store(&state->bad, 1U);
        (void)dcc_cluster_stop(state->cluster);
        return;
    }

    unsigned expected = 0;
    if (!atomic_compare_exchange_strong(&state->initial_seen[ready->shard_id], &expected, 1U)) {
        expected = 0;
        if (!atomic_compare_exchange_strong(&state->rolling_seen_shard[ready->shard_id], &expected, 1U)) {
            atomic_store(&state->bad, 1U);
            (void)dcc_cluster_stop(state->cluster);
            return;
        }
    }

    unsigned count = atomic_fetch_add(&state->ready_count, 1U) + 1U;
    (void)count;
}

static int wait_for_ready_count(cluster_gateway_state_t *state, unsigned expected, unsigned timeout_ms) {
    unsigned waited_ms = 0;
    while (waited_ms <= timeout_ms) {
        if (atomic_load(&state->bad) != 0U) {
            return 0;
        }
        if (atomic_load(&state->ready_count) >= expected) {
            return 1;
        }
        (void)usleep(10000U);
        waited_ms += 10U;
    }
    return 0;
}

static dcc_status_t run_initial_wait_assertions(cluster_gateway_state_t *state) {
    dcc_cluster_health_summary_t ready_summary = {
        .size = sizeof(ready_summary),
    };
    dcc_status_t wait_status =
        dcc_cluster_wait_until_ready(state->cluster, 1U, &ready_summary);
    atomic_store(&state->wait_ready_status, (unsigned)wait_status);
    atomic_store(&state->wait_ready_shards, ready_summary.ready_shards);

    dcc_cluster_health_wait_options_t health_options;
    dcc_cluster_health_wait_options_init(&health_options);
    health_options.timeout_ms = 1U;
    health_options.interval_ms = 0U;
    dcc_cluster_recovery_plan_t health_plan = {
        .size = sizeof(health_plan),
    };
    dcc_status_t health_status =
        dcc_cluster_wait_until_health(state->cluster, &health_options, &health_plan);
    atomic_store(&state->wait_health_status, (unsigned)health_status);
    atomic_store(&state->wait_health_state, (unsigned)health_plan.health);
    atomic_store(&state->wait_health_action, (unsigned)health_plan.action);
    atomic_store(&state->wait_health_shards, health_plan.summary.ready_shards);

    return wait_status == DCC_OK && health_status == DCC_OK ? DCC_OK : DCC_ERR_RUNTIME;
}

static dcc_status_t run_rolling_reconnect(cluster_gateway_state_t *state) {
    dcc_cluster_rolling_reconnect_options_t rolling = {
        .size = sizeof(rolling),
        .first_index = 0,
        .shard_count = CLUSTER_GATEWAY_SHARDS,
        .batch_size = 1,
        .delay_ms = 1,
        .resume = 0,
    };
    dcc_status_t rolling_status = DCC_ERR_STATE;
    for (unsigned i = 0; i < 100U && rolling_status == DCC_ERR_STATE; ++i) {
        rolling_status = dcc_cluster_rolling_reconnect(state->cluster, &rolling);
        if (rolling_status == DCC_ERR_STATE) {
            (void)usleep(1000U);
        }
    }
    atomic_store(&state->rolling_status, (unsigned)rolling_status);
    dcc_cluster_operation_status_t operation = {
        .size = sizeof(operation),
    };
    dcc_status_t operation_status = dcc_cluster_operation_status(state->cluster, &operation);
    if (rolling_status == DCC_OK && operation_status == DCC_OK) {
        atomic_store(&state->operation_completed, operation.completed);
        atomic_store(&state->operation_completed_shards, operation.completed_shards);
        atomic_store(&state->operation_failed_shards, operation.failed_shards);
        atomic_store(&state->operation_total_batches, operation.total_batches);
        atomic_store(&state->operation_last_status, (unsigned)operation.last_status);
    } else {
        atomic_store(&state->bad, 1U);
    }
    atomic_store(&state->rolling_seen, 1U);
    return rolling_status == DCC_OK && operation_status == DCC_OK ? DCC_OK : DCC_ERR_RUNTIME;
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    cluster_gateway_server_t gateway;
    pthread_t gateway_thread;
    if (dcc_test_cluster_gateway_start_gateway_server(&gateway, &gateway_thread) != 0) {
        fprintf(stderr, "failed to start gateway server\n");
        return 1;
    }

    cluster_api_server_t api;
    pthread_t api_thread;
    if (dcc_test_cluster_gateway_start_api_server(&api, &api_thread, gateway.port) != 0) {
        fprintf(stderr, "failed to start api server\n");
        close(gateway.fd);
        return 1;
    }

    char api_base[128];
    snprintf(api_base, sizeof(api_base), "http://127.0.0.1:%u", (unsigned)api.port);
    (void)setenv("DCC_DISCORD_API_BASE", api_base, 1);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
        },
        .shard_count = CLUSTER_GATEWAY_SHARDS,
        .total_shard_count = CLUSTER_GATEWAY_SHARDS,
        .gateway_max_concurrency = 1,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_cluster_create failed: %s\n", dcc_status_string(status));
        close(api.fd);
        close(gateway.fd);
        return 1;
    }

    cluster_gateway_state_t state;
    memset(&state, 0, sizeof(state));
    state.cluster = cluster;
    atomic_init(&state.ready_count, 0U);
    atomic_init(&state.rolling_status, (unsigned)DCC_ERR_STATE);
    atomic_init(&state.rolling_seen, 0U);
    atomic_init(&state.operation_completed, 0U);
    atomic_init(&state.operation_completed_shards, 0U);
    atomic_init(&state.operation_failed_shards, 0U);
    atomic_init(&state.operation_total_batches, 0U);
    atomic_init(&state.operation_last_status, (unsigned)DCC_ERR_STATE);
    atomic_init(&state.wait_ready_status, (unsigned)DCC_ERR_STATE);
    atomic_init(&state.wait_ready_shards, 0U);
    atomic_init(&state.wait_health_status, (unsigned)DCC_ERR_STATE);
    atomic_init(&state.wait_health_state, (unsigned)DCC_CLUSTER_HEALTH_UNKNOWN);
    atomic_init(&state.wait_health_action, (unsigned)DCC_CLUSTER_RECOVERY_UNKNOWN);
    atomic_init(&state.wait_health_shards, 0U);
    atomic_init(&state.bad, 0U);
    for (unsigned i = 0; i < CLUSTER_GATEWAY_SHARDS; ++i) {
        atomic_init(&state.initial_seen[i], 0U);
        atomic_init(&state.rolling_seen_shard[i], 0U);
    }

    status = dcc_cluster_on(cluster, DCC_EVENT_READY, on_ready, &state, NULL, 0);
    int cluster_started = 0;
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
        cluster_started = status == DCC_OK;
    }
    cluster_gateway_wait_state_t wait_state = {
        .cluster = cluster,
        .status = DCC_ERR_RUNTIME,
    };
    pthread_t cluster_wait_thread;
    int cluster_wait_thread_started = 0;
    if (status == DCC_OK) {
        if (pthread_create(&cluster_wait_thread, NULL, cluster_gateway_wait_main, &wait_state) == 0) {
            cluster_wait_thread_started = 1;
        } else {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        if (!wait_for_ready_count(&state, CLUSTER_GATEWAY_SHARDS, 5000U)) {
            status = DCC_ERR_TIMEOUT;
        }
    }
    if (status == DCC_OK) {
        status = run_initial_wait_assertions(&state);
    }
    if (status == DCC_OK) {
        status = run_rolling_reconnect(&state);
    }
    if (status == DCC_OK) {
        if (!wait_for_ready_count(&state, CLUSTER_GATEWAY_CONNECTIONS, 10000U)) {
            status = DCC_ERR_TIMEOUT;
        }
    }
    if (cluster_started) {
        dcc_status_t stop_status = dcc_cluster_stop(cluster);
        dcc_status_t wait_status = DCC_OK;
        if (cluster_wait_thread_started) {
            (void)pthread_join(cluster_wait_thread, NULL);
            wait_status = wait_state.status;
        }
        if (status == DCC_OK) {
            status = stop_status == DCC_OK ? wait_status : stop_status;
        }
    }

    (void)pthread_join(api_thread, NULL);
    (void)pthread_join(gateway_thread, NULL);
    close(api.fd);
    close(gateway.fd);
    (void)unsetenv("DCC_DISCORD_API_BASE");

    dcc_gateway_info_t gateway_info = {
        .size = sizeof(gateway_info),
    };
    dcc_status_t info_status = dcc_cluster_gateway_info(cluster, &gateway_info);
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = dcc_cluster_health_summary(cluster, &summary);
    int health_ok = 1;
    for (unsigned i = 0; i < CLUSTER_GATEWAY_SHARDS; ++i) {
        dcc_cluster_shard_info_t shard_info = {
            .size = sizeof(shard_info),
        };
        dcc_status_t shard_info_status = dcc_cluster_shard_info(cluster, i, &shard_info);
        dcc_runtime_stats_t stats = {
            .size = sizeof(stats),
        };
        dcc_status_t stats_status = dcc_client_runtime_stats(dcc_cluster_shard(cluster, i), &stats);
        dcc_gateway_info_t shard_gateway_info = {
            .size = sizeof(shard_gateway_info),
        };
        dcc_status_t shard_gateway_status = dcc_client_gateway_info(
            dcc_cluster_shard_const(cluster, i),
            &shard_gateway_info
        );
        if (shard_info_status != DCC_OK ||
            stats_status != DCC_OK ||
            shard_gateway_status != DCC_OK ||
            shard_info.shard_id != i ||
            shard_info.shard_count != CLUSTER_GATEWAY_SHARDS ||
            shard_info.status != DCC_CLUSTER_SHARD_STOPPED ||
            shard_info.starts != 1 ||
            shard_info.restart_attempts != 1 ||
            shard_info.restart_budget_used != 0 ||
            shard_info.ready_events != 2 ||
            shard_info.resumed_events != 0 ||
            shard_info.log_errors != 0 ||
            strcmp(dcc_cluster_shard_status_string(shard_info.status), "stopped") != 0 ||
            stats.tasks_spawned == 0 ||
            stats.events_dispatched == 0 ||
            shard_gateway_info.recommended_shards != CLUSTER_GATEWAY_SHARDS ||
            shard_gateway_info.max_concurrency != 2 ||
            shard_gateway_info.total_identifies != 1000 ||
            shard_gateway_info.remaining_identifies != 997 ||
            shard_gateway_info.reset_after_ms != 60000) {
            fprintf(
                stderr,
                "shard %u health mismatch: shard_info=%s stats=%s gateway=%s id=%u/%u status=%s "
                "starts=%llu restart_attempts=%llu budget=%llu ready=%llu resumed=%llu log_errors=%llu "
                "tasks=%llu events=%llu recommended=%u concurrency=%u remaining=%u total=%u reset=%llu\n",
                i,
                dcc_status_string(shard_info_status),
                dcc_status_string(stats_status),
                dcc_status_string(shard_gateway_status),
                shard_info.shard_id,
                shard_info.shard_count,
                dcc_cluster_shard_status_string(shard_info.status),
                (unsigned long long)shard_info.starts,
                (unsigned long long)shard_info.restart_attempts,
                (unsigned long long)shard_info.restart_budget_used,
                (unsigned long long)shard_info.ready_events,
                (unsigned long long)shard_info.resumed_events,
                (unsigned long long)shard_info.log_errors,
                (unsigned long long)stats.tasks_spawned,
                (unsigned long long)stats.events_dispatched,
                shard_gateway_info.recommended_shards,
                shard_gateway_info.max_concurrency,
                shard_gateway_info.remaining_identifies,
                shard_gateway_info.total_identifies,
                (unsigned long long)shard_gateway_info.reset_after_ms
            );
            health_ok = 0;
        }
    }
    int ok = status == DCC_OK &&
        info_status == DCC_OK &&
        summary_status == DCC_OK &&
        health_ok &&
        summary.total_shards == CLUSTER_GATEWAY_SHARDS &&
        summary.created_shards == 0 &&
        summary.starting_shards == 0 &&
        summary.ready_shards == 0 &&
        summary.stopping_shards == 0 &&
        summary.stopped_shards == CLUSTER_GATEWAY_SHARDS &&
        summary.error_shards == 0 &&
        summary.starts == CLUSTER_GATEWAY_SHARDS &&
        summary.restart_attempts == CLUSTER_GATEWAY_SHARDS &&
        summary.restarts == 0 &&
        summary.restart_suppressed == 0 &&
        summary.restart_budget_used == 0 &&
        summary.ready_events == CLUSTER_GATEWAY_CONNECTIONS &&
        summary.resumed_events == 0 &&
        summary.log_errors == 0 &&
        gateway_info.recommended_shards == CLUSTER_GATEWAY_SHARDS &&
        gateway_info.max_concurrency == 2 &&
        gateway_info.remaining_identifies == 997 &&
        gateway_info.total_identifies == 1000 &&
        gateway_info.reset_after_ms == 60000 &&
        atomic_load(&api.requests) == 1U &&
        atomic_load(&gateway.bad) == 0U &&
        atomic_load(&gateway.identify_count) == CLUSTER_GATEWAY_CONNECTIONS &&
        atomic_load(&gateway.rolling_identify_count) == CLUSTER_GATEWAY_SHARDS &&
        atomic_load(&gateway.initial_seen[0]) == 1U &&
        atomic_load(&gateway.initial_seen[1]) == 1U &&
        atomic_load(&gateway.rolling_seen[0]) == 1U &&
        atomic_load(&gateway.rolling_seen[1]) == 1U &&
        atomic_load(&state.bad) == 0U &&
        atomic_load(&state.ready_count) == CLUSTER_GATEWAY_CONNECTIONS &&
        atomic_load(&state.rolling_status) == DCC_OK &&
        atomic_load(&state.rolling_seen) == 1U &&
        atomic_load(&state.operation_completed) == 1U &&
        atomic_load(&state.operation_completed_shards) == CLUSTER_GATEWAY_SHARDS &&
        atomic_load(&state.operation_failed_shards) == 0U &&
        atomic_load(&state.operation_total_batches) == CLUSTER_GATEWAY_SHARDS &&
        atomic_load(&state.operation_last_status) == DCC_OK &&
        atomic_load(&state.wait_ready_status) == DCC_OK &&
        atomic_load(&state.wait_ready_shards) == CLUSTER_GATEWAY_SHARDS &&
        atomic_load(&state.wait_health_status) == DCC_OK &&
        atomic_load(&state.wait_health_state) == DCC_CLUSTER_HEALTH_HEALTHY &&
        atomic_load(&state.wait_health_action) == DCC_CLUSTER_RECOVERY_NONE &&
        atomic_load(&state.wait_health_shards) == CLUSTER_GATEWAY_SHARDS &&
        atomic_load(&state.initial_seen[0]) == 1U &&
        atomic_load(&state.initial_seen[1]) == 1U &&
        atomic_load(&state.rolling_seen_shard[0]) == 1U &&
        atomic_load(&state.rolling_seen_shard[1]) == 1U;

    if (!ok) {
        fprintf(
            stderr,
            "cluster gateway smoke failed: status=%s info=%s health=%d api=%u gw_bad=%u identifies=%u "
            "rolling_identifies=%u gw_initial=%u/%u gw_rolling=%u/%u ready=%u state_bad=%u "
            "rolling=%s rolling_seen=%u op_done=%u op_completed=%u op_failed=%u op_batches=%u op_last=%s "
            "wait_ready=%s wait_shards=%u wait_health=%s/%u/%u/%u "
            "state_initial=%u/%u state_rolling=%u/%u summary=%s stopped=%u starts=%llu restarts=%llu "
            "restart_attempts=%llu ready_events=%llu shards=%u concurrency=%u remaining=%u total=%u reset=%llu\n",
            dcc_status_string(status),
            dcc_status_string(info_status),
            health_ok,
            atomic_load(&api.requests),
            atomic_load(&gateway.bad),
            atomic_load(&gateway.identify_count),
            atomic_load(&gateway.rolling_identify_count),
            atomic_load(&gateway.initial_seen[0]),
            atomic_load(&gateway.initial_seen[1]),
            atomic_load(&gateway.rolling_seen[0]),
            atomic_load(&gateway.rolling_seen[1]),
            atomic_load(&state.ready_count),
            atomic_load(&state.bad),
            dcc_status_string((dcc_status_t)atomic_load(&state.rolling_status)),
            atomic_load(&state.rolling_seen),
            atomic_load(&state.operation_completed),
            atomic_load(&state.operation_completed_shards),
            atomic_load(&state.operation_failed_shards),
            atomic_load(&state.operation_total_batches),
            dcc_status_string((dcc_status_t)atomic_load(&state.operation_last_status)),
            dcc_status_string((dcc_status_t)atomic_load(&state.wait_ready_status)),
            atomic_load(&state.wait_ready_shards),
            dcc_status_string((dcc_status_t)atomic_load(&state.wait_health_status)),
            atomic_load(&state.wait_health_state),
            atomic_load(&state.wait_health_action),
            atomic_load(&state.wait_health_shards),
            atomic_load(&state.initial_seen[0]),
            atomic_load(&state.initial_seen[1]),
            atomic_load(&state.rolling_seen_shard[0]),
            atomic_load(&state.rolling_seen_shard[1]),
            dcc_status_string(summary_status),
            summary.stopped_shards,
            (unsigned long long)summary.starts,
            (unsigned long long)summary.restarts,
            (unsigned long long)summary.restart_attempts,
            (unsigned long long)summary.ready_events,
            gateway_info.recommended_shards,
            gateway_info.max_concurrency,
            gateway_info.remaining_identifies,
            gateway_info.total_identifies,
            (unsigned long long)gateway_info.reset_after_ms
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
