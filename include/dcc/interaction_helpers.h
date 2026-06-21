#ifndef DCC_INTERACTION_HELPERS_H
#define DCC_INTERACTION_HELPERS_H

#include <dcc/message.h>
#include <dcc/modal.h>
#include <dcc/objects/interactions.h>
#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_interaction_reply_message(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_reply_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_reply_ephemeral_text(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *content,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_reply_embed(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    uint32_t color,
    uint8_t ephemeral,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_reply_error(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_reply_success(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const char *title,
    const char *description,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_defer(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_defer_ephemeral(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_update_message(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_message_builder_t *message,
    dcc_rest_cb cb,
    void *user_data
);
DCC_API dcc_status_t dcc_interaction_show_modal(
    dcc_client_t *client,
    const dcc_interaction_t *interaction,
    const dcc_modal_builder_t *modal,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
