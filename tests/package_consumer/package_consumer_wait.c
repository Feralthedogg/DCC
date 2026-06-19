#include "package_consumer_checks.h"

#include <string.h>

int dcc_package_consumer_check_wait_api(void) {
    dcc_package_wait_for_event_any_fn wait_for_event_any = dcc_client_wait_for_event_any;
    dcc_package_wait_for_event_sequence_fn wait_for_event_sequence = dcc_client_wait_for_event_sequence;
    dcc_package_wait_for_event_sequence_fn wait_for_event_set = dcc_client_wait_for_event_set;
    dcc_package_wait_for_event_count_fn wait_for_event_count = dcc_client_wait_for_event_count;
    dcc_package_wait_for_event_count_fn wait_for_event_quorum = dcc_client_wait_for_event_quorum;
    dcc_status_t (*wait_for_voice_descriptor)(dcc_client_t *, uint32_t,
        dcc_event_snapshot_t *, size_t, size_t *) = dcc_client_wait_for_voice_session_descriptor;
    dcc_package_event_wait_policy_init_fn wait_policy_init = dcc_event_wait_policy_init;
    dcc_package_event_wait_policy_init_any_fn wait_policy_init_any = dcc_event_wait_policy_init_any;
    dcc_package_event_wait_policy_init_any_fn wait_policy_init_sequence =
        dcc_event_wait_policy_init_sequence;
    dcc_package_event_wait_policy_init_any_fn wait_policy_init_set = dcc_event_wait_policy_init_set;
    dcc_package_event_wait_policy_init_count_fn wait_policy_init_count =
        dcc_event_wait_policy_init_count;
    dcc_package_event_wait_policy_init_count_fn wait_policy_init_quorum =
        dcc_event_wait_policy_init_quorum;
    dcc_status_t (*wait_policy_init_voice_descriptor)(dcc_event_wait_policy_t *, uint32_t) =
        dcc_event_wait_policy_init_voice_session_descriptor;
    dcc_package_wait_for_gateway_data_any_fn wait_for_gateway_data_any =
        dcc_client_wait_for_gateway_data_any;
    dcc_package_wait_for_message_any_fn wait_for_message_any = dcc_client_wait_for_message_any;
    dcc_package_wait_for_guild_any_fn wait_for_guild_any = dcc_client_wait_for_guild_any;
    dcc_package_wait_for_channel_any_fn wait_for_channel_any = dcc_client_wait_for_channel_any;
    dcc_package_wait_for_interaction_any_fn wait_for_interaction_any =
        dcc_client_wait_for_interaction_any;
    dcc_package_wait_for_voice_state_any_fn wait_for_voice_state_any =
        dcc_client_wait_for_voice_state_any;
    dcc_package_wait_for_member_fn wait_for_member = dcc_client_wait_for_member;
    dcc_package_wait_for_role_fn wait_for_role = dcc_client_wait_for_role;
    dcc_package_wait_for_user_fn wait_for_user = dcc_client_wait_for_user;
    dcc_package_wait_for_member_any_fn wait_for_member_any = dcc_client_wait_for_member_any;
    dcc_package_wait_for_role_any_fn wait_for_role_any = dcc_client_wait_for_role_any;
    dcc_package_wait_for_user_any_fn wait_for_user_any = dcc_client_wait_for_user_any;
    dcc_event_type_t wait_any_types[] = {DCC_EVENT_MESSAGE_CREATE, DCC_EVENT_USER_UPDATE};
    dcc_event_snapshot_t wait_any_snapshot = { .size = sizeof(wait_any_snapshot) };
    dcc_event_snapshot_t wait_sequence_snapshots[2] = {
        { .size = sizeof(wait_sequence_snapshots[0]) },
        { .size = sizeof(wait_sequence_snapshots[1]) }
    };
    size_t wait_sequence_completed = 99U;
    dcc_event_wait_policy_t wait_policy;
    memset(&wait_policy, 0x5A, sizeof(wait_policy));
    wait_policy_init(&wait_policy);
    dcc_gateway_event_data_t *wait_data = NULL;
    dcc_message_t *wait_message = NULL;
    dcc_guild_t *wait_guild = NULL;
    dcc_channel_t *wait_channel = NULL;
    dcc_interaction_t *wait_interaction = NULL;
    dcc_voice_state_t *wait_voice_state = NULL;
    dcc_member_t *wait_member = NULL;
    dcc_role_t *wait_role = NULL;
    dcc_user_t *wait_user = NULL;

    return wait_for_event_any != NULL &&
           wait_for_event_sequence != NULL &&
           wait_for_event_set != NULL &&
           wait_for_event_count != NULL &&
           wait_for_event_quorum != NULL &&
           wait_for_voice_descriptor != NULL &&
           wait_policy_init != NULL &&
           wait_policy_init_any != NULL &&
           wait_policy_init_sequence != NULL &&
           wait_policy_init_set != NULL &&
           wait_policy_init_count != NULL &&
           wait_policy_init_quorum != NULL &&
           wait_policy_init_voice_descriptor != NULL &&
           wait_policy.size == sizeof(wait_policy) &&
           wait_policy.mode == 0 &&
           wait_policy.types == NULL &&
           wait_policy_init_any(&wait_policy, wait_any_types, 2U, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_ANY &&
           wait_policy.target_count == 1U &&
           wait_policy_init_sequence(&wait_policy, wait_any_types, 2U, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_SEQUENCE &&
           wait_policy.target_count == 2U &&
           wait_policy_init_set(&wait_policy, wait_any_types, 2U, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_SET &&
           wait_policy.target_count == 2U &&
           wait_policy_init_count(&wait_policy, wait_any_types, 2U, 2U, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_COUNT &&
           wait_policy.target_count == 2U &&
           wait_policy_init_quorum(&wait_policy, wait_any_types, 2U, 2U, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_QUORUM &&
           dcc_event_wait_policy_init_gateway_ready_or_resumed(&wait_policy, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_ANY &&
           wait_policy.types[0] == DCC_EVENT_READY &&
           wait_policy.types[1] == DCC_EVENT_RESUMED &&
           dcc_event_wait_policy_init_gateway_ready_resumed_or_close(&wait_policy, 1) == DCC_OK &&
           wait_policy.type_count == 3U &&
           wait_policy.types[2] == DCC_EVENT_SOCKET_CLOSE &&
           dcc_event_wait_policy_init_interaction_or_close(&wait_policy, 1) == DCC_OK &&
           wait_policy.type_count == 9U &&
           wait_policy.types[0] == DCC_EVENT_SLASH_COMMAND &&
           wait_policy.types[8] == DCC_EVENT_SOCKET_CLOSE &&
           wait_policy_init_voice_descriptor(&wait_policy, 1) == DCC_OK &&
           wait_policy.mode == DCC_EVENT_WAIT_POLICY_SET &&
           wait_policy.types[0] == DCC_EVENT_VOICE_STATE_UPDATE &&
           wait_policy.types[1] == DCC_EVENT_VOICE_SERVER_UPDATE &&
           wait_for_gateway_data_any != NULL &&
           wait_for_message_any != NULL &&
           wait_for_guild_any != NULL &&
           wait_for_channel_any != NULL &&
           wait_for_interaction_any != NULL &&
           wait_for_voice_state_any != NULL &&
           wait_for_member != NULL &&
           wait_for_role != NULL &&
           wait_for_user != NULL &&
           wait_for_member_any != NULL &&
           wait_for_role_any != NULL &&
           wait_for_user_any != NULL &&
           wait_for_event_any(NULL, wait_any_types, 2U, 1, &wait_any_snapshot) == DCC_ERR_INVALID_ARG &&
           dcc_client_wait_for_gateway_ready_or_resumed(NULL, 1, &wait_any_snapshot) ==
               DCC_ERR_INVALID_ARG &&
           dcc_client_wait_for_gateway_ready_resumed_or_close(NULL, 1, &wait_any_snapshot) ==
               DCC_ERR_INVALID_ARG &&
           dcc_client_wait_until_gateway_ready(NULL, 1, &wait_any_snapshot) == DCC_ERR_INVALID_ARG &&
           dcc_client_wait_for_interaction_or_close(NULL, 1, &wait_any_snapshot) ==
               DCC_ERR_INVALID_ARG &&
           dcc_client_wait_until_interaction(NULL, 1, &wait_any_snapshot) == DCC_ERR_INVALID_ARG &&
           dcc_client_wait_for_interaction_or_close_owned(NULL, 1, &wait_any_snapshot, &wait_interaction) ==
               DCC_ERR_INVALID_ARG &&
           dcc_client_wait_until_interaction_owned(NULL, 1, &wait_any_snapshot, &wait_interaction) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_voice_descriptor(NULL, 1, wait_sequence_snapshots, 2U, &wait_sequence_completed) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_event_sequence(NULL, wait_any_types, 2U, 1, wait_sequence_snapshots, 2U,
               &wait_sequence_completed) == DCC_ERR_INVALID_ARG &&
           wait_for_event_set(NULL, wait_any_types, 2U, 1, wait_sequence_snapshots, 2U,
               &wait_sequence_completed) == DCC_ERR_INVALID_ARG &&
           wait_for_event_count(NULL, wait_any_types, 2U, 2U, 1, wait_sequence_snapshots, 2U,
               &wait_sequence_completed) == DCC_ERR_INVALID_ARG &&
           wait_for_event_quorum(NULL, wait_any_types, 2U, 2U, 1, wait_sequence_snapshots, 2U,
               &wait_sequence_completed) == DCC_ERR_INVALID_ARG &&
           wait_for_gateway_data_any(NULL, wait_any_types, 2U, 1, NULL, &wait_data) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_message_any(NULL, wait_any_types, 2U, 1, NULL, &wait_message) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_guild_any(NULL, wait_any_types, 2U, 1, NULL, &wait_guild) == DCC_ERR_INVALID_ARG &&
           wait_for_channel_any(NULL, wait_any_types, 2U, 1, NULL, &wait_channel) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_interaction_any(NULL, wait_any_types, 2U, 1, NULL, &wait_interaction) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_voice_state_any(NULL, wait_any_types, 2U, 1, NULL, &wait_voice_state) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_member(NULL, DCC_EVENT_GUILD_MEMBER_ADD, 1, NULL, &wait_member) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_role(NULL, DCC_EVENT_GUILD_ROLE_CREATE, 1, NULL, &wait_role) == DCC_ERR_INVALID_ARG &&
           wait_for_user(NULL, DCC_EVENT_USER_UPDATE, 1, NULL, &wait_user) == DCC_ERR_INVALID_ARG &&
           wait_for_member_any(NULL, wait_any_types, 2U, 1, NULL, &wait_member) ==
               DCC_ERR_INVALID_ARG &&
           wait_for_role_any(NULL, wait_any_types, 2U, 1, NULL, &wait_role) == DCC_ERR_INVALID_ARG &&
           wait_for_user_any(NULL, wait_any_types, 2U, 1, NULL, &wait_user) == DCC_ERR_INVALID_ARG &&
           wait_data == NULL &&
           wait_message == NULL &&
           wait_guild == NULL &&
           wait_channel == NULL &&
           wait_interaction == NULL &&
           wait_voice_state == NULL &&
           wait_member == NULL &&
           wait_role == NULL &&
           wait_user == NULL;
}
