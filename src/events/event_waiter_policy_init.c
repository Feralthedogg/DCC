#include "internal/events/dcc_event_wait_policy_internal.h"

#include <string.h>

void dcc_event_wait_policy_init(dcc_event_wait_policy_t *policy) {
    if (policy == NULL) {
        return;
    }

    memset(policy, 0, sizeof(*policy));
    policy->size = sizeof(*policy);
}

dcc_status_t dcc_event_wait_policy_init_mode(
    dcc_event_wait_policy_t *policy,
    dcc_event_wait_policy_mode_t mode,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
) {
    if (policy == NULL || types == NULL || type_count == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_wait_policy_init(policy);
    policy->mode = mode;
    policy->types = types;
    policy->type_count = type_count;
    policy->target_count = target_count;
    policy->timeout_ms = timeout_ms;
    return DCC_OK;
}

dcc_status_t dcc_event_wait_policy_init_any(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_mode(
        policy,
        DCC_EVENT_WAIT_POLICY_ANY,
        types,
        type_count,
        1U,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_sequence(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_mode(
        policy,
        DCC_EVENT_WAIT_POLICY_SEQUENCE,
        types,
        type_count,
        type_count,
        timeout_ms
    );
}

dcc_status_t dcc_event_wait_policy_init_set(
    dcc_event_wait_policy_t *policy,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms
) {
    return dcc_event_wait_policy_init_mode(
        policy,
        DCC_EVENT_WAIT_POLICY_SET,
        types,
        type_count,
        type_count,
        timeout_ms
    );
}
