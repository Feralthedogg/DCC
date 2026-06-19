#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_event_wait_for_owned_kind(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
) {
    if (out_owned == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_owned = NULL;

    void *owned = NULL;
    dcc_status_t status = dcc_client_wait_for_event_owned(
        client,
        type,
        timeout_ms,
        kind,
        out_snapshot,
        &owned
    );
    if (status == DCC_OK) {
        *out_owned = owned;
    }
    return status;
}

dcc_status_t dcc_event_wait_for_any_owned_kind(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
) {
    if (out_owned == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_owned = NULL;

    void *owned = NULL;
    dcc_status_t status = dcc_client_wait_for_event_any_owned(
        client,
        types,
        type_count,
        timeout_ms,
        kind,
        out_snapshot,
        &owned
    );
    if (status == DCC_OK) {
        *out_owned = owned;
    }
    return status;
}
