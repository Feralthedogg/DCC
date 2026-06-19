#ifndef DCC_EVENT_LISTENER_STATE_INTERNAL_H
#define DCC_EVENT_LISTENER_STATE_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

typedef struct dcc_event_listener {
    dcc_listener_id_t id;
    dcc_event_cb cb;
    void *user_data;
} dcc_event_listener_t;

typedef struct dcc_event_listener_list {
    dcc_event_listener_t *items;
    size_t len;
    size_t cap;
    dcc_listener_id_t next_id;
} dcc_event_listener_list_t;

#endif
