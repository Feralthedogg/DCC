#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_role(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_role_t **out_role
) {
    if (out_role == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_owned_kind(
        client,
        type,
        timeout_ms,
        DCC_EVENT_WAIT_ROLE,
        out_snapshot,
        &owned
    );
    *out_role = status == DCC_OK ? (dcc_role_t *)owned : NULL;
    return status;
}
