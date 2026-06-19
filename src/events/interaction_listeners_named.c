#include "internal/events/dcc_interaction_listener_internal.h"

dcc_status_t dcc_client_on_slash_command(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_name(client, DCC_EVENT_SLASH_COMMAND, name, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_autocomplete(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_name(client, DCC_EVENT_AUTOCOMPLETE, name, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_user_context_menu(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_name(client, DCC_EVENT_USER_CONTEXT_MENU, name, cb, user_data, out_id);
}

dcc_status_t dcc_client_on_message_context_menu(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
) {
    return dcc_client_on_interaction_name(client, DCC_EVENT_MESSAGE_CONTEXT_MENU, name, cb, user_data, out_id);
}
