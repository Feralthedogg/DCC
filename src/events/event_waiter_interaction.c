#include "internal/events/dcc_event_wait_owned_internal.h"
#include "internal/events/dcc_event_waiter_gateway_internal.h"

#include <dcc/events/wait/events.h>

dcc_status_t dcc_client_wait_for_interaction_or_close(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    return dcc_client_wait_for_event_any(
        client,
        dcc_event_wait_interaction_or_close_types,
        dcc_event_wait_interaction_or_close_type_count,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_client_wait_until_interaction(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    dcc_status_t status = dcc_client_wait_for_interaction_or_close(client, timeout_ms, out);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_event_wait_snapshot_is_interaction(out) ? DCC_OK : DCC_ERR_STATE;
}

dcc_status_t dcc_client_wait_for_interaction_or_close_owned(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
) {
    if (out_interaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_any_owned_kind(
        client,
        dcc_event_wait_interaction_or_close_types,
        dcc_event_wait_interaction_or_close_type_count,
        timeout_ms,
        DCC_EVENT_WAIT_INTERACTION_OR_CLOSE,
        out_snapshot,
        &owned
    );
    *out_interaction = status == DCC_OK ? (dcc_interaction_t *)owned : NULL;
    return status;
}

dcc_status_t dcc_client_wait_until_interaction_owned(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
) {
    if (out_interaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_interaction = NULL;
    dcc_status_t status = dcc_client_wait_for_interaction_or_close_owned(
        client,
        timeout_ms,
        out_snapshot,
        out_interaction
    );
    if (status != DCC_OK) {
        *out_interaction = NULL;
        return status;
    }
    if (!dcc_event_wait_snapshot_is_interaction(out_snapshot)) {
        *out_interaction = NULL;
        return DCC_ERR_STATE;
    }
    return DCC_OK;
}
