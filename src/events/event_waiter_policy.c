#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_event_policy(
    dcc_client_t *client,
    const dcc_event_wait_policy_t *policy,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
) {
    if (out_completed != NULL) {
        *out_completed = 0;
    }
    if (client == NULL || policy == NULL || policy->size < sizeof(*policy) ||
        policy->types == NULL || policy->type_count == 0 || out_snapshots == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    switch (policy->mode) {
        case DCC_EVENT_WAIT_POLICY_ANY: {
            if (out_snapshot_count < 1U) {
                return DCC_ERR_INVALID_ARG;
            }
            dcc_status_t status = dcc_client_wait_for_event_any(
                client,
                policy->types,
                policy->type_count,
                policy->timeout_ms,
                &out_snapshots[0]
            );
            if (status == DCC_OK && out_completed != NULL) {
                *out_completed = 1U;
            }
            return status;
        }
        case DCC_EVENT_WAIT_POLICY_SEQUENCE:
            return dcc_client_wait_for_event_sequence(
                client,
                policy->types,
                policy->type_count,
                policy->timeout_ms,
                out_snapshots,
                out_snapshot_count,
                out_completed
            );
        case DCC_EVENT_WAIT_POLICY_SET:
            return dcc_client_wait_for_event_set(
                client,
                policy->types,
                policy->type_count,
                policy->timeout_ms,
                out_snapshots,
                out_snapshot_count,
                out_completed
            );
        case DCC_EVENT_WAIT_POLICY_COUNT:
            return dcc_client_wait_for_event_count(
                client,
                policy->types,
                policy->type_count,
                policy->target_count,
                policy->timeout_ms,
                out_snapshots,
                out_snapshot_count,
                out_completed
            );
        case DCC_EVENT_WAIT_POLICY_QUORUM:
            return dcc_client_wait_for_event_quorum(
                client,
                policy->types,
                policy->type_count,
                policy->target_count,
                policy->timeout_ms,
                out_snapshots,
                out_snapshot_count,
                out_completed
            );
        default:
            return DCC_ERR_INVALID_ARG;
    }
}
