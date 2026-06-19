#ifndef DCC_VOICE_CONNECT_WAIT_INTERNAL_H
#define DCC_VOICE_CONNECT_WAIT_INTERNAL_H

#include "internal/events/dcc_events_internal.h"
#include "internal/voice/dcc_voice_internal.h"

void dcc_voice_connect_wait_result_init(dcc_voice_connect_wait_result_t *out);
dcc_status_t dcc_voice_connect_wait_options_normalize(
    const dcc_voice_connect_wait_options_t *options,
    dcc_voice_connect_wait_options_t *out
);
dcc_status_t dcc_voice_connect_wait_descriptor(
    dcc_voice_client_t *voice_client,
    dcc_client_t *client,
    const dcc_voice_connect_wait_options_t *options,
    dcc_event_snapshot_t snapshots[2],
    size_t *out_completed,
    dcc_status_t *out_connect_status
);

#endif
