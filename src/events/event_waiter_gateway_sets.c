#include "internal/events/dcc_event_waiter_gateway_internal.h"

const dcc_event_type_t dcc_event_wait_gateway_ready_or_resumed_types[] = {
    DCC_EVENT_READY,
    DCC_EVENT_RESUMED
};

const size_t dcc_event_wait_gateway_ready_or_resumed_type_count =
    sizeof(dcc_event_wait_gateway_ready_or_resumed_types) /
    sizeof(dcc_event_wait_gateway_ready_or_resumed_types[0]);

const dcc_event_type_t dcc_event_wait_gateway_ready_resumed_or_close_types[] = {
    DCC_EVENT_READY,
    DCC_EVENT_RESUMED,
    DCC_EVENT_SOCKET_CLOSE
};

const size_t dcc_event_wait_gateway_ready_resumed_or_close_type_count =
    sizeof(dcc_event_wait_gateway_ready_resumed_or_close_types) /
    sizeof(dcc_event_wait_gateway_ready_resumed_or_close_types[0]);

const dcc_event_type_t dcc_event_wait_interaction_or_close_types[] = {
    DCC_EVENT_SLASH_COMMAND,
    DCC_EVENT_BUTTON_CLICK,
    DCC_EVENT_AUTOCOMPLETE,
    DCC_EVENT_SELECT_CLICK,
    DCC_EVENT_MESSAGE_CONTEXT_MENU,
    DCC_EVENT_USER_CONTEXT_MENU,
    DCC_EVENT_FORM_SUBMIT,
    DCC_EVENT_INTERACTION_CREATE,
    DCC_EVENT_SOCKET_CLOSE
};

const size_t dcc_event_wait_interaction_or_close_type_count =
    sizeof(dcc_event_wait_interaction_or_close_types) /
    sizeof(dcc_event_wait_interaction_or_close_types[0]);

const dcc_event_type_t dcc_event_wait_voice_session_descriptor_types[] = {
    DCC_EVENT_VOICE_STATE_UPDATE,
    DCC_EVENT_VOICE_SERVER_UPDATE
};

const size_t dcc_event_wait_voice_session_descriptor_type_count =
    sizeof(dcc_event_wait_voice_session_descriptor_types) /
    sizeof(dcc_event_wait_voice_session_descriptor_types[0]);

int dcc_event_wait_snapshot_is_interaction(const dcc_event_snapshot_t *snapshot) {
    if (snapshot == NULL) {
        return 0;
    }
    switch (snapshot->type) {
        case DCC_EVENT_SLASH_COMMAND:
        case DCC_EVENT_BUTTON_CLICK:
        case DCC_EVENT_AUTOCOMPLETE:
        case DCC_EVENT_SELECT_CLICK:
        case DCC_EVENT_MESSAGE_CONTEXT_MENU:
        case DCC_EVENT_USER_CONTEXT_MENU:
        case DCC_EVENT_FORM_SUBMIT:
        case DCC_EVENT_INTERACTION_CREATE:
            return 1;
        default:
            return 0;
    }
}
