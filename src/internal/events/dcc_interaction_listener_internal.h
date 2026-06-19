#ifndef DCC_INTERACTION_LISTENER_INTERNAL_H
#define DCC_INTERACTION_LISTENER_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"

void dcc_interaction_listeners_lock(dcc_client_t *client);
void dcc_interaction_listeners_unlock(dcc_client_t *client);

uint8_t dcc_interaction_name_event_type(dcc_event_type_t type);
uint8_t dcc_interaction_custom_id_event_type(dcc_event_type_t type);

void dcc_interaction_listener_trampoline(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
);

void dcc_interaction_listener_free(dcc_interaction_listener_ctx_t *ctx);

dcc_status_t dcc_client_on_interaction_filtered(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *key,
    uint8_t match_custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);

#endif
