#include <dcc/dcc.h>
#include <llam/runtime.h>

#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct soak_state {
    dcc_cluster_t *cluster;
    uint32_t shard_count;
    uint32_t duration_seconds;
    uint32_t interval_seconds;
    uint32_t raw_sample;
    uint8_t print_shards;
    atomic_uint ready_events;
    atomic_uint resumed_events;
    atomic_uint socket_closes;
    atomic_uint rate_limited;
    atomic_uint log_errors;
    atomic_uint monitor_started;
    atomic_uint stop_requested;
    atomic_ulong raw_events;
    _Atomic(uintptr_t) monitor_group;
} soak_state_t;

static int env_bool(const char *name, int fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }
    return strcmp(value, "0") != 0 &&
        strcmp(value, "false") != 0 &&
        strcmp(value, "FALSE") != 0 &&
        strcmp(value, "no") != 0 &&
        strcmp(value, "NO") != 0;
}

static uint32_t env_u32(const char *name, uint32_t fallback, uint32_t min_value, uint32_t max_value) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }

    char *end = NULL;
    unsigned long parsed = strtoul(value, &end, 10);
    if (end == value || *end != '\0' || parsed < (unsigned long)min_value ||
        parsed > (unsigned long)max_value) {
        fprintf(stderr, "ignoring invalid %s=%s, using %u\n", name, value, (unsigned)fallback);
        return fallback;
    }
    return (uint32_t)parsed;
}

static uint64_t env_u64(const char *name, uint64_t fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        return fallback;
    }

    char *end = NULL;
    unsigned long long parsed = strtoull(value, &end, 0);
    if (end == value || *end != '\0') {
        fprintf(stderr, "ignoring invalid %s=%s, using %llu\n",
                name,
                value,
                (unsigned long long)fallback);
        return fallback;
    }
    return (uint64_t)parsed;
}

static const char *env_token(void) {
    const char *token = getenv("BOT_TOKEN");
    if (token == NULL || token[0] == '\0') {
        token = getenv("DISCORD_TOKEN");
    }
    return token;
}

static void print_gateway_info(dcc_cluster_t *cluster) {
    dcc_gateway_info_t info = {
        .size = sizeof(info),
    };
    if (dcc_cluster_gateway_info(cluster, &info) != DCC_OK) {
        return;
    }

    printf(
        "gateway: recommended_shards=%u max_concurrency=%u remaining_identifies=%u/%u "
        "reset_after_ms=%llu url=%s\n",
        info.recommended_shards,
        info.max_concurrency,
        info.remaining_identifies,
        info.total_identifies,
        (unsigned long long)info.reset_after_ms,
        info.url != NULL ? info.url : ""
    );
}

static void print_shards(soak_state_t *state) {
    if (!state->print_shards) {
        return;
    }

    for (uint32_t i = 0; i < state->shard_count; ++i) {
        dcc_cluster_shard_info_t shard = {
            .size = sizeof(shard),
        };
        if (dcc_cluster_shard_info(state->cluster, i, &shard) != DCC_OK) {
            continue;
        }
        printf(
            "  shard[%u] id=%u status=%s action=%s starts=%llu ready=%llu resumed=%llu closes=%llu "
            "restarts=%llu suppressed=%llu last_close=%u error=%s\n",
            i,
            shard.shard_id,
            dcc_cluster_shard_status_string(shard.status),
            dcc_cluster_recovery_action_string(dcc_cluster_shard_recovery_action(&shard)),
            (unsigned long long)shard.starts,
            (unsigned long long)shard.ready_events,
            (unsigned long long)shard.resumed_events,
            (unsigned long long)shard.socket_closes,
            (unsigned long long)shard.restarts,
            (unsigned long long)shard.restart_suppressed,
            shard.last_close_code,
            shard.last_error
        );
    }
}

