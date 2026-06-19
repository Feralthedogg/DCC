#ifndef DCC_PACKAGE_CONSUMER_SYMBOLS_WAIT_H
#define DCC_PACKAGE_CONSUMER_SYMBOLS_WAIT_H

#include <dcc/dcc.h>

typedef dcc_status_t (*dcc_package_wait_for_event_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *
    );
typedef dcc_status_t (*dcc_package_wait_for_event_sequence_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        size_t,
        size_t *
    );
typedef dcc_status_t (*dcc_package_wait_for_event_count_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        size_t,
        size_t *
    );
typedef void (*dcc_package_event_wait_policy_init_fn)(dcc_event_wait_policy_t *);
typedef dcc_status_t (*dcc_package_event_wait_policy_init_any_fn)(
        dcc_event_wait_policy_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t
    );
typedef dcc_status_t (*dcc_package_event_wait_policy_init_count_fn)(
        dcc_event_wait_policy_t *,
        const dcc_event_type_t *,
        size_t,
        size_t,
        uint32_t
    );
typedef dcc_status_t (*dcc_package_wait_for_gateway_data_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_gateway_event_data_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_message_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_message_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_guild_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_guild_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_channel_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_channel_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_interaction_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_interaction_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_voice_state_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_voice_state_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_member_fn)(
        dcc_client_t *,
        dcc_event_type_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_member_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_role_fn)(
        dcc_client_t *,
        dcc_event_type_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_role_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_user_fn)(
        dcc_client_t *,
        dcc_event_type_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_user_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_member_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_member_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_role_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_role_t **
    );
typedef dcc_status_t (*dcc_package_wait_for_user_any_fn)(
        dcc_client_t *,
        const dcc_event_type_t *,
        size_t,
        uint32_t,
        dcc_event_snapshot_t *,
        dcc_user_t **
    );

#endif
