#ifndef DCC_INTERACTION_LISTENER_STATE_INTERNAL_H
#define DCC_INTERACTION_LISTENER_STATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stdatomic.h>
#include <stdint.h>

typedef struct dcc_interaction_listener_ctx {
    dcc_event_type_t type;
    dcc_listener_id_t listener_id;
    char *key;
    dcc_event_cb cb;
    void *user_data;
    uint8_t match_custom_id;
    atomic_bool active;
    struct dcc_interaction_listener_ctx *next;
} dcc_interaction_listener_ctx_t;

#endif
