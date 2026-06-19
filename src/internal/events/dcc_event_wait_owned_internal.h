#ifndef DCC_EVENT_WAIT_OWNED_INTERNAL_H
#define DCC_EVENT_WAIT_OWNED_INTERNAL_H

#include "internal/events/dcc_event_state_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_client_wait_for_event_owned(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
);
dcc_status_t dcc_client_wait_for_event_any_owned(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
);
dcc_status_t dcc_event_wait_for_owned_kind(
    dcc_client_t *client,
    dcc_event_type_t type,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
);
dcc_status_t dcc_event_wait_for_any_owned_kind(
    dcc_client_t *client,
    const dcc_event_type_t *types,
    size_t type_count,
    uint32_t timeout_ms,
    dcc_event_wait_kind_t kind,
    dcc_event_snapshot_t *out_snapshot,
    void **out_owned
);

#ifdef __cplusplus
}
#endif

#endif