static void print_summary(soak_state_t *state, const char *label) {
    dcc_cluster_recovery_plan_t plan = {
        .size = sizeof(plan),
    };
    if (dcc_cluster_recovery_plan(state->cluster, &plan) != DCC_OK) {
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
        "%s: health=%s action=%s action_shard=%s total=%u ready=%u stopped=%u error=%u starts=%llu restarts=%llu "
        "suppressed=%llu budget_used=%llu ready_events=%llu resumed=%llu closes=%llu "
        "raw=%lu rate_limited=%u log_errors=%u reason=%s\n",
        label,
        dcc_cluster_health_state_string(plan.health),
        dcc_cluster_recovery_action_string(plan.action),
        action_shard,
        summary->total_shards,
        summary->ready_shards,
        summary->stopped_shards,
        summary->error_shards,
        (unsigned long long)summary->starts,
        (unsigned long long)summary->restarts,
        (unsigned long long)summary->restart_suppressed,
        (unsigned long long)summary->restart_budget_used,
        (unsigned long long)summary->ready_events,
        (unsigned long long)summary->resumed_events,
        (unsigned long long)summary->socket_closes,
        atomic_load_explicit(&state->raw_events, memory_order_relaxed),
        atomic_load_explicit(&state->rate_limited, memory_order_relaxed),
        atomic_load_explicit(&state->log_errors, memory_order_relaxed),
        plan.reason
    );
    print_gateway_info(state->cluster);
    print_shards(state);
}

static void monitor_task(void *arg) {
    soak_state_t *state = (soak_state_t *)arg;
    uint32_t elapsed = 0;

    print_summary(state, "soak start");
    while (elapsed < state->duration_seconds &&
           atomic_load_explicit(&state->stop_requested, memory_order_acquire) == 0U) {
        uint32_t remaining = state->duration_seconds - elapsed;
        uint32_t step = state->interval_seconds < remaining ? state->interval_seconds : remaining;
        if (step == 0U) {
            break;
        }
        if (llam_sleep_ns((uint64_t)step * UINT64_C(1000000000)) != 0) {
            return;
        }
        elapsed += step;
        print_summary(state, "soak tick");
    }

    if (atomic_exchange_explicit(&state->stop_requested, 1U, memory_order_acq_rel) == 0U) {
        print_summary(state, "soak complete");
        (void)dcc_cluster_stop(state->cluster);
    }
}

static void start_monitor_if_ready(dcc_client_t *client, soak_state_t *state, unsigned ready_events) {
    if (ready_events < state->shard_count ||
        atomic_exchange_explicit(&state->monitor_started, 1U, memory_order_acq_rel) != 0U) {
        return;
    }

    dcc_task_group_t *group = NULL;
    dcc_status_t status = dcc_task_group_create(client, &group);
    if (status != DCC_OK) {
        fprintf(stderr, "soak monitor create failed: %s\n", dcc_status_string(status));
        (void)dcc_cluster_stop(state->cluster);
        return;
    }

    atomic_store_explicit(&state->monitor_group, (uintptr_t)group, memory_order_release);
    dcc_task_options_t options = {
        .size = sizeof(options),
        .task_class = DCC_TASK_CLASS_BATCH,
    };
    status = dcc_task_group_spawn(group, monitor_task, state, &options);
    if (status != DCC_OK) {
        fprintf(stderr, "soak monitor spawn failed: %s\n", dcc_status_string(status));
        atomic_store_explicit(&state->monitor_group, (uintptr_t)0, memory_order_release);
        (void)dcc_task_group_destroy(group);
        (void)dcc_cluster_stop(state->cluster);
    }
}

static void on_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_ready_event_t *ready = dcc_event_ready(event);
    if (ready == NULL) {
        return;
    }

    unsigned ready_events = atomic_fetch_add_explicit(&state->ready_events, 1U, memory_order_relaxed) + 1U;
    printf("READY shard=%u/%u event=%u guilds=%u\n",
           ready->shard_id,
           ready->shard_count,
           ready_events,
           ready->guild_count);
    start_monitor_if_ready(client, state, ready_events);
}

