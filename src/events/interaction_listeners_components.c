#include "internal/events/dcc_interaction_listener_internal.h"

dcc_status_t dcc_client_on_button(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_custom_id(client, DCC_EVENT_BUTTON_CLICK, custom_id, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_select(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_custom_id(client, DCC_EVENT_SELECT_CLICK, custom_id, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_form_submit(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_custom_id(client, DCC_EVENT_FORM_SUBMIT, custom_id, cb, user_data, out_id);
}
