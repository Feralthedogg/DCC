/* DCC_DOC_SNIPPET_BEGIN(command-registry) */
#include <dcc/command_registry.h>

/* Pure planning: no Discord connection. Pass 0 for global scope, otherwise
 * an explicit guild id. The caller owns a successful out_plan. */
dcc_status_t dcc_example_command_plan(dcc_snowflake_t guild_id,
                                      dcc_command_registry_plan_t *out_plan) {
    dcc_command_registry_options_t options = DCC_COMMAND_REGISTRY_OPTIONS_INIT;
    dcc_status_t status = guild_id == 0U
        ? dcc_command_registry_options_set_global(&options)
        : dcc_command_registry_options_set_guild(&options, guild_id);
    if (status != DCC_OK) return status;
    status = dcc_command_registry_options_set_delete_stale(&options, 1U);
    if (status != DCC_OK) return status;

    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    dcc_application_command_builder_t ping = DCC_APPLICATION_COMMAND_BUILDER_INIT;
    status = dcc_application_command_builder_set_type(
        &ping, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    if (status == DCC_OK)
        status = dcc_application_command_builder_set_name(&ping, "ping");
    if (status == DCC_OK)
        status = dcc_application_command_builder_set_description(&ping, "Latency check");
    if (status == DCC_OK) status = dcc_command_registry_add_builder(&registry, &ping);
    /* Fixture only: replace with the actual remote snapshot when reconciling. */
    dcc_command_registry_remote_entry_t remote[] = {{
        .size = sizeof(dcc_command_registry_remote_entry_t),
        .id = 123U,
        .name = "old-command",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .canonical_json = "{\"name\":\"old-command\",\"type\":1}",
    }};
    if (status == DCC_OK)
        status = dcc_command_registry_build_plan(&registry, &options, remote,
            sizeof(remote) / sizeof(remote[0]), out_plan);
    dcc_command_registry_deinit(&registry);
    return status;
}
/* DCC_DOC_SNIPPET_END(command-registry) */

/* DCC_DOC_SNIPPET_BEGIN(replay-records) */
#include <dcc/replay.h>

dcc_status_t dcc_example_record_pair(dcc_replay_recorder_t *recorder,
    uint64_t now_ms, const char *message_json, size_t message_len,
    const char *interaction_json, size_t interaction_len
) {
    dcc_replay_record_t gateway = {
        .size = sizeof(gateway), .kind = DCC_REPLAY_GATEWAY,
        .ts_ms = now_ms, .event = "MESSAGE_CREATE",
        .payload = message_json, .payload_len = message_len,
    };
    dcc_replay_record_t interaction = {
        .size = sizeof(interaction), .kind = DCC_REPLAY_INTERACTION,
        .ts_ms = now_ms, .event = NULL,
        .payload = interaction_json, .payload_len = interaction_len,
    };
    dcc_status_t status = dcc_replay_recorder_write(recorder, &gateway);
    if (status == DCC_OK)
        status = dcc_replay_recorder_write(recorder, &interaction);
    /* These records borrow caller payloads; do not record_deinit them.
     * Deinit records returned by dcc_replay_player_next instead. */
    return status;
}
/* DCC_DOC_SNIPPET_END(replay-records) */

/* DCC_DOC_SNIPPET_BEGIN(rest-firewall) */
#include <dcc/rest/firewall.h>

dcc_status_t dcc_example_firewall_attach(dcc_client_t *client) {
    dcc_rest_firewall_options_t options;
    dcc_rest_firewall_options_init(&options);
    options.invalid_request_soft_limit = 8000U;
    options.invalid_request_hard_limit = 9500U;
    return dcc_rest_firewall_attach(client, &options);
}

dcc_status_t dcc_example_firewall_json(dcc_client_t *client,
                                      char *json, size_t capacity, size_t *length) {
    dcc_rest_firewall_snapshot_t snapshot;
    dcc_status_t status = dcc_rest_firewall_snapshot(client, &snapshot);
    if (status == DCC_OK)
        status = dcc_rest_firewall_snapshot_json(&snapshot, json, capacity, length);
    return status;
}
/* DCC_DOC_SNIPPET_END(rest-firewall) */

/* DCC_DOC_SNIPPET_BEGIN(hot-reload-canary) */
#include <dcc/hot_reload.h>

/* worker_path must remain valid while the caller uses these options. */
dcc_hot_reload_options_t dcc_example_canary_options(const char *worker_path) {
    dcc_hot_reload_canary_options_t canary;
    dcc_hot_reload_canary_options_init(&canary);
    canary.canary_percent = 5U;
    canary.promote_after_ms = 30000U;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = worker_path,
        .worker_health_timeout_ms = 5000U,
        .worker_drain_timeout_ms = 1000U,
        .worker_canary_enabled = 1U,
        .worker_canary_options = canary,
    };
    return options;
}
/* DCC_DOC_SNIPPET_END(hot-reload-canary) */

/* Offline link/ownership harness: does not create a client or worker. */
int main(void) {
    dcc_command_registry_plan_t plan;
    dcc_status_t status = dcc_example_command_plan(0U, &plan);
    if (status != DCC_OK) return 1;
    dcc_command_registry_plan_deinit(&plan);
    dcc_hot_reload_options_t options = dcc_example_canary_options("dcc_hot_reload_worker");
    return dcc_hot_reload_canary_options_validate(&options.worker_canary_options) == DCC_OK
        ? 0 : 1;
}
