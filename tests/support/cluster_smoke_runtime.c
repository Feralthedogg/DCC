#include "cluster_smoke_support.h"

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

int cluster_smoke_check_runtime_state(dcc_cluster_t *cluster, const cluster_smoke_state_t *state) {
    if (atomic_load(&state->bad_event) != 0 ||
        atomic_load(&state->ready_count) != 3U ||
        atomic_load(&state->drain_seen) != 1U ||
        atomic_load(&state->admission_reconnect_status) != DCC_ERR_STATE ||
        atomic_load(&state->admission_restore_status) != DCC_OK ||
        atomic_load(&state->drain_status) != DCC_OK ||
        atomic_load(&state->planned_run_status) != DCC_ERR_STATE ||
        atomic_load(&state->planned_completed) != 1U ||
        atomic_load(&state->planned_failed_shards) != 2U ||
        atomic_load(&state->planned_last_status) != DCC_ERR_STATE ||
        atomic_load(&state->seen[0]) == 0 ||
        atomic_load(&state->seen[1]) == 0 ||
        atomic_load(&state->seen[2]) == 0) {
        fprintf(
            stderr,
            "cluster ready events invalid: bad=%u ready=%u drain_seen=%u admission_reconnect=%u "
            "admission_restore=%u drain=%u planned_run=%u planned_completed=%u planned_failed=%u "
            "planned_last=%u seen=%u/%u/%u\n",
            atomic_load(&state->bad_event),
            atomic_load(&state->ready_count),
            atomic_load(&state->drain_seen),
            atomic_load(&state->admission_reconnect_status),
            atomic_load(&state->admission_restore_status),
            atomic_load(&state->drain_status),
            atomic_load(&state->planned_run_status),
            atomic_load(&state->planned_completed),
            atomic_load(&state->planned_failed_shards),
            atomic_load(&state->planned_last_status),
            atomic_load(&state->seen[0]),
            atomic_load(&state->seen[1]),
            atomic_load(&state->seen[2])
        );
        return 1;
    }

    for (uint32_t i = 0; i < 3; ++i) {
        dcc_cluster_shard_info_t shard_info = {
            .size = sizeof(shard_info),
        };
        dcc_cluster_shard_status_t expected_status =
            i == 0 ? DCC_CLUSTER_SHARD_STOPPED : DCC_CLUSTER_SHARD_ERROR;
        dcc_status_t status = dcc_cluster_shard_info(cluster, i, &shard_info);
        if (status != DCC_OK ||
            shard_info.shard_id != i ||
            shard_info.shard_count != 3 ||
            shard_info.status != expected_status ||
            dcc_cluster_shard_recovery_action(&shard_info) !=
                (i == 0 ? DCC_CLUSTER_RECOVERY_NONE : DCC_CLUSTER_RECOVERY_INVESTIGATE) ||
            shard_info.starts != 1 ||
            shard_info.ready_events != 1 ||
            (i == 0 ? shard_info.restart_attempts != 0 : shard_info.restart_attempts != 1) ||
            shard_info.restart_budget_used != 0 ||
            shard_info.resumed_events != 0 ||
            shard_info.log_errors != 0) {
            fprintf(
                stderr,
                "cluster final shard info failed for shard %u: status=%u(%s) expected=%u(%s) "
                "shard=%u/%u starts=%llu ready=%llu restart_attempts=%llu budget=%llu "
                "resumed=%llu log_errors=%llu\n",
                (unsigned)i,
                (unsigned)shard_info.status,
                dcc_cluster_shard_status_string(shard_info.status),
                (unsigned)expected_status,
                dcc_cluster_shard_status_string(expected_status),
                shard_info.shard_id,
                shard_info.shard_count,
                (unsigned long long)shard_info.starts,
                (unsigned long long)shard_info.ready_events,
                (unsigned long long)shard_info.restart_attempts,
                (unsigned long long)shard_info.restart_budget_used,
                (unsigned long long)shard_info.resumed_events,
                (unsigned long long)shard_info.log_errors
            );
            return 1;
        }

        dcc_runtime_stats_t stats = {
            .size = sizeof(stats),
        };
        status = dcc_client_runtime_stats(dcc_cluster_shard(cluster, i), &stats);
        if (status != DCC_OK || stats.tasks_spawned == 0 || stats.events_dispatched == 0) {
            fprintf(stderr, "cluster runtime stats invalid for shard %u\n", (unsigned)i);
            return 1;
        }
    }

    return 0;
}

