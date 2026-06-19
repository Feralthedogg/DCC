#include "interaction_server_smoke_support.h"

#if LLAM_PLATFORM_POSIX

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int interaction_validate_ready_health(dcc_interaction_server_t *server) {
    dcc_interaction_server_health_snapshot_t health = {
        .size = sizeof(health),
    };
    dcc_interaction_server_health_snapshot_t invalid_health = {
        .size = 0,
    };
    char health_json[1024];
    size_t health_json_len = 0;
    if (dcc_interaction_server_health_snapshot(NULL, &health) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot(server, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot(server, &invalid_health) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_wait_until_ready(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_wait_until_ready(server, 1U, &invalid_health) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_wait_until_ready(server, 200U, &health) != DCC_OK ||
        dcc_interaction_server_health_snapshot(server, &health) != DCC_OK ||
        health.health != DCC_INTERACTION_SERVER_HEALTH_READY ||
        health.ok != 1U ||
        health.ready != 1U ||
        health.accepting != 1U ||
        health.drain_complete != 0U ||
        health.state.accepted_connections != 17U ||
        health.stats.completed_requests != 17U ||
        strcmp(dcc_interaction_server_health_string(health.health), "ready") != 0 ||
        strcmp(dcc_interaction_server_health_string((dcc_interaction_server_health_t)999), "unknown") != 0 ||
        dcc_interaction_server_health_snapshot_json(NULL, health_json, sizeof(health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot_json(&invalid_health, health_json, sizeof(health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot_json(&health, NULL, sizeof(health_json), NULL) !=
            DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_health_snapshot_json(&health, health_json, sizeof(health_json), &health_json_len) !=
            DCC_OK ||
        health_json_len == 0U ||
        strstr(health_json, "\"health\":\"ready\"") == NULL ||
        strstr(health_json, "\"ready\":true") == NULL ||
        strstr(health_json, "\"reason\":\"server is ready\"") == NULL) {
        fprintf(stderr, "interaction health ready snapshot failed: %s\n", health_json);
        return 0;
    }
    return 1;
}

static int interaction_run_slow_drain_smoke(
    dcc_interaction_server_t *server,
    uint16_t port,
    interaction_seen_t *seen
) {
    dcc_interaction_server_health_snapshot_t invalid_health = {
        .size = 0,
    };
    dcc_interaction_server_state_t server_state = {
        .size = sizeof(server_state),
    };
    dcc_interaction_server_health_snapshot_t health = {
        .size = sizeof(health),
    };
    char health_json[1024];
    interaction_slow_client_state_t slow_client;
    memset(&slow_client, 0, sizeof(slow_client));
    slow_client.port = port;
    pthread_t slow_thread;
    if (dcc_interaction_server_drain(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_wait_until_drained(NULL, 1U, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_wait_until_drained(server, 1U, &invalid_health) != DCC_ERR_INVALID_ARG ||
        pthread_create(&slow_thread, NULL, interaction_slow_client_thread, &slow_client) != 0) {
        fprintf(stderr, "failed to start interaction slow client\n");
        return 0;
    }
    for (unsigned i = 0; i < 2000U &&
         atomic_load_explicit(&seen->slow_entered, memory_order_acquire) == 0U; ++i) {
        usleep(1000U);
    }
    if (atomic_load_explicit(&seen->slow_entered, memory_order_acquire) == 0U) {
        fprintf(stderr, "interaction slow route did not become active\n");
        atomic_store_explicit(&seen->slow_release, 1U, memory_order_release);
        (void)pthread_join(slow_thread, NULL);
        return 0;
    }

    dcc_status_t st = dcc_interaction_server_drain(server, 5U, &server_state);
    if (st != DCC_ERR_TIMEOUT ||
        server_state.stopping != 1U ||
        server_state.listening != 0U ||
        server_state.draining != 1U ||
        server_state.active_requests == 0U) {
        fprintf(
            stderr,
            "interaction drain timeout mismatch: status=%s stopping=%u listening=%u draining=%u active=%llu\n",
            dcc_status_string(st),
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned long long)server_state.active_requests
        );
        atomic_store_explicit(&seen->slow_release, 1U, memory_order_release);
        (void)pthread_join(slow_thread, NULL);
        return 0;
    }
    if (dcc_interaction_server_health_snapshot(server, &health) != DCC_OK ||
        health.health != DCC_INTERACTION_SERVER_HEALTH_DRAINING ||
        health.ok != 0U ||
        health.ready != 0U ||
        health.accepting != 0U ||
        health.drain_complete != 0U ||
        health.state.draining != 1U ||
        health.stats.active_requests == 0U ||
        dcc_interaction_server_health_snapshot_json(&health, health_json, sizeof(health_json), NULL) !=
            DCC_OK ||
        strstr(health_json, "\"health\":\"draining\"") == NULL ||
        strstr(health_json, "\"draining\":true") == NULL ||
        dcc_interaction_server_wait_until_ready(server, 1U, &health) != DCC_ERR_STATE ||
        dcc_interaction_server_wait_until_drained(server, 5U, &health) != DCC_ERR_TIMEOUT) {
        fprintf(stderr, "interaction health draining snapshot failed: %s\n", health_json);
        atomic_store_explicit(&seen->slow_release, 1U, memory_order_release);
        (void)pthread_join(slow_thread, NULL);
        return 0;
    }
    atomic_store_explicit(&seen->slow_release, 1U, memory_order_release);
    if (pthread_join(slow_thread, NULL) != 0 ||
        !slow_client.ok ||
        slow_client.response.status != 200 ||
        strcmp(slow_client.response.body, "slow") != 0 ||
        seen->slow_seen != 1) {
        fprintf(
            stderr,
            "interaction slow client mismatch: ok=%d status=%d body=%s slow_seen=%d\n",
            slow_client.ok,
            slow_client.response.status,
            slow_client.response.body,
            seen->slow_seen
        );
        return 0;
    }
    st = dcc_interaction_server_drain(server, 2000U, &server_state);
    if (st != DCC_OK ||
        server_state.stopping != 1U ||
        server_state.listening != 0U ||
        server_state.draining != 0U ||
        server_state.active_requests != 0U ||
        server_state.completed_requests != 18U) {
        fprintf(
            stderr,
            "interaction drain completion mismatch: status=%s stopping=%u listening=%u draining=%u "
            "active=%llu completed=%llu\n",
            dcc_status_string(st),
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned long long)server_state.active_requests,
            (unsigned long long)server_state.completed_requests
        );
        return 0;
    }
    memset(&health, 0, sizeof(health));
    health.size = sizeof(health);
    if (dcc_interaction_server_health_snapshot(server, &health) != DCC_OK ||
        health.health != DCC_INTERACTION_SERVER_HEALTH_STOPPED ||
        health.ok != 0U ||
        health.ready != 0U ||
        health.accepting != 0U ||
        health.drain_complete != 1U ||
        health.state.active_requests != 0U ||
        dcc_interaction_server_health_snapshot_json(&health, health_json, sizeof(health_json), NULL) !=
            DCC_OK ||
        strstr(health_json, "\"health\":\"stopped\"") == NULL ||
        strstr(health_json, "\"drain_complete\":true") == NULL ||
        dcc_interaction_server_wait_until_drained(server, 1U, &health) != DCC_OK) {
        fprintf(stderr, "interaction health stopped snapshot failed: %s\n", health_json);
        return 0;
    }
    return 1;
}

static int interaction_validate_final_stats(dcc_interaction_server_t *server) {
    dcc_interaction_server_stats_t stats = {
        .size = sizeof(stats),
    };
    if (dcc_interaction_server_stats(NULL, &stats) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_stats(server, NULL) != DCC_ERR_INVALID_ARG ||
        dcc_interaction_server_stats(server, &stats) != DCC_OK ||
        stats.accepted_connections != 18U ||
        stats.active_requests != 0U ||
        stats.completed_requests != 18U ||
        stats.read_errors != 3U ||
        stats.write_errors != 0U ||
        stats.spawn_errors != 0U ||
        stats.response_2xx != 10U ||
        stats.response_3xx != 0U ||
        stats.response_4xx != 8U ||
        stats.response_5xx != 0U ||
        stats.bad_request_responses != 2U ||
        stats.unauthorized_responses != 2U ||
        stats.not_found_responses != 1U ||
        stats.method_not_allowed_responses != 1U ||
        stats.payload_too_large_responses != 2U) {
        fprintf(
            stderr,
            "interaction stats mismatch: accepted=%llu active=%llu completed=%llu read=%llu write=%llu "
            "spawn=%llu 2xx=%llu 3xx=%llu 4xx=%llu 5xx=%llu bad=%llu unauth=%llu not_found=%llu "
            "method=%llu too_large=%llu\n",
            (unsigned long long)stats.accepted_connections,
            (unsigned long long)stats.active_requests,
            (unsigned long long)stats.completed_requests,
            (unsigned long long)stats.read_errors,
            (unsigned long long)stats.write_errors,
            (unsigned long long)stats.spawn_errors,
            (unsigned long long)stats.response_2xx,
            (unsigned long long)stats.response_3xx,
            (unsigned long long)stats.response_4xx,
            (unsigned long long)stats.response_5xx,
            (unsigned long long)stats.bad_request_responses,
            (unsigned long long)stats.unauthorized_responses,
            (unsigned long long)stats.not_found_responses,
            (unsigned long long)stats.method_not_allowed_responses,
            (unsigned long long)stats.payload_too_large_responses
        );
        return 0;
    }

    dcc_interaction_server_state_t server_state = {
        .size = sizeof(server_state),
    };
    if (dcc_interaction_server_get_state(server, &server_state) != DCC_OK ||
        server_state.stopping != 1U ||
        server_state.listening != 0U ||
        server_state.draining != 0U ||
        server_state.accepted_connections != 18U ||
        server_state.active_requests != 0U ||
        server_state.completed_requests != 18U) {
        fprintf(
            stderr,
            "interaction drained state mismatch: started=%u stopping=%u listening=%u draining=%u "
            "accepted=%llu active=%llu completed=%llu\n",
            (unsigned)server_state.started,
            (unsigned)server_state.stopping,
            (unsigned)server_state.listening,
            (unsigned)server_state.draining,
            (unsigned long long)server_state.accepted_connections,
            (unsigned long long)server_state.active_requests,
            (unsigned long long)server_state.completed_requests
        );
        return 0;
    }
    return 1;
}

int interaction_run_health_and_drain_smoke(
    dcc_interaction_server_t *server,
    uint16_t port,
    interaction_seen_t *seen
) {
    return interaction_validate_ready_health(server) &&
           interaction_run_slow_drain_smoke(server, port, seen) &&
           interaction_validate_final_stats(server);
}

#endif
