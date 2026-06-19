#include "internal/events/dcc_event_state_internal.h"

const dcc_interaction_t *dcc_event_interaction(const dcc_event_t *event) {
    if (event == NULL) {
        return NULL;
    }
    switch (event->type) {
        case DCC_EVENT_INTERACTION_CREATE:
        case DCC_EVENT_SLASH_COMMAND:
        case DCC_EVENT_BUTTON_CLICK:
        case DCC_EVENT_AUTOCOMPLETE:
        case DCC_EVENT_SELECT_CLICK:
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
        case DCC_EVENT_USER_CONTEXT_MENU:
        case DCC_EVENT_FORM_SUBMIT:
            return &event->data.interaction;
        default:
            return NULL;
    }
}
