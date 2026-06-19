#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_user(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_user_t **out_user
) {
    if (out_user == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_owned_kind(
        client,
        type,
        timeout_ms,
        DCC_EVENT_WAIT_USER,
        out_snapshot,
        &owned
    );
    *out_user = status == DCC_OK ? (dcc_user_t *)owned : NULL;
    return status;
}
