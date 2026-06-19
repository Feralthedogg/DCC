#include "internal/events/dcc_interaction_listener_internal.h"

dcc_status_t dcc_client_on(
    dcc_client_t *client,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_event_bus_add(&client->events, type, cb, user_data, out_id);
}

dcc_status_t dcc_client_off(dcc_client_t *client, dcc_event_type_t type, dcc_listener_id_t id) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_event_bus_remove(&client->events, type, id);
}

dcc_status_t dcc_client_on_interaction_name(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_filtered(client, type, name, 0U, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_interaction_custom_id(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_filtered(client, type, custom_id, 1U, cb, user_data, out_id);
}
