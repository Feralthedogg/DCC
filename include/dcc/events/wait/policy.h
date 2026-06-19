#ifndef DCC_EVENTS_WAIT_POLICY_H
#define DCC_EVENTS_WAIT_POLICY_H

#include <dcc/events/wait/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API void dcc_event_wait_policy_init(dcc_event_wait_policy_t *policy);

DCC_API dcc_status_t dcc_event_wait_policy_init_any(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_sequence(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_set(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_count(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_quorum(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_gateway_ready_or_resumed(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_gateway_ready_resumed_or_close(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_interaction_or_close(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
);

DCC_API dcc_status_t dcc_event_wait_policy_init_voice_session_descriptor(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
);

#ifdef __cplusplus
}
#endif

#endif
