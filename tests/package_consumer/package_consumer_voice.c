#include "package_consumer_checks.h"

int dcc_package_consumer_check_voice_api(void) {
    dcc_package_voice_state_fn voice_state_modify = dcc_rest_modify_current_user_voice_state_params;
    dcc_package_gateway_voice_state_fn gateway_voice_state_update = dcc_client_update_voice_state;
    dcc_package_voice_connect_fn voice_connect = dcc_voice_client_connect;
    dcc_package_voice_leave_fn voice_leave = dcc_voice_client_leave;
    dcc_package_voice_ws_start_fn voice_ws_start = dcc_voice_client_start_websocket_loop;
    dcc_package_voice_ws_stop_fn voice_ws_stop = dcc_voice_client_stop_websocket_loop;
    dcc_package_voice_ws_running_fn voice_ws_running = dcc_voice_client_websocket_loop_running;
    dcc_package_voice_ws_status_fn voice_ws_status = dcc_voice_client_websocket_loop_status;
    dcc_package_voice_stats_fn voice_stats = dcc_voice_client_stats;
    dcc_package_voice_health_fn voice_health = dcc_voice_client_health_snapshot;
    dcc_package_voice_wait_fn voice_wait_ready = dcc_voice_client_wait_until_ready;
    dcc_package_voice_wait_fn voice_wait_active = dcc_voice_client_wait_until_active;
    dcc_package_voice_connect_wait_options_init_fn voice_connect_wait_options_init =
        dcc_voice_connect_wait_options_init;
    dcc_package_voice_connect_wait_fn voice_connect_wait_ready =
        dcc_voice_client_connect_and_wait_until_ready;
    dcc_package_voice_health_string_fn voice_health_string = dcc_voice_health_string;
    dcc_package_voice_state_string_fn voice_state_string = dcc_voice_session_state_string;
    dcc_package_voice_health_json_fn voice_health_json = dcc_voice_client_health_snapshot_json;
    dcc_package_voice_close_action_fn voice_close_action = dcc_voice_reconnect_action_for_close_code;
    dcc_package_voice_client_action_fn voice_client_action = dcc_voice_client_reconnect_action;
    dcc_package_voice_action_string_fn voice_action_string = dcc_voice_reconnect_action_string;
    dcc_package_voice_dave_handler_fn voice_dave_handler = dcc_voice_client_set_dave_mls_handler;
    dcc_package_voice_dave_opcode_string_fn voice_dave_opcode_string = dcc_voice_dave_mls_opcode_string;
    dcc_package_voice_dave_parse_fn voice_dave_parse = dcc_voice_parse_dave_mls_gateway_frame;
    dcc_package_voice_dave_gateway_build_fn voice_dave_gateway_build = dcc_voice_build_dave_mls_gateway_frame;
    dcc_package_voice_dave_binary_fn voice_dave_binary = dcc_voice_client_handle_gateway_binary_frame;
    dcc_package_voice_dave_build_fn voice_dave_ready_build = dcc_voice_client_build_dave_transition_ready_payload;
    dcc_package_voice_dave_build_fn voice_dave_invalid_build = dcc_voice_client_build_dave_invalid_commit_payload;
    dcc_package_voice_dave_transition_fn voice_dave_ready_send = dcc_voice_client_send_dave_transition_ready;
    dcc_package_voice_dave_binary_fn voice_dave_key_package = dcc_voice_client_send_dave_mls_key_package;
    dcc_package_voice_dave_binary_fn voice_dave_commit = dcc_voice_client_send_dave_mls_commit_welcome;
    dcc_package_voice_dave_transition_fn voice_dave_invalid_send =
        dcc_voice_client_send_dave_invalid_commit_welcome;
    dcc_voice_connect_wait_options_t package_voice_connect_options;
    dcc_voice_connect_wait_result_t package_voice_connect_result = { .size = sizeof(package_voice_connect_result) };
    int voice_connect_options_ok = voice_connect_wait_options_init != NULL && voice_connect_wait_ready != NULL;
    if (voice_connect_options_ok) {
        voice_connect_wait_options_init(&package_voice_connect_options);
        voice_connect_options_ok = package_voice_connect_options.size == sizeof(package_voice_connect_options) &&
            package_voice_connect_options.descriptor_timeout_ms == 30000U &&
            package_voice_connect_options.ready_timeout_ms == 30000U &&
            voice_connect_wait_ready(NULL, &package_voice_connect_options, &package_voice_connect_result) ==
                DCC_ERR_INVALID_ARG;
    }

    return voice_state_modify != NULL &&
           gateway_voice_state_update != NULL &&
           voice_connect != NULL &&
           voice_leave != NULL &&
           voice_ws_start != NULL &&
           voice_ws_stop != NULL &&
           voice_ws_running != NULL &&
           voice_ws_status != NULL &&
           voice_stats != NULL &&
           voice_health != NULL &&
           voice_wait_ready != NULL &&
           voice_wait_active != NULL &&
           voice_connect_wait_options_init != NULL &&
           voice_connect_wait_ready != NULL &&
           voice_health_string != NULL &&
           voice_state_string != NULL &&
           voice_health_json != NULL &&
           voice_close_action != NULL &&
           voice_client_action != NULL &&
           voice_action_string != NULL &&
           voice_health(NULL, NULL) == DCC_ERR_INVALID_ARG &&
           voice_wait_ready(NULL, 1U, NULL) == DCC_ERR_INVALID_ARG &&
           voice_wait_active(NULL, 1U, NULL) == DCC_ERR_INVALID_ARG &&
           voice_connect_wait_ready(NULL, NULL, NULL) == DCC_ERR_INVALID_ARG &&
           voice_health_string(DCC_VOICE_HEALTH_ACTIVE) != NULL &&
           voice_state_string(DCC_VOICE_SESSION_ACTIVE) != NULL &&
           voice_close_action(4014U) == DCC_VOICE_RECONNECT_ACTION_TERMINATE &&
           voice_action_string(DCC_VOICE_RECONNECT_ACTION_FULL_RECONNECT) != NULL &&
           voice_dave_handler != NULL &&
           voice_dave_opcode_string != NULL &&
           voice_dave_parse != NULL &&
           voice_dave_gateway_build != NULL &&
           voice_dave_binary != NULL &&
           voice_dave_ready_build != NULL &&
           voice_dave_invalid_build != NULL &&
           voice_dave_ready_send != NULL &&
           voice_dave_key_package != NULL &&
           voice_dave_commit != NULL &&
           voice_dave_invalid_send != NULL &&
           voice_connect_options_ok != 0;
}
