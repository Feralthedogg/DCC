#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_gateway_data(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_gateway_event_data_t **out_data
) {
    if (out_data == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_owned_kind(
        client,
        type,
        timeout_ms,
        DCC_EVENT_WAIT_GATEWAY_DATA,
        out_snapshot,
        &owned
    );
    *out_data = status == DCC_OK ? (dcc_gateway_event_data_t *)owned : NULL;
    return status;
}

dcc_status_t dcc_client_wait_for_message(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_message_t **out_message
) {
    if (out_message == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_owned_kind(
        client,
        type,
        timeout_ms,
        DCC_EVENT_WAIT_MESSAGE,
        out_snapshot,
        &owned
    );
    *out_message = status == DCC_OK ? (dcc_message_t *)owned : NULL;
    return status;
}

dcc_status_t dcc_client_wait_for_interaction(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshot,
    dcc_interaction_t **out_interaction
) {
    if (out_interaction == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    void *owned = NULL;
    dcc_status_t status = dcc_event_wait_for_owned_kind(
        client,
        type,
        timeout_ms,
        DCC_EVENT_WAIT_INTERACTION,
        out_snapshot,
        &owned
    );
    *out_interaction = status == DCC_OK ? (dcc_interaction_t *)owned : NULL;
    return status;
}
