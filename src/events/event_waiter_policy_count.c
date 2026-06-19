#include "internal/events/dcc_event_waiter_policy_internal.h"

#include <string.h>

dcc_status_t dcc_client_wait_for_event_count(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
) {
    if (out_completed != NULL) {
        *out_completed = 0;
    }
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_waiter_node_t waiter;
    memset(&waiter, 0, sizeof(waiter));
    dcc_status_t status = dcc_event_waiter_set_event_count(
        &waiter,
        types,
        type_count,
        target_count,
        out_snapshots,
        out_snapshot_count
    );
    if (status != DCC_OK) {
        return status;
    }
    return dcc_event_waiter_policy_wait(
        client,
        &waiter,
        timeout_ms,
        DCC_EVENT_WAITER_POLICY_COUNT_MATCHES,
        out_completed
    );
}

dcc_status_t dcc_client_wait_for_event_quorum(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    size_t target_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
) {
    if (out_completed != NULL) {
        *out_completed = 0;
    }
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_event_waiter_node_t waiter;
    memset(&waiter, 0, sizeof(waiter));
    dcc_status_t status = dcc_event_waiter_set_event_quorum(
        &waiter,
        types,
        type_count,
        target_count,
        out_snapshots,
        out_snapshot_count
    );
    if (status != DCC_OK) {
        return status;
    }
    return dcc_event_waiter_policy_wait(
        client,
        &waiter,
        timeout_ms,
        DCC_EVENT_WAITER_POLICY_COUNT_MATCHES,
        out_completed
    );
}
