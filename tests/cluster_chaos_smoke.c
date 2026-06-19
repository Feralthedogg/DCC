#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "support/cluster_chaos_smoke_server.h"

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct cluster_chaos_state {
    dcc_cluster_t *cluster;
    cluster_chaos_server_t *server;
    atomic_uint ready_count;
    atomic_uint resumed_count;
    atomic_uint close_4000;
    atomic_uint close_4009;
    atomic_uint monitor_done;
    atomic_uint bad;
} cluster_chaos_state_t;
static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    cluster_chaos_state_t *state = (cluster_chaos_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL || ready->shard_count != CLUSTER_CHAOS_SHARDS ||
        ready->shard_id >= CLUSTER_CHAOS_SHARDS) {
        atomic_store(&state->bad, 1U);
        return;
    }

    unsigned count = atomic_fetch_add(&state->ready_count, 1U) + 1U;
    if (count > CLUSTER_CHAOS_SHARDS * 2U) {
        atomic_store(&state->bad, 1U);
    }
}

static void on_resumed(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    cluster_chaos_state_t *state = (cluster_chaos_state_t *)user_data;
    if (dcc_event_type(event) != DCC_EVENT_RESUMED) {
        atomic_store(&state->bad, 1U);
        return;
    }
    atomic_fetch_add(&state->resumed_count, 1U);
}

static void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    cluster_chaos_state_t *state = (cluster_chaos_state_t *)user_data;
    const dcc_socket_close_event_t *close_event = dcc_event_socket_close(event);
    if (close_event == NULL) {
        atomic_store(&state->bad, 1U);
        return;
    }
    if (close_event->code == 4000) {
        atomic_fetch_add(&state->close_4000, 1U);
    } else if (close_event->code == 4009) {
        atomic_fetch_add(&state->close_4009, 1U);
    }
}

