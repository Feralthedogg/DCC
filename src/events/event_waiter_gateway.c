#include "internal/events/dcc_event_waiter_gateway_internal.h"

#include <dcc/events/wait/events.h>

dcc_status_t dcc_client_wait_for_gateway_ready_or_resumed(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    return dcc_client_wait_for_event_any(
        client,
        dcc_event_wait_gateway_ready_or_resumed_types,
        dcc_event_wait_gateway_ready_or_resumed_type_count,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_client_wait_for_gateway_ready_resumed_or_close(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    return dcc_client_wait_for_event_any(
        client,
        dcc_event_wait_gateway_ready_resumed_or_close_types,
        dcc_event_wait_gateway_ready_resumed_or_close_type_count,
        timeout_ms,
        out
    );
}

dcc_status_t dcc_client_wait_until_gateway_ready(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    dcc_status_t status = dcc_client_wait_for_gateway_ready_resumed_or_close(client, timeout_ms, out);
    if (status != DCC_OK) {
        return status;
    }
    return out != NULL && (out->type == DCC_EVENT_READY || out->type == DCC_EVENT_RESUMED)
        ? DCC_OK
        : DCC_ERR_STATE;
}
