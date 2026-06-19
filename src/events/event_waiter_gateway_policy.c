#include "internal/events/dcc_event_waiter_gateway_internal.h"

#include <dcc/events/wait/policy.h>

dcc_status_t dcc_event_wait_policy_init_gateway_ready_or_resumed(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_any(
        policy,
        dcc_event_wait_gateway_ready_or_resumed_types,
        dcc_event_wait_gateway_ready_or_resumed_type_count,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_gateway_ready_resumed_or_close(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_any(
        policy,
        dcc_event_wait_gateway_ready_resumed_or_close_types,
        dcc_event_wait_gateway_ready_resumed_or_close_type_count,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_interaction_or_close(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_any(
        policy,
        dcc_event_wait_interaction_or_close_types,
        dcc_event_wait_interaction_or_close_type_count,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_voice_session_descriptor(
    dcc_event_wait_policy_t *policy,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_set(
        policy,
        dcc_event_wait_voice_session_descriptor_types,
        dcc_event_wait_voice_session_descriptor_type_count,
        timeout_ms
    );
}
