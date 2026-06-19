#ifndef DCC_EVENTS_WAIT_OWNED_OBJECTS_ANY_H
#define DCC_EVENTS_WAIT_OWNED_OBJECTS_ANY_H

#include <dcc/events/wait/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_client_wait_for_gateway_data_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_gateway_event_data_t **out_data
);

DCC_API dcc_status_t dcc_client_wait_for_message_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_message_t **out_message
);

DCC_API dcc_status_t dcc_client_wait_for_guild_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_guild_t **out_guild
);

DCC_API dcc_status_t dcc_client_wait_for_channel_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_channel_t **out_channel
);

DCC_API dcc_status_t dcc_client_wait_for_interaction_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
);

DCC_API dcc_status_t dcc_client_wait_for_voice_state_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_voice_state_t **out_voice_state
);

DCC_API dcc_status_t dcc_client_wait_for_member_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_member_t **out_member
);

DCC_API dcc_status_t dcc_client_wait_for_role_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_role_t **out_role
);

DCC_API dcc_status_t dcc_client_wait_for_user_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_user_t **out_user
);

#ifdef __cplusplus
}
#endif

#endif
