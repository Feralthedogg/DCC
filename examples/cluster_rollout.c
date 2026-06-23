#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct rollout_state {
    dcc_cluster_t *cluster;
    uint32_t shard_count;
    uint32_t rollout_batch_size;
    uint32_t rollout_delay_ms;
    uint8_t rollout_resume;
    int rollout_on_ready;
    int exit_after_ready;
    atomic_uint ready_events;
    atomic_uint rollout_started;
    atomic_uint rollout_status;
    atomic_uint stop_requested;
} rollout_state_t;

static int env_bool(const char *name, int fallback) {
    uint8_t parsed = fallback ? 1U : 0U;
    if (DCC_ENV_BOOL_OR(name, parsed, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr, "ignoring invalid %s=%s, using %d\n", name, value, fallback ? 1 : 0);
        return fallback;
    }
    return parsed ? 1 : 0;
}

static uint32_t env_u32(const char *name, uint32_t fallback, uint32_t min_value, uint32_t max_value) {
    uint32_t parsed = fallback;
    if (DCC_ENV_U32_RANGE_OR(name, fallback, min_value, max_value, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(
            stderr,
            "ignoring invalid %s=%s, using %u\n",
            name,
            value,
            (unsigned)fallback
        );
        return fallback;
    }
    return parsed;
}

static const char *env_token(void) {
    const char *token = NULL;
    return DCC_ENV_TOKEN(&token) == DCC_OK ? token : NULL;
}

static void print_status_json(dcc_cluster_t *cluster, const char *label) {
    if (!env_bool("DCC_CLUSTER_PRINT_JSON", 0)) {
        return;
    }

    char json[12288];
    size_t json_len = 0;
    dcc_status_t status = dcc_cluster_status_json(cluster, json, sizeof(json), &json_len);
    if (status == DCC_OK) {
        printf("%s: %.*s\n", label, (int)json_len, json);
    } else {
        fprintf(stderr, "%s failed: %s\n", label, dcc_status_string(status));
    }
}

static void print_summary(dcc_cluster_t *cluster) {
    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    if (dcc_cluster_recovery_plan(cluster, &plan) != DCC_OK) {
        return;
    }
    const dcc_cluster_health_summary_t *summary = &plan.summary;
    char action_shard[16];
    if (plan.shard_index == DCC_CLUSTER_NO_SHARD_INDEX) {
        snprintf(action_shard, sizeof(action_shard), "-");
    } else {
        snprintf(action_shard, sizeof(action_shard), "%u", (unsigned)plan.shard_index);
    }

    printf(
        "cluster summary: health=%s action=%s action_shard=%s total=%u ready=%u stopped=%u error=%u starts=%llu "
        "restart_attempts=%llu restarts=%llu suppressed=%llu budget_used=%llu "
        "ready_events=%llu log_errors=%llu reason=%s\n",
        dcc_cluster_health_state_string(plan.health),
        dcc_cluster_recovery_action_string(plan.action),
        action_shard,
        summary->total_shards,
        summary->ready_shards,
        summary->stopped_shards,
        summary->error_shards,
        (unsigned long long)summary->starts,
        (unsigned long long)summary->restart_attempts,
        (unsigned long long)summary->restarts,
        (unsigned long long)summary->restart_suppressed,
        (unsigned long long)summary->restart_budget_used,
        (unsigned long long)summary->ready_events,
        (unsigned long long)summary->log_errors,
        plan.reason
    );

    print_status_json(cluster, "cluster status json");
}

static void maybe_stop_after_ready(rollout_state_t *state, unsigned ready_events) {
    if (!state->exit_after_ready) {
        return;
    }
    uint32_t target = state->rollout_on_ready ? state->shard_count * 2U : state->shard_count;
    if (ready_events >= target && atomic_exchange(&state->stop_requested, 1U) == 0U) {
        print_summary(state->cluster);
        (void)dcc_cluster_stop(state->cluster);
    }
}

static void start_rollout_if_ready(rollout_state_t *state, unsigned ready_events) {
    if (!state->rollout_on_ready ||
        ready_events < state->shard_count ||
        atomic_exchange(&state->rollout_started, 1U) != 0U) {
        return;
    }

    dcc_cluster_rolling_reconnect_options_t rolling = {
        .size = sizeof(rolling),
        .first_index = 0,
        .shard_count = state->shard_count,
        .batch_size = state->rollout_batch_size,
        .delay_ms = state->rollout_delay_ms,
        .resume = state->rollout_resume,
    };

    dcc_status_t status = DCC_ERR_STATE;
    for (unsigned i = 0; i < 100U && status == DCC_ERR_STATE; ++i) {
        status = dcc_cluster_rolling_reconnect(state->cluster, &rolling);
    }
    atomic_store(&state->rollout_status, (unsigned)status);

    dcc_cluster_operation_status_t operation = {
        .size = sizeof(operation),
    };
    dcc_status_t snapshot_status = dcc_cluster_operation_status(state->cluster, &operation);
    printf(
        "rolling reconnect: status=%s snapshot=%s completed=%u completed_shards=%u "
        "failed_shards=%u batches=%u last=%s\n",
        dcc_status_string(status),
        dcc_status_string(snapshot_status),
        operation.completed,
        operation.completed_shards,
        operation.failed_shards,
        operation.total_batches,
        dcc_status_string(operation.last_status)
    );

    if (snapshot_status == DCC_OK) {
        print_status_json(state->cluster, "rolling reconnect status json");
    }

    if (status != DCC_OK && atomic_exchange(&state->stop_requested, 1U) == 0U) {
        (void)dcc_cluster_stop(state->cluster);
    }
}

static void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log != NULL) {
        fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message);
    }
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    rollout_state_t *state = (rollout_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL) {
        return;
    }

    unsigned ready_events = atomic_fetch_add(&state->ready_events, 1U) + 1U;
    printf(
        "READY shard=%u/%u event=%u\n",
        ready->shard_id,
        ready->shard_count,
        ready_events
    );

    start_rollout_if_ready(state, ready_events);
    maybe_stop_after_ready(state, ready_events);
}

