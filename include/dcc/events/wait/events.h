#ifndef DCC_EVENTS_WAIT_EVENTS_H
#define DCC_EVENTS_WAIT_EVENTS_H

#include <dcc/events/wait/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_client_wait_for_event(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_for_event_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_for_event_sequence(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

DCC_API dcc_status_t dcc_client_wait_for_event_set(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

DCC_API dcc_status_t dcc_client_wait_for_event_count(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

DCC_API dcc_status_t dcc_client_wait_for_event_quorum(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

DCC_API dcc_status_t dcc_client_wait_for_event_policy(
    dcc_client_t *client,
    const dcc_event_wait_policy_t *policy,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

DCC_API dcc_status_t dcc_client_wait_for_gateway_ready_or_resumed(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_for_gateway_ready_resumed_or_close(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_until_gateway_ready(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_for_interaction_or_close(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_until_interaction(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
);

DCC_API dcc_status_t dcc_client_wait_for_interaction_or_close_owned(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
);

DCC_API dcc_status_t dcc_client_wait_until_interaction_owned(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
);

DCC_API dcc_status_t dcc_client_wait_for_voice_session_descriptor(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
);

#ifdef __cplusplus
}
#endif

#endif
