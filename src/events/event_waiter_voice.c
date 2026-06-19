#include "internal/events/dcc_event_waiter_gateway_internal.h"

#include <dcc/events/wait/events.h>

dcc_status_t dcc_client_wait_for_voice_session_descriptor(
    dcc_client_t *client,
    uint32_t timeout_ms,
    dcc_event_snapshot_t *out_snapshots,
    size_t out_snapshot_count,
    size_t *out_completed
) {
    return dcc_client_wait_for_event_set(
        client,
        dcc_event_wait_voice_session_descriptor_types,
        dcc_event_wait_voice_session_descriptor_type_count,
        timeout_ms,
        out_snapshots,
        out_snapshot_count,
        out_completed
    );
}
