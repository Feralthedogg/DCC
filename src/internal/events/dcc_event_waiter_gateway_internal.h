#ifndef DCC_EVENT_WAITER_GATEWAY_INTERNAL_H
#define DCC_EVENT_WAITER_GATEWAY_INTERNAL_H

#include <dcc/events/wait/events.h>

#include <stddef.h>

extern const dcc_event_type_t dcc_event_wait_gateway_ready_or_resumed_types[];
extern const size_t dcc_event_wait_gateway_ready_or_resumed_type_count;

extern const dcc_event_type_t dcc_event_wait_gateway_ready_resumed_or_close_types[];
extern const size_t dcc_event_wait_gateway_ready_resumed_or_close_type_count;

extern const dcc_event_type_t dcc_event_wait_interaction_or_close_types[];
extern const size_t dcc_event_wait_interaction_or_close_type_count;

extern const dcc_event_type_t dcc_event_wait_voice_session_descriptor_types[];
extern const size_t dcc_event_wait_voice_session_descriptor_type_count;

int dcc_event_wait_snapshot_is_interaction(const dcc_event_snapshot_t *snapshot);

#endif
