#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_member_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_member_t **out_member
) {
    if (out_member == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_any_owned_kind(
        client,
        types,
        type_count,
        timeout_ms,
        DCC_EVENT_WAIT_MEMBER,
        out_snapshot,
        &owned
    );
    *out_member = status == DCC_OK ? (dcc_member_t *)owned : NULL;
    return status;
}
