#include <dcc/dcc.h>

#include <string.h>

int main(void) {
    const char *version = dcc_version_string();
    dcc_hot_reload_options_t hot_reload_options;
    dcc_hot_reload_health_snapshot_t hot_reload_health;
    dcc_application_command_registration_options_t command_options;
    dcc_event_wait_policy_t gateway_ready_policy;
    dcc_event_wait_policy_t interaction_policy;
    dcc_event_wait_policy_t voice_descriptor_policy;
    dcc_event_snapshot_t voice_descriptor_snapshots[2];
    dcc_interaction_t *wait_interaction = NULL;
    dcc_voice_connect_wait_options_t voice_join_options;
    dcc_voice_connect_wait_result_t voice_join_result;
    dcc_interaction_server_health_snapshot_t interaction_health;
    dcc_cluster_remediation_loop_options_t remediation_options;
    dcc_cluster_remediation_loop_result_t remediation_result;
    dcc_cluster_health_wait_options_t cluster_health_wait_options;
    dcc_cluster_operation_status_t operation;
    dcc_cluster_supervisor_status_t supervisor_status;

    memset(&hot_reload_options, 0, sizeof(hot_reload_options));
    hot_reload_options.size = sizeof(hot_reload_options);
    memset(&hot_reload_health, 0, sizeof(hot_reload_health));
    hot_reload_health.size = sizeof(hot_reload_health);
    hot_reload_health.backend = DCC_HOT_RELOAD_BACKEND_IN_PROCESS;
    dcc_application_command_registration_options_init(&command_options);
    if (dcc_application_command_registration_options_set_guild(&command_options, 123) != DCC_OK ||
        dcc_application_command_registration_options_set_global(&command_options) != DCC_OK) {
        return 1;
    }
    if (dcc_event_wait_policy_init_gateway_ready_or_resumed(&gateway_ready_policy, 25) != DCC_OK) {
        return 1;
    }
    if (dcc_event_wait_policy_init_interaction_or_close(&interaction_policy, 25) != DCC_OK) {
        return 1;
    }
    memset(voice_descriptor_snapshots, 0, sizeof(voice_descriptor_snapshots));
    voice_descriptor_snapshots[0].size = sizeof(voice_descriptor_snapshots[0]);
    voice_descriptor_snapshots[1].size = sizeof(voice_descriptor_snapshots[1]);
    if (dcc_event_wait_policy_init_voice_session_descriptor(&voice_descriptor_policy, 25) != DCC_OK) {
        return 1;
    }
    dcc_voice_connect_wait_options_init(&voice_join_options);
    memset(&voice_join_result, 0, sizeof(voice_join_result));
    voice_join_result.size = sizeof(voice_join_result);
    dcc_cluster_remediation_loop_options_init(&remediation_options);
    dcc_cluster_health_wait_options_init(&cluster_health_wait_options);
    memset(&remediation_result, 0, sizeof(remediation_result));
    remediation_result.size = sizeof(remediation_result);
    memset(&operation, 0, sizeof(operation));
    operation.size = sizeof(operation);
    memset(&supervisor_status, 0, sizeof(supervisor_status));
    supervisor_status.size = sizeof(supervisor_status);

    memset(&interaction_health, 0, sizeof(interaction_health));
    interaction_health.size = sizeof(interaction_health);

    return version != NULL &&
            strcmp(version, DCC_VERSION_STRING) == 0 &&
            strcmp(DCC_HOT_RELOAD_DEFAULT_WORKER, "dcc_hot_reload_worker") == 0 &&
            hot_reload_options.backend == DCC_HOT_RELOAD_BACKEND_IN_PROCESS &&
            hot_reload_health.backend == DCC_HOT_RELOAD_BACKEND_IN_PROCESS &&
            hot_reload_health.active_worker_generation == 0 &&
            hot_reload_health.active_worker_pid == 0 &&
            hot_reload_health.worker_dispatch_failures == 0 &&
            dcc_hot_reload_wait_until_loaded(NULL, 1, &hot_reload_health) ==
                DCC_ERR_INVALID_ARG &&
            command_options.guild_id == 0 &&
            gateway_ready_policy.types != NULL &&
            gateway_ready_policy.types[0] == DCC_EVENT_READY &&
            gateway_ready_policy.types[1] == DCC_EVENT_RESUMED &&
            interaction_policy.mode == DCC_EVENT_WAIT_POLICY_ANY &&
            interaction_policy.type_count == 9 &&
            interaction_policy.types[0] == DCC_EVENT_SLASH_COMMAND &&
            interaction_policy.types[8] == DCC_EVENT_SOCKET_CLOSE &&
            voice_descriptor_policy.mode == DCC_EVENT_WAIT_POLICY_SET &&
            voice_descriptor_policy.types[0] == DCC_EVENT_VOICE_STATE_UPDATE &&
            voice_descriptor_policy.types[1] == DCC_EVENT_VOICE_SERVER_UPDATE &&
            dcc_event_wait_policy_init_gateway_ready_resumed_or_close(&gateway_ready_policy, 25) ==
                DCC_OK &&
            gateway_ready_policy.type_count == 3 &&
            gateway_ready_policy.types[2] == DCC_EVENT_SOCKET_CLOSE &&
            dcc_client_wait_for_gateway_ready_or_resumed(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_for_gateway_ready_resumed_or_close(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_until_gateway_ready(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_for_interaction_or_close(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_until_interaction(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_for_interaction_or_close_owned(NULL, 1, NULL, &wait_interaction) ==
                DCC_ERR_INVALID_ARG &&
            dcc_client_wait_until_interaction_owned(NULL, 1, NULL, &wait_interaction) ==
                DCC_ERR_INVALID_ARG &&
            wait_interaction == NULL &&
            dcc_client_wait_for_voice_session_descriptor(NULL, 1, voice_descriptor_snapshots, 2, NULL) ==
                DCC_ERR_INVALID_ARG &&
            voice_join_options.size == sizeof(voice_join_options) &&
            voice_join_options.descriptor_timeout_ms == 30000U &&
            voice_join_options.ready_timeout_ms == 30000U &&
            dcc_voice_client_connect_and_wait_until_ready(NULL, &voice_join_options, &voice_join_result) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_wait_until_ready(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            cluster_health_wait_options.size == sizeof(cluster_health_wait_options) &&
            cluster_health_wait_options.target_health == DCC_CLUSTER_HEALTH_HEALTHY &&
            dcc_cluster_wait_until_health(NULL, &cluster_health_wait_options, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_supervisor_status(NULL, &supervisor_status) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_get_supervisor_decision_callback(NULL, NULL, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_supervisor_decision_event_json(NULL, NULL, 0, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_voice_client_wait_until_ready(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_voice_client_wait_until_active(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            interaction_health.size == sizeof(interaction_health) &&
            dcc_interaction_server_wait_until_ready(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_interaction_server_wait_until_drained(NULL, 1, NULL) ==
                DCC_ERR_INVALID_ARG &&
            remediation_options.size == sizeof(remediation_options) &&
            dcc_cluster_remediation_loop_result_json(&remediation_result, NULL, 0, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_operation_status_json(&operation, NULL, 0, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_status_json(NULL, NULL, 0, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_cluster_add_status_route(NULL, NULL, NULL) ==
                DCC_ERR_INVALID_ARG &&
            dcc_hot_reload_add_health_route(NULL, NULL, NULL) ==
                DCC_ERR_INVALID_ARG
        ? 0
        : 1;
}
