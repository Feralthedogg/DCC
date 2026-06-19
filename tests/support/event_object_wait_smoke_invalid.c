#include "event_object_wait_smoke_support.h"

#if !defined(_WIN32)

#include <stdio.h>
#include <string.h>

int event_object_wait_smoke_check_invalid_handling(dcc_client_t *client) {
    dcc_message_t *invalid_message = NULL;
    dcc_gateway_event_data_t *invalid_data = NULL;
    dcc_member_t *invalid_member = NULL;
    dcc_role_t *invalid_role = NULL;
    dcc_user_t *invalid_user = NULL;
    dcc_event_type_t wait_any_types[] = {DCC_EVENT_RATE_LIMITED, DCC_EVENT_USER_UPDATE};
    dcc_event_type_t sequence_types[] = {
        DCC_EVENT_MESSAGE_CREATE,
        DCC_EVENT_GUILD_MEMBER_ADD,
        DCC_EVENT_GUILD_ROLE_CREATE,
        DCC_EVENT_USER_UPDATE
    };
    dcc_event_type_t duplicate_set_types[] = {
        DCC_EVENT_MESSAGE_CREATE,
        DCC_EVENT_MESSAGE_CREATE
    };
    dcc_event_type_t invalid_wait_any_types[] = {DCC_EVENT_RATE_LIMITED, DCC_EVENT_MAX};
    dcc_event_snapshot_t invalid_any_snapshot;
    memset(&invalid_any_snapshot, 0, sizeof(invalid_any_snapshot));
    invalid_any_snapshot.size = sizeof(invalid_any_snapshot);
    dcc_event_snapshot_t sequence_snapshots[4];
    memset(sequence_snapshots, 0, sizeof(sequence_snapshots));
    for (size_t i = 0; i < sizeof(sequence_snapshots) / sizeof(sequence_snapshots[0]); ++i) {
        sequence_snapshots[i].size = sizeof(sequence_snapshots[i]);
    }
    size_t sequence_completed = 99U;
    dcc_event_snapshot_t set_snapshots[4];
    memset(set_snapshots, 0, sizeof(set_snapshots));
    for (size_t i = 0; i < sizeof(set_snapshots) / sizeof(set_snapshots[0]); ++i) {
        set_snapshots[i].size = sizeof(set_snapshots[i]);
    }
    size_t set_completed = 99U;
    dcc_event_snapshot_t count_snapshots[4];
    memset(count_snapshots, 0, sizeof(count_snapshots));
    for (size_t i = 0; i < sizeof(count_snapshots) / sizeof(count_snapshots[0]); ++i) {
        count_snapshots[i].size = sizeof(count_snapshots[i]);
    }
    size_t count_completed = 99U;
    dcc_event_snapshot_t quorum_snapshots[4];
    memset(quorum_snapshots, 0, sizeof(quorum_snapshots));
    for (size_t i = 0; i < sizeof(quorum_snapshots) / sizeof(quorum_snapshots[0]); ++i) {
        quorum_snapshots[i].size = sizeof(quorum_snapshots[i]);
    }
    size_t quorum_completed = 99U;
    dcc_event_wait_policy_t zero_policy;
    memset(&zero_policy, 0xA5, sizeof(zero_policy));
    dcc_event_wait_policy_init(&zero_policy);
    dcc_event_wait_policy_t any_policy;
    dcc_event_wait_policy_t sequence_policy;
    dcc_event_wait_policy_t set_policy;
    dcc_event_wait_policy_t count_policy;
    dcc_event_wait_policy_t quorum_policy;
    if (zero_policy.size != sizeof(zero_policy) || zero_policy.mode != 0 ||
        zero_policy.types != NULL || zero_policy.type_count != 0 ||
        zero_policy.target_count != 0 || zero_policy.timeout_ms != 0 ||
        dcc_event_wait_policy_init_any(NULL, wait_any_types, 2U, 1) != DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_any(&any_policy, NULL, 2U, 1) != DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_any(&any_policy, wait_any_types, 0U, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_any(&any_policy, wait_any_types, 2U, 1) != DCC_OK ||
        any_policy.size != sizeof(any_policy) ||
        any_policy.mode != DCC_EVENT_WAIT_POLICY_ANY ||
        any_policy.types != wait_any_types ||
        any_policy.type_count != 2U ||
        any_policy.target_count != 1U ||
        any_policy.timeout_ms != 1 ||
        dcc_event_wait_policy_init_sequence(&sequence_policy, sequence_types, 4U, 1) != DCC_OK ||
        sequence_policy.mode != DCC_EVENT_WAIT_POLICY_SEQUENCE ||
        sequence_policy.target_count != 4U ||
        dcc_event_wait_policy_init_set(&set_policy, sequence_types, 4U, 1) != DCC_OK ||
        set_policy.mode != DCC_EVENT_WAIT_POLICY_SET ||
        set_policy.target_count != 4U ||
        dcc_event_wait_policy_init_count(&count_policy, sequence_types, 4U, 0U, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_count(&count_policy, sequence_types, 4U, 2U, 1) != DCC_OK ||
        count_policy.mode != DCC_EVENT_WAIT_POLICY_COUNT ||
        count_policy.target_count != 2U ||
        dcc_event_wait_policy_init_quorum(&quorum_policy, sequence_types, 4U, 0U, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_quorum(&quorum_policy, sequence_types, 4U, 5U, 1) !=
            DCC_ERR_INVALID_ARG ||
        dcc_event_wait_policy_init_quorum(&quorum_policy, sequence_types, 4U, 3U, 1) != DCC_OK ||
        quorum_policy.mode != DCC_EVENT_WAIT_POLICY_QUORUM ||
        quorum_policy.target_count != 3U) {
        fprintf(stderr, "event wait policy init helper handling failed\n");
        return 1;
    }
    if (dcc_client_wait_for_message(NULL, DCC_EVENT_MESSAGE_CREATE, 1, NULL, &invalid_message) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_message(client, DCC_EVENT_MESSAGE_CREATE, 1, NULL, &invalid_message) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_member(NULL, DCC_EVENT_GUILD_MEMBER_ADD, 1, NULL, &invalid_member) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_member(client, DCC_EVENT_GUILD_MEMBER_ADD, 1, NULL, &invalid_member) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_role(NULL, DCC_EVENT_GUILD_ROLE_CREATE, 1, NULL, &invalid_role) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_role(client, DCC_EVENT_GUILD_ROLE_CREATE, 1, NULL, &invalid_role) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_user(NULL, DCC_EVENT_USER_UPDATE, 1, NULL, &invalid_user) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_user(client, DCC_EVENT_USER_UPDATE, 1, NULL, &invalid_user) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_any(NULL, wait_any_types, 2U, 1, &invalid_any_snapshot) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_any(client, NULL, 2U, 1, &invalid_any_snapshot) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_any(client, wait_any_types, 0U, 1, &invalid_any_snapshot) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_any(client, invalid_wait_any_types, 2U, 1, &invalid_any_snapshot) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_any(client, wait_any_types, 2U, 1, &invalid_any_snapshot) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_gateway_data_any(NULL, wait_any_types, 2U, 1, NULL, &invalid_data) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_gateway_data_any(client, wait_any_types, 2U, 1, NULL, &invalid_data) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_message_any(NULL, wait_any_types, 2U, 1, NULL, &invalid_message) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_message_any(client, wait_any_types, 2U, 1, NULL, &invalid_message) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_user_any(NULL, wait_any_types, 2U, 1, NULL, &invalid_user) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_user_any(client, wait_any_types, 2U, 1, NULL, &invalid_user) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_sequence(
            NULL,
            sequence_types,
            4U,
            1,
            sequence_snapshots,
            4U,
            &sequence_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_sequence(
            client,
            NULL,
            4U,
            1,
            sequence_snapshots,
            4U,
            &sequence_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_sequence(
            client,
            invalid_wait_any_types,
            2U,
            1,
            sequence_snapshots,
            4U,
            &sequence_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_sequence(
            client,
            sequence_types,
            4U,
            1,
            sequence_snapshots,
            3U,
            &sequence_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_sequence(
            client,
            sequence_types,
            4U,
            1,
            sequence_snapshots,
            4U,
            &sequence_completed
        ) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_set(
            NULL,
            sequence_types,
            4U,
            1,
            set_snapshots,
            4U,
            &set_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_set(
            client,
            NULL,
            4U,
            1,
            set_snapshots,
            4U,
            &set_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_set(
            client,
            invalid_wait_any_types,
            2U,
            1,
            set_snapshots,
            4U,
            &set_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_set(
            client,
            duplicate_set_types,
            2U,
            1,
            set_snapshots,
            4U,
            &set_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_set(
            client,
            sequence_types,
            4U,
            1,
            set_snapshots,
            3U,
            &set_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_set(
            client,
            sequence_types,
            4U,
            1,
            set_snapshots,
            4U,
            &set_completed
        ) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_count(
            NULL,
            sequence_types,
            4U,
            2U,
            1,
            count_snapshots,
            4U,
            &count_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_count(
            client,
            NULL,
            4U,
            2U,
            1,
            count_snapshots,
            4U,
            &count_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_count(
            client,
            sequence_types,
            4U,
            0U,
            1,
            count_snapshots,
            4U,
            &count_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_count(
            client,
            invalid_wait_any_types,
            2U,
            2U,
            1,
            count_snapshots,
            4U,
            &count_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_count(
            client,
            sequence_types,
            4U,
            4U,
            1,
            count_snapshots,
            3U,
            &count_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_count(
            client,
            sequence_types,
            4U,
            2U,
            1,
            count_snapshots,
            4U,
            &count_completed
        ) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_quorum(
            NULL,
            sequence_types,
            4U,
            3U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            NULL,
            4U,
            3U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            sequence_types,
            4U,
            0U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            sequence_types,
            4U,
            5U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            invalid_wait_any_types,
            2U,
            2U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            duplicate_set_types,
            2U,
            2U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            sequence_types,
            4U,
            4U,
            1,
            quorum_snapshots,
            3U,
            &quorum_completed
        ) != DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_quorum(
            client,
            sequence_types,
            4U,
            3U,
            1,
            quorum_snapshots,
            4U,
            &quorum_completed
        ) !=
            DCC_ERR_STATE ||
        dcc_client_wait_for_event_policy(NULL, &sequence_policy, sequence_snapshots, 4U, &sequence_completed) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_policy(client, &sequence_policy, sequence_snapshots, 3U, &sequence_completed) !=
            DCC_ERR_INVALID_ARG ||
        dcc_client_wait_for_event_policy(client, &sequence_policy, sequence_snapshots, 4U, &sequence_completed) !=
            DCC_ERR_STATE) {
        fprintf(stderr, "object wait invalid handling failed\n");
        return 1;
    }

    return 0;
}

#endif
