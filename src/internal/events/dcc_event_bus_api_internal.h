#ifndef DCC_EVENT_BUS_API_INTERNAL_H
#define DCC_EVENT_BUS_API_INTERNAL_H

#include "internal/events/dcc_event_bus_state_internal.h"
#include "internal/events/dcc_event_object_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_event_bus_init(dcc_event_bus_t *bus);
void dcc_event_bus_deinit(dcc_event_bus_t *bus);
dcc_status_t dcc_event_bus_add(
    dcc_event_bus_t *bus,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
dcc_status_t dcc_event_bus_remove(dcc_event_bus_t *bus, dcc_event_type_t type, dcc_listener_id_t id);
dcc_status_t dcc_event_bus_dispatch(dcc_event_bus_t *bus, dcc_client_t *client, const dcc_event_t *event);
void dcc_client_interaction_listeners_deinit(dcc_client_t *client);

#ifdef __cplusplus
}
#endif

#endif
