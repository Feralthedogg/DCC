#ifndef DCC_EVENTS_LISTENERS_H
#define DCC_EVENTS_LISTENERS_H

#include <dcc/events/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*dcc_event_cb)(dcc_client_t *client, const dcc_event_t *event, void *user_data);

DCC_API dcc_status_t dcc_client_on(
    dcc_client_t *client,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);

DCC_API dcc_status_t dcc_client_off(
    dcc_client_t *client,
    dcc_event_type_t type,
    dcc_listener_id_t id
);

DCC_API dcc_status_t dcc_client_on_interaction_name(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_slash_command(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_autocomplete(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_user_context_menu(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_message_context_menu(
    dcc_client_t *client,
    const char *name,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_interaction_custom_id(
    dcc_client_t *client,
    dcc_event_type_t type,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_button(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_select(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_on_form_submit(
    dcc_client_t *client,
    const char *custom_id,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_id
);
DCC_API dcc_status_t dcc_client_off_interaction(
    dcc_client_t *client,
    dcc_event_type_t type,
    dcc_listener_id_t id
);

#ifdef __cplusplus
}
#endif

#endif