int main(void) {
    (void)dcc_app_load_dotenv();

    const char *token = env_token();
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "set DCC_TOKEN, BOT_TOKEN, or DISCORD_TOKEN to run the cluster rollout example\n");
        return 0;
    }

    uint32_t shard_count = env_u32("DCC_CLUSTER_SHARDS", 1U, 1U, 1024U);
    uint32_t total_shards = env_u32("DCC_CLUSTER_TOTAL_SHARDS", shard_count, shard_count, 4096U);
    uint32_t first_shard = env_u32("DCC_CLUSTER_FIRST_SHARD", 0U, 0U, total_shards - 1U);
    if (shard_count > total_shards - first_shard) {
        fprintf(stderr, "invalid shard window: first=%u count=%u total=%u\n",
                first_shard, shard_count, total_shards);
        return 1;
    }

    rollout_state_t state;
    memset(&state, 0, sizeof(state));
    state.shard_count = shard_count;
    state.rollout_batch_size = env_u32("DCC_CLUSTER_ROLLOUT_BATCH", 1U, 1U, shard_count);
    state.rollout_delay_ms = env_u32("DCC_CLUSTER_ROLLOUT_DELAY_MS", 5000U, 0U, 600000U);
    state.rollout_resume = env_bool("DCC_CLUSTER_ROLLOUT_RESUME", 0) ? 1U : 0U;
    state.rollout_on_ready = env_bool("DCC_CLUSTER_ROLLOUT_ON_READY", 0);
    state.exit_after_ready = env_bool("DCC_CLUSTER_EXIT_AFTER_READY", 1);
    atomic_init(&state.ready_events, 0U);
    atomic_init(&state.rollout_started, 0U);
    atomic_init(&state.rollout_status, (unsigned)DCC_ERR_STATE);
    atomic_init(&state.stop_requested, 0U);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = DCC_CLIENT_OPTIONS(token, DCC_INTENTS_DEFAULT),
        .shard_count = shard_count,
        .first_shard_id = first_shard,
        .total_shard_count = total_shards,
        .gateway_max_concurrency = env_u32("DCC_CLUSTER_MAX_CONCURRENCY", 1U, 1U, 16U),
        .shard_restart_max = env_u32("DCC_CLUSTER_RESTART_MAX", 3U, 0U, 100U),
        .shard_restart_base_delay_ms = env_u32("DCC_CLUSTER_RESTART_BASE_MS", 250U, 1U, 600000U),
        .shard_restart_max_delay_ms = env_u32("DCC_CLUSTER_RESTART_MAX_MS", 5000U, 1U, 600000U),
    };

    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "cluster create failed: %s\n", dcc_status_string(status));
        return 1;
    }
    state.cluster = cluster;

    status = dcc_cluster_on(cluster, DCC_EVENT_LOG, on_log, NULL, NULL, 0);
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_READY, on_ready, &state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_start(cluster);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }

    print_summary(cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "cluster run failed: %s\n", dcc_status_string(status));
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}
