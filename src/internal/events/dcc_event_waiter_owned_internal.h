#ifndef DCC_EVENT_WAITER_OWNED_INTERNAL_H
#define DCC_EVENT_WAITER_OWNED_INTERNAL_H

#include "internal/events/dcc_event_state_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_event_waiter_free_owned(dcc_event_wait_kind_t kind, void *owned);
void *dcc_event_waiter_clone_owned(
    dcc_event_wait_kind_t kind,
    const dcc_event_t *event,
    dcc_status_t *out_status
);
void *dcc_event_waiter_clone_gateway_data(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_message(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_guild(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_channel(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_interaction(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_voice_state(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_member(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_role(const dcc_event_t *event, dcc_status_t *out_status);
void *dcc_event_waiter_clone_user(const dcc_event_t *event, dcc_status_t *out_status);

#ifdef __cplusplus
}
#endif

#endif