int cluster_smoke_check_post_run_health(dcc_cluster_t *cluster) {
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t status = dcc_cluster_health_summary(cluster, &summary);
    if (status != DCC_OK ||
        summary.total_shards != 3 ||
        summary.created_shards != 0 ||
        summary.starting_shards != 0 ||
        summary.ready_shards != 0 ||
        summary.stopping_shards != 0 ||
        summary.stopped_shards != 1 ||
        summary.error_shards != 2 ||
        summary.starts != 3 ||
        summary.restart_attempts != 2 ||
        summary.restart_budget_used != 0 ||
        summary.ready_events != 3 ||
        summary.resumed_events != 0 ||
        summary.log_errors != 0 ||
        dcc_cluster_health_summary_state(&summary) != DCC_CLUSTER_HEALTH_CRITICAL) {
        fprintf(stderr, "cluster health summary failed\n");
        return 1;
    }

    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    status = dcc_cluster_recovery_plan(cluster, &plan);
    if (status != DCC_OK ||
        plan.health != DCC_CLUSTER_HEALTH_CRITICAL ||
        plan.action != DCC_CLUSTER_RECOVERY_INVESTIGATE ||
        plan.shard_index != 1 ||
        plan.shard_id != 1 ||
        plan.shard_status != DCC_CLUSTER_SHARD_ERROR ||
        plan.summary.error_shards != 2 ||
        plan.summary.stopped_shards != 1 ||
        plan.shard.status != DCC_CLUSTER_SHARD_ERROR ||
        plan.reason[0] == '\0') {
        fprintf(stderr, "cluster final recovery plan failed\n");
        return 1;
    }

    return 0;
}

int cluster_smoke_check_counter_resets(dcc_cluster_t *cluster) {
    dcc_status_t status = dcc_cluster_reset_shard_counters(cluster, 1);
    dcc_cluster_shard_info_t reset_info = {
        .size = sizeof(reset_info),
    };
    dcc_status_t reset_info_status = dcc_cluster_shard_info(cluster, 1, &reset_info);
    dcc_cluster_health_summary_t summary = {
        .size = sizeof(summary),
    };
    dcc_status_t summary_status = dcc_cluster_health_summary(cluster, &summary);
    if (status != DCC_OK ||
        reset_info_status != DCC_OK ||
        summary_status != DCC_OK ||
        reset_info.status != DCC_CLUSTER_SHARD_ERROR ||
        reset_info.starts != 0 ||
        reset_info.ready_events != 0 ||
        reset_info.log_errors != 0 ||
        summary.stopped_shards != 1 ||
        summary.error_shards != 2 ||
        summary.starts != 2 ||
        summary.restart_attempts != 1 ||
        summary.restart_budget_used != 0 ||
        summary.ready_events != 2) {
        fprintf(stderr, "cluster shard counter reset failed\n");
        return 1;
    }

    status = dcc_cluster_reset_all_counters(cluster);
    summary = (dcc_cluster_health_summary_t){
        .size = sizeof(summary),
    };
    summary_status = dcc_cluster_health_summary(cluster, &summary);
    if (status != DCC_OK ||
        summary_status != DCC_OK ||
        summary.stopped_shards != 1 ||
        summary.error_shards != 2 ||
        summary.starts != 0 ||
        summary.restart_attempts != 0 ||
        summary.restart_budget_used != 0 ||
        summary.ready_events != 0 ||
        summary.resumed_events != 0 ||
        summary.socket_closes != 0 ||
        summary.log_errors != 0) {
        fprintf(stderr, "cluster all counter reset failed\n");
        return 1;
    }

    return 0;
}
