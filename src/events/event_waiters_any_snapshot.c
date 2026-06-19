#include "internal/events/dcc_events_internal.h"

dcc_status_t dcc_client_wait_for_event_any(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out
) {
    if (out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_client_wait_for_event_any_owned(
        client,
        types,
        type_count,
        timeout_ms,
        DCC_EVENT_WAIT_SNAPSHOT,
        out,
        NULL
    );
}