static void on_resumed(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_ready_event_t *resumed = dcc_event_resumed(event);
    if (resumed != NULL) {
        unsigned count = atomic_fetch_add_explicit(&state->resumed_events, 1U, memory_order_relaxed) + 1U;
        printf("RESUMED shard=%u/%u event=%u\n", resumed->shard_id, resumed->shard_count, count);
    }
}

static void on_socket_close(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_socket_close_event_t *close_event = dcc_event_socket_close(event);
    if (close_event != NULL) {
        unsigned count = atomic_fetch_add_explicit(&state->socket_closes, 1U, memory_order_relaxed) + 1U;
        printf("SOCKET_CLOSE count=%u code=%u reason=%s\n",
               count,
               close_event->code,
               close_event->reason != NULL ? close_event->reason : "");
    }
}

static void on_rate_limited(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_rate_limited_event_t *rate = dcc_event_rate_limited(event);
    if (rate != NULL) {
        unsigned count = atomic_fetch_add_explicit(&state->rate_limited, 1U, memory_order_relaxed) + 1U;
        printf("RATE_LIMIT count=%u status=%u retry_after=%.3f global=%u %s %s\n",
               count,
               rate->status,
               rate->retry_after,
               rate->global,
               rate->method != NULL ? rate->method : "",
               rate->path != NULL ? rate->path : "");
    }
}

static void on_raw(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_raw_event_t *raw = dcc_event_raw(event);
    unsigned long count = atomic_fetch_add_explicit(&state->raw_events, 1UL, memory_order_relaxed) + 1UL;
    if (raw != NULL && state->raw_sample != 0U && count % state->raw_sample == 0UL) {
        printf("RAW sample=%lu seq=%llu name=%s type=%s\n",
               count,
               (unsigned long long)raw->sequence,
               raw->name != NULL ? raw->name : "",
               dcc_event_type_name(raw->type));
    }
}

static void on_log(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    soak_state_t *state = (soak_state_t *)user_data;
    const dcc_log_event_t *log = dcc_event_log(event);
    if (log == NULL) {
        return;
    }
    if (log->level >= DCC_LOG_ERROR) {
        atomic_fetch_add_explicit(&state->log_errors, 1U, memory_order_relaxed);
    }
    fprintf(stderr, "[dcc:%d] %s\n", (int)log->level, log->message != NULL ? log->message : "");
}

static int register_handlers(dcc_cluster_t *cluster, soak_state_t *state) {
    dcc_status_t status = dcc_cluster_on(cluster, DCC_EVENT_LOG, on_log, state, NULL, 0);
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_READY, on_ready, state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_RESUMED, on_resumed, state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_SOCKET_CLOSE, on_socket_close, state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_RATE_LIMITED, on_rate_limited, state, NULL, 0);
    }
    if (status == DCC_OK) {
        status = dcc_cluster_on(cluster, DCC_EVENT_RAW, on_raw, state, NULL, 0);
    }
    if (status != DCC_OK) {
        fprintf(stderr, "handler registration failed: %s\n", dcc_status_string(status));
        return 1;
    }
    return 0;
}

