#include "internal/events/dcc_event_wait_policy_internal.h"

dcc_status_t dcc_event_wait_policy_init_count(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
) {
    if (target_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    return dcc_event_wait_policy_init_mode(
        policy,
        DCC_EVENT_WAIT_POLICY_COUNT,
        types,
        type_count,
        target_count,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_quorum(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
) {
    if (target_count == 0 || target_count > type_count) {
        return DCC_ERR_INVALID_ARG;
    }

    return dcc_event_wait_policy_init_mode(
        policy,
        DCC_EVENT_WAIT_POLICY_QUORUM,
        types,
        type_count,
        target_count,
        timeout_ms
    );
}