static void *cluster_chaos_monitor_main(void *arg) {
    cluster_chaos_state_t *state = (cluster_chaos_state_t *)arg;
    for (unsigned i = 0; i < 4500U; ++i) {
        if (atomic_load(&state->monitor_done) != 0U) {
            return NULL;
        }
        if (atomic_load(&state->bad) != 0U ||
            (state->server != NULL && atomic_load(&state->server->bad) != 0U)) {
            atomic_store(&state->bad, 1U);
            (void)dcc_cluster_stop(state->cluster);
            return NULL;
        }
        if (atomic_load(&state->ready_count) == CLUSTER_CHAOS_SHARDS * 2U &&
            atomic_load(&state->resumed_count) == CLUSTER_CHAOS_SHARDS &&
            atomic_load(&state->close_4000) == CLUSTER_CHAOS_SHARDS &&
            atomic_load(&state->close_4009) == CLUSTER_CHAOS_SHARDS &&
            state->server != NULL &&
            atomic_load(&state->server->close_4000) == CLUSTER_CHAOS_SHARDS &&
            atomic_load(&state->server->close_4009) == CLUSTER_CHAOS_SHARDS &&
            atomic_load(&state->server->final_ready) == CLUSTER_CHAOS_SHARDS) {
            (void)dcc_cluster_stop(state->cluster);
            return NULL;
        }
        (void)usleep(10000U);
    }
    atomic_store(&state->bad, 1U);
    (void)dcc_cluster_stop(state->cluster);
    return NULL;
}

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    cluster_chaos_server_t gateway;
    pthread_t gateway_thread;
    if (dcc_test_cluster_chaos_start_gateway_server(&gateway, &gateway_thread) != 0) {
        fprintf(stderr, "failed to start cluster chaos gateway server\n");
        return 1;
    }

    char gateway_url[160];
    snprintf(gateway_url, sizeof(gateway_url), "ws://127.0.0.1:%u/?v=10&encoding=json", (unsigned)gateway.port);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
            .gateway_url = gateway_url,
        },
        .shard_count = CLUSTER_CHAOS_SHARDS,
        .total_shard_count = CLUSTER_CHAOS_SHARDS,
        .gateway_max_concurrency = 2,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_cluster_create failed: %s\n", dcc_status_string(status));
        close(gateway.fd);
        (void)pthread_join(gateway_thread, NULL);
        return 1;
    }

    cluster_chaos_state_t state;
    memset(&state, 0, sizeof(state));
    state.cluster = cluster;
    state.server = &gateway;
    atomic_init(&state.ready_count, 0U);
    atomic_init(&state.resumed_count, 0U);
    atomic_init(&state.close_4000, 0U);
    atomic_init(&state.close_4009, 0U);
    atomic_init(&state.monitor_done, 0U);
    atomic_init(&state.bad, 0U);

    pthread_t monitor_thread;
    int monitor_started = pthread_create(&monitor_thread, NULL, cluster_chaos_monitor_main, &state) == 0;

    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_READY, on_ready, &state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_RESUMED, on_resumed, &state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_SOCKET_CLOSE, on_socket_close, &state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }

    atomic_store(&state.monitor_done, 1U);
    if (monitor_started) {
        (void)pthread_join(monitor_thread, NULL);
    }
    (void)pthread_join(gateway_thread, NULL);
    close(gateway.fd);

    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = dcc_cluster_health_summary(cluster, &summary);
    int health_ok = 1;
    for (unsigned i = 0; i < CLUSTER_CHAOS_SHARDS; ++i) {
        dcc_cluster_shard_info_t shard_info = {
            .size = sizeof(shard_info),
        };
        dcc_status_t shard_status = dcc_cluster_shard_info(cluster, i, &shard_info);
        if (shard_status != DCC_OK ||
            shard_info.shard_id != i ||
            shard_info.shard_count != CLUSTER_CHAOS_SHARDS ||
            shard_info.status != DCC_CLUSTER_SHARD_STOPPED ||
            shard_info.starts != 1 ||
            shard_info.restart_attempts != 0 ||
            shard_info.restart_budget_used != 0 ||
            shard_info.ready_events != 2 ||
            shard_info.resumed_events != 1 ||
            shard_info.socket_closes != 2 ||
            shard_info.last_close_code != 4009 ||
            shard_info.log_errors != 0) {
            fprintf(
                stderr,
                "cluster chaos shard %u mismatch: status=%s id=%u/%u state=%s starts=%llu "
                "attempts=%llu budget=%llu ready=%llu resumed=%llu closes=%llu last_close=%u errors=%llu\n",
                i,
                dcc_status_string(shard_status),
                shard_info.shard_id,
                shard_info.shard_count,
                dcc_cluster_shard_status_string(shard_info.status),
                (unsigned long long)shard_info.starts,
                (unsigned long long)shard_info.restart_attempts,
                (unsigned long long)shard_info.restart_budget_used,
                (unsigned long long)shard_info.ready_events,
                (unsigned long long)shard_info.resumed_events,
                (unsigned long long)shard_info.socket_closes,
                (unsigned)shard_info.last_close_code,
                (unsigned long long)shard_info.log_errors
            );
            health_ok = 0;
        }
    }

    int ok = status == DCC_OK &&
        summary_status == DCC_OK &&
        health_ok &&
        atomic_load(&state.bad) == 0U &&
        atomic_load(&state.ready_count) == CLUSTER_CHAOS_SHARDS * 2U &&
        atomic_load(&state.resumed_count) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&state.close_4000) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&state.close_4009) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&gateway.bad) == 0U &&
        atomic_load(&gateway.accepts) == CLUSTER_CHAOS_CONNECTIONS &&
        atomic_load(&gateway.identifies) == CLUSTER_CHAOS_SHARDS * 2U &&
        atomic_load(&gateway.resumes) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&gateway.close_4000) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&gateway.close_4009) == CLUSTER_CHAOS_SHARDS &&
        atomic_load(&gateway.final_ready) == CLUSTER_CHAOS_SHARDS &&
        summary.total_shards == CLUSTER_CHAOS_SHARDS &&
        summary.stopped_shards == CLUSTER_CHAOS_SHARDS &&
        summary.error_shards == 0 &&
        summary.starts == CLUSTER_CHAOS_SHARDS &&
        summary.restart_attempts == 0 &&
        summary.restart_budget_used == 0 &&
        summary.ready_events == CLUSTER_CHAOS_SHARDS * 2U &&
        summary.resumed_events == CLUSTER_CHAOS_SHARDS &&
        summary.socket_closes == CLUSTER_CHAOS_SHARDS * 2U &&
        summary.log_errors == 0;

    if (!ok) {
        fprintf(
            stderr,
            "cluster chaos smoke failed: status=%s summary=%s health=%d state_bad=%u ready=%u "
            "resumed=%u close4000=%u close4009=%u gw_bad=%u accepts=%u identifies=%u resumes=%u "
            "gw4000=%u gw4009=%u final=%u stopped=%u errors=%u starts=%llu attempts=%llu "
            "ready_events=%llu resumed_events=%llu socket_closes=%llu log_errors=%llu\n",
            dcc_status_string(status),
            dcc_status_string(summary_status),
            health_ok,
            atomic_load(&state.bad),
            atomic_load(&state.ready_count),
            atomic_load(&state.resumed_count),
            atomic_load(&state.close_4000),
            atomic_load(&state.close_4009),
            atomic_load(&gateway.bad),
            atomic_load(&gateway.accepts),
            atomic_load(&gateway.identifies),
            atomic_load(&gateway.resumes),
            atomic_load(&gateway.close_4000),
            atomic_load(&gateway.close_4009),
            atomic_load(&gateway.final_ready),
            summary.stopped_shards,
            summary.error_shards,
            (unsigned long long)summary.starts,
            (unsigned long long)summary.restart_attempts,
            (unsigned long long)summary.ready_events,
            (unsigned long long)summary.resumed_events,
            (unsigned long long)summary.socket_closes,
            (unsigned long long)summary.log_errors
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
