#ifndef DCC_EVENT_WAIT_POLICY_INTERNAL_H
#define DCC_EVENT_WAIT_POLICY_INTERNAL_H

#include <dcc/events/wait/policy.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_event_wait_policy_init_mode(
    dcc_event_wait_policy_t *policy,
    dcc_event_wait_policy_mode_t mode,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms
);

#ifdef __cplusplus
}
#endif

#endif