int main(void) {
    const char *token = env_token();
    if (token == NULL || token[0] == '\0') {
        fprintf(stderr, "set BOT_TOKEN or DISCORD_TOKEN to run the live gateway soak example\n");
        return 0;
    }

    uint32_t shard_count = env_u32("DCC_SOAK_SHARDS", 1U, 1U, 1024U);
    uint32_t total_shards = env_u32("DCC_SOAK_TOTAL_SHARDS", shard_count, shard_count, 4096U);
    uint32_t first_shard = env_u32("DCC_SOAK_FIRST_SHARD", 0U, 0U, total_shards - 1U);
    if (shard_count > total_shards - first_shard) {
        fprintf(stderr, "invalid shard window: first=%u count=%u total=%u\n",
                first_shard,
                shard_count,
                total_shards);
        return 1;
    }

    soak_state_t state;
    memset(&state, 0, sizeof(state));
    state.shard_count = shard_count;
    state.duration_seconds = env_u32("DCC_SOAK_SECONDS", 300U, 1U, 86400U);
    state.interval_seconds = env_u32("DCC_SOAK_INTERVAL_SECONDS", 30U, 1U, 3600U);
    state.raw_sample = env_u32("DCC_SOAK_RAW_SAMPLE", 0U, 0U, 1000000U);
    state.print_shards = env_bool("DCC_SOAK_PRINT_SHARDS", 1) ? 1U : 0U;
    atomic_init(&state.ready_events, 0U);
    atomic_init(&state.resumed_events, 0U);
    atomic_init(&state.socket_closes, 0U);
    atomic_init(&state.rate_limited, 0U);
    atomic_init(&state.log_errors, 0U);
    atomic_init(&state.monitor_started, 0U);
    atomic_init(&state.stop_requested, 0U);
    atomic_init(&state.raw_events, 0UL);
    atomic_init(&state.monitor_group, (uintptr_t)0);

    dcc_cluster_t *cluster = NULL;
    dcc_cluster_options_t opts = {
        .size = sizeof(opts),
        .client_options = {
            .size = sizeof(dcc_client_options_t),
            .token = token,
            .intents = (dcc_intents_t)env_u64("DCC_SOAK_INTENTS", DCC_INTENT_GUILDS),
        },
        .shard_count = shard_count,
        .first_shard_id = first_shard,
        .total_shard_count = total_shards,
        .gateway_max_concurrency = env_u32("DCC_SOAK_MAX_CONCURRENCY", 1U, 1U, 16U),
        .shard_restart_max = env_u32("DCC_SOAK_RESTART_MAX", 3U, 0U, 100U),
        .shard_restart_base_delay_ms = env_u32("DCC_SOAK_RESTART_BASE_MS", 250U, 1U, 600000U),
        .shard_restart_max_delay_ms = env_u32("DCC_SOAK_RESTART_MAX_MS", 5000U, 1U, 600000U),
    };

    dcc_status_t status = dcc_cluster_create(&opts, &cluster);
    if (status != DCC_OK) {
        fprintf(stderr, "cluster create failed: %s\n", dcc_status_string(status));
        return 1;
    }
    state.cluster = cluster;

    if (register_handlers(cluster, &state) != 0) {
        dcc_cluster_destroy(cluster);
        return 1;
    }

    if (env_bool("DCC_SOAK_FETCH_GATEWAY", 1)) {
        status = dcc_cluster_fetch_gateway_info(cluster);
        if (status != DCC_OK) {
            fprintf(stderr, "gateway discovery failed: %s\n", dcc_status_string(status));
            dcc_cluster_destroy(cluster);
            return 1;
        }
        print_gateway_info(cluster);
    }

    status = dcc_cluster_start(cluster);
    if (status == DCC_OK) {
        status = dcc_cluster_wait(cluster);
    }

    dcc_task_group_t *monitor_group =
        (dcc_task_group_t *)(uintptr_t)atomic_load_explicit(&state.monitor_group, memory_order_acquire);
    if (monitor_group != NULL) {
        dcc_task_group_wait_result_t result = {
            .size = sizeof(result),
        };
        (void)dcc_task_group_wait_result(monitor_group, 2000U, &result);
        (void)dcc_task_group_destroy(monitor_group);
        atomic_store_explicit(&state.monitor_group, (uintptr_t)0, memory_order_release);
    }

    print_summary(&state, "final");
    if (status != DCC_OK) {
        fprintf(stderr, "cluster run failed: %s\n", dcc_status_string(status));
        dcc_cluster_destroy(cluster);
        return 1;
    }

    dcc_cluster_destroy(cluster);
    return 0;
}
