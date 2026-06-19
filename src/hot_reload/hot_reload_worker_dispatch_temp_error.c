#include "internal/hot_reload/dcc_hot_reload_internal.h"

static uint8_t dcc_worker_event_can_receive_temp_error(const dcc_event_t *event) {
    if (event == NULL) {
        return 0U;
    }
    switch (event->type) {
        case DCC_EVENT_SLASH_COMMAND:
        case DCC_EVENT_BUTTON_CLICK:
        case DCC_EVENT_SELECT_CLICK:
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
        case DCC_EVENT_USER_CONTEXT_MENU:
        case DCC_EVENT_FORM_SUBMIT:
            return 1U;
        default:
            return 0U;
    }
}

dcc_status_t dcc_hot_reload_worker_dispatch_send_temp_interaction_error(
    dcc_hot_reload_t *hot_reload,
    const dcc_event_t *event
) {
    if (hot_reload == NULL || !dcc_worker_event_can_receive_temp_error(event)) {
        return DCC_ERR_STATE;
    }
    const dcc_interaction_t *interaction = dcc_event_interaction(event);
    if (interaction == NULL || interaction->id == 0U ||
        interaction->token == NULL || interaction->token[0] == '\0') {
        return DCC_ERR_STATE;
    }

    static const char body[] =
        "{\"type\":4,\"data\":{\"content\":\"Bot worker is reloading. Please try again.\",\"flags\":64}}";
    return dcc_rest_interaction_response_create_from_interaction(
        hot_reload->client,
        interaction,
        body,
        NULL,
        NULL
    );
}
