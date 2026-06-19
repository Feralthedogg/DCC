#include "internal/events/dcc_interaction_listener_internal.h"

#include <string.h>

uint8_t dcc_interaction_name_event_type(dcc_event_type_t type) {
    return type == DCC_EVENT_SLASH_COMMAND ||
        type == DCC_EVENT_AUTOCOMPLETE ||
        type == DCC_EVENT_USER_CONTEXT_MENU ||
        type == DCC_EVENT_MESSAGE_CONTEXT_MENU;
}

uint8_t dcc_interaction_custom_id_event_type(dcc_event_type_t type) {
    return type == DCC_EVENT_BUTTON_CLICK ||
        type == DCC_EVENT_SELECT_CLICK ||
        type == DCC_EVENT_FORM_SUBMIT;
}

void dcc_interaction_listener_trampoline(
    dcc_client_t *client,
    const dcc_event_t *event,
    void *user_data
) {
    dcc_interaction_listener_ctx_t *ctx = (dcc_interaction_listener_ctx_t *)user_data;
    if (ctx == NULL ||
        !atomic_load_explicit(&ctx->active, memory_order_acquire) ||
        ctx->cb == NULL ||
        event == NULL) {
        return;
    }

    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL) {
        return;
    }
    const char *value = ctx->match_custom_id ? interaction->custom_id : interaction->name;
    if (value == NULL || ctx->key == NULL || strcmp(value, ctx->key) != 0) {
        return;
    }

    ctx->cb(client, event, ctx->user_data);
}
