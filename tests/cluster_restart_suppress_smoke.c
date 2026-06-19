#include <dcc/dcc.h>

#if defined(_WIN32)
int main(void) {
    return 0;
}
#else

#include "support/cluster_restart_suppress_smoke_support.h"

#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    (void)signal(SIGPIPE, SIG_IGN);

    suppress_gateway_server_t server;
    memset(&server, 0, sizeof(server));
    atomic_init(&server.accepts, 0U);
    atomic_init(&server.identifies, 0U);
    atomic_init(&server.bad, 0U);
    if (suppress_start_listener(&server.fd, &server.port) != 0) {
        fprintf(stderr, "failed to start suppress gateway server\n");
        return 1;
    }

    pthread_t server_thread;
    if (pthread_create(&server_thread, NULL, suppress_gateway_server_main, &server) != 0) {
        close(server.fd);
        return 1;
    }

    char gateway_url[160];
    snprintf(gateway_url, sizeof(gateway_url), "ws://127.0.0.1:%u/?v=10&encoding=json", (unsigned)server.port);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = "mock-token",
            .intents = DCC_INTENT_GUILDS,
            .gateway_url = gateway_url,
        },
        .shard_count = 1,
        .total_shard_count = 1,
        .shard_restart_max = 1,
        .shard_restart_base_delay_ms = 10,
        .shard_restart_max_delay_ms = 10,
    };
    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "dcc_cluster_create failed: %s\n", dcc_status_string(status));
        close(server.fd);
        (void)pthread_join(server_thread, NULL);
        return 1;
    }

    dcc_cluster_restart_policy_t policy = {
        .size = sizeof(policy),
    };
    dcc_status_t policy_status = dcc_cluster_get_restart_policy(cluster, &policy);

    suppress_monitor_t monitor;
    memset(&monitor, 0, sizeof(monitor));
    monitor.cluster = cluster;
    atomic_init(&monitor.suppressed_seen, 0U);
    atomic_init(&monitor.timeout_seen, 0U);
    atomic_init(&monitor.bad, 0U);

    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_READY, suppress_on_ready, &monitor, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
    }

    pthread_t monitor_thread;
    int monitor_started = 0;
    if (status == DCC_OK) {
        if (pthread_create(&monitor_thread, NULL, suppress_monitor_main, &monitor) == 0) {
            monitor_started = 1;
        } else {
            status = DCC_ERR_RUNTIME;
        }
    }
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }

    if (monitor_started) {
        (void)pthread_join(monitor_thread, NULL);
    }
    (void)pthread_join(server_thread, NULL);
    close(server.fd);

    dcc_cluster_shard_info_t info = {
        .size = sizeof(info),
    };
    dcc_status_t info_status = dcc_cluster_shard_info(cluster, 0, &info);
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = dcc_cluster_health_summary(cluster, &summary);
    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    dcc_status_t plan_status = dcc_cluster_recovery_plan(cluster, &plan);
    int ok = status == DCC_OK &&
        policy_status == DCC_OK &&
        policy.max_restarts == 1 &&
        policy.base_delay_ms == 10 &&
        policy.max_delay_ms == 10 &&
        info_status == DCC_OK &&
        summary_status == DCC_OK &&
        plan_status == DCC_OK &&
        info.status == DCC_CLUSTER_SHARD_ERROR &&
        info.starts == 1 &&
        info.restart_attempts == 1 &&
        info.restarts == 1 &&
        info.restart_suppressed == 1 &&
        info.restart_budget_used == 1 &&
        info.ready_events == 0 &&
        info.resumed_events == 0 &&
        info.log_errors >= 2 &&
        strcmp(info.last_error, "restart limit reached") == 0 &&
        dcc_cluster_shard_recovery_action(&info) == DCC_CLUSTER_RECOVERY_REARM_RESTART &&
        strcmp(dcc_cluster_recovery_action_string(DCC_CLUSTER_RECOVERY_REARM_RESTART), "rearm_restart") == 0 &&
        summary.total_shards == 1 &&
        summary.created_shards == 0 &&
        summary.starting_shards == 0 &&
        summary.ready_shards == 0 &&
        summary.stopping_shards == 0 &&
        summary.stopped_shards == 0 &&
        summary.error_shards == 1 &&
        summary.starts == 1 &&
        summary.restart_attempts == 1 &&
        summary.restarts == 1 &&
        summary.restart_suppressed == 1 &&
        summary.restart_budget_used == 1 &&
        summary.ready_events == 0 &&
        summary.resumed_events == 0 &&
        summary.log_errors >= 2 &&
        dcc_cluster_health_summary_state(&summary) == DCC_CLUSTER_HEALTH_CRITICAL &&
        strcmp(dcc_cluster_health_state_string(DCC_CLUSTER_HEALTH_CRITICAL), "critical") == 0 &&
        plan.health == DCC_CLUSTER_HEALTH_CRITICAL &&
        plan.action == DCC_CLUSTER_RECOVERY_REARM_RESTART &&
        plan.shard_index == 0 &&
        plan.shard.status == DCC_CLUSTER_SHARD_ERROR &&
        plan.summary.error_shards == 1 &&
        plan.reason[0] != '\0' &&
        atomic_load(&monitor.suppressed_seen) == 1U &&
        atomic_load(&monitor.timeout_seen) == 0U &&
        atomic_load(&monitor.bad) == 0U &&
        atomic_load(&server.bad) == 0U &&
        atomic_load(&server.accepts) == 2U &&
        atomic_load(&server.identifies) == 2U;

    if (!ok) {
        fprintf(
            stderr,
            "cluster restart suppress smoke failed: status=%s policy=%s info=%s summary=%s plan=%s "
            "monitor=%u/%u/%u accepts=%u identifies=%u server_bad=%u health_status=%s starts=%llu "
            "attempts=%llu restarts=%llu suppressed=%llu budget=%llu ready=%llu errors=%llu last=%s "
            "summary_error=%u summary_stopped=%u summary_attempts=%llu summary_restarts=%llu "
            "summary_suppressed=%llu summary_budget=%llu summary_errors=%llu plan_action=%s plan_reason=%s\n",
            dcc_status_string(status),
            dcc_status_string(policy_status),
            dcc_status_string(info_status),
            dcc_status_string(summary_status),
            dcc_status_string(plan_status),
            atomic_load(&monitor.suppressed_seen),
            atomic_load(&monitor.timeout_seen),
            atomic_load(&monitor.bad),
            atomic_load(&server.accepts),
            atomic_load(&server.identifies),
            atomic_load(&server.bad),
            dcc_cluster_shard_status_string(info.status),
            (unsigned long long)info.starts,
            (unsigned long long)info.restart_attempts,
            (unsigned long long)info.restarts,
            (unsigned long long)info.restart_suppressed,
            (unsigned long long)info.restart_budget_used,
            (unsigned long long)info.ready_events,
            (unsigned long long)info.log_errors,
            info.last_error,
            summary.error_shards,
            summary.stopped_shards,
            (unsigned long long)summary.restart_attempts,
            (unsigned long long)summary.restarts,
            (unsigned long long)summary.restart_suppressed,
            (unsigned long long)summary.restart_budget_used,
            (unsigned long long)summary.log_errors,
            dcc_cluster_recovery_action_string(plan.action),
            plan.reason
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_remediation_result_t default_remediation = {
        .size = sizeof(default_remediation),
    };
    dcc_status_t default_remediation_status =
        dcc_cluster_remediate(cluster, NULL, &default_remediation);
    dcc_cluster_remediation_options_t remediation_options = {
        .size = sizeof(remediation_options),
        .allow_rearm_restart = 1U,
    };
    dcc_cluster_remediation_loop_options_t remediation_loop_options;
    dcc_cluster_remediation_loop_options_init(&remediation_loop_options);
    remediation_loop_options.remediation = remediation_options;
    remediation_loop_options.max_attempts = 1U;
    remediation_loop_options.interval_ms = 0;
    dcc_cluster_remediation_loop_result_t remediation_loop = {
        .size = sizeof(remediation_loop),
    };
    dcc_status_t remediation_status =
        dcc_cluster_remediate_loop(cluster, &remediation_loop_options, &remediation_loop);
    dcc_cluster_remediation_result_t remediation = remediation_loop.last;
    dcc_status_t rearm_invalid_status = dcc_cluster_rearm_shard_restart(NULL, 0);
    dcc_status_t rearm_index_status = dcc_cluster_rearm_shard_restart(cluster, 1);
    memset(&info, 0, sizeof(info));
    info.size = sizeof(info);
    info_status = dcc_cluster_shard_info(cluster, 0, &info);
    memset(&summary, 0, sizeof(summary));
    summary.size = sizeof(summary);
    summary_status = dcc_cluster_health_summary(cluster, &summary);
    dcc_status_t rearm_all_status = dcc_cluster_rearm_all_shard_restarts(cluster);
    if (rearm_invalid_status != DCC_ERR_INVALID_ARG ||
        rearm_index_status != DCC_ERR_INVALID_ARG ||
        default_remediation_status != DCC_ERR_STATE ||
        default_remediation.plan.action != DCC_CLUSTER_RECOVERY_REARM_RESTART ||
        default_remediation.action_status != DCC_ERR_STATE ||
        default_remediation.action_attempted != 0U ||
        default_remediation.action_allowed != 0U ||
        remediation_status != DCC_OK ||
        remediation_loop.attempts != 1U ||
        remediation_loop.actions_attempted != 1U ||
        remediation_loop.exhausted_attempts != 1U ||
        remediation_loop.stopped_error != 0U ||
        remediation.plan.action != DCC_CLUSTER_RECOVERY_REARM_RESTART ||
        remediation.action_status != DCC_OK ||
        remediation.action_attempted != 1U ||
        remediation.action_allowed != 1U ||
        info_status != DCC_OK ||
        summary_status != DCC_OK ||
        rearm_all_status != DCC_OK ||
        info.status != DCC_CLUSTER_SHARD_CREATED ||
        info.restart_attempts != 1 ||
        info.restarts != 1 ||
        info.restart_suppressed != 1 ||
        info.restart_budget_used != 0 ||
        info.last_error[0] != '\0' ||
        dcc_cluster_shard_recovery_action(&info) != DCC_CLUSTER_RECOVERY_WAIT ||
        summary.created_shards != 1 ||
        summary.error_shards != 0 ||
        summary.restart_attempts != 1 ||
        summary.restarts != 1 ||
        summary.restart_suppressed != 1 ||
        summary.restart_budget_used != 0 ||
        dcc_cluster_rearm_all_shard_restarts(NULL) != DCC_ERR_INVALID_ARG) {
        fprintf(
            stderr,
            "cluster restart rearm failed: invalid=%s index=%s rearm=%s info=%s summary=%s all=%s "
            "default_remediate=%s/%s/%u/%u remediate=%s/%s/%u/%u status=%s "
            "attempts=%llu restarts=%llu suppressed=%llu budget=%llu last=%s created=%u error=%u "
            "summary_budget=%llu\n",
            dcc_status_string(rearm_invalid_status),
            dcc_status_string(rearm_index_status),
            dcc_status_string(remediation_status),
            dcc_status_string(info_status),
            dcc_status_string(summary_status),
            dcc_status_string(rearm_all_status),
            dcc_status_string(default_remediation_status),
            dcc_status_string(default_remediation.action_status),
            default_remediation.action_attempted,
            default_remediation.action_allowed,
            dcc_status_string(remediation_status),
            dcc_status_string(remediation.action_status),
            remediation.action_attempted,
            remediation.action_allowed,
            dcc_cluster_shard_status_string(info.status),
            (unsigned long long)info.restart_attempts,
            (unsigned long long)info.restarts,
            (unsigned long long)info.restart_suppressed,
            (unsigned long long)info.restart_budget_used,
            info.last_error,
            summary.created_shards,
            summary.error_shards,
            (unsigned long long)summary.restart_budget_used
        );
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}

#endif
