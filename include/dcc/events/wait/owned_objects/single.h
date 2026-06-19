#ifndef DCC_EVENTS_WAIT_OWNED_OBJECTS_SINGLE_H
#define DCC_EVENTS_WAIT_OWNED_OBJECTS_SINGLE_H

#include <dcc/events/wait/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_client_wait_for_gateway_data(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_gateway_event_data_t **out_data
);

DCC_API dcc_status_t dcc_client_wait_for_message(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_message_t **out_message
);

DCC_API dcc_status_t dcc_client_wait_for_guild(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_guild_t **out_guild
);

DCC_API dcc_status_t dcc_client_wait_for_channel(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_channel_t **out_channel
);

DCC_API dcc_status_t dcc_client_wait_for_interaction(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
);

DCC_API dcc_status_t dcc_client_wait_for_voice_state(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_voice_state_t **out_voice_state
);

DCC_API dcc_status_t dcc_client_wait_for_member(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_member_t **out_member
);

DCC_API dcc_status_t dcc_client_wait_for_role(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_role_t **out_role
);

DCC_API dcc_status_t dcc_client_wait_for_user(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_user_t **out_user
);

#ifdef __cplusplus
}
#endif

#endif
