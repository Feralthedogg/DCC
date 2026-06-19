#include "internal/interactions/dcc_interaction_server_internal.h"

static int dcc_interaction_component_is_select(uint32_t component_type) {
    return component_type == 3U || component_type == 5U ||
        component_type == 6U || component_type == 7U ||
        component_type == 8U;
}

dcc_event_type_t dcc_interaction_event_type_from_payload(const dcc_interaction_t *interaction) {
    if (interaction == NULL) {
        return DCC_EVENT_INTERACTION_CREATE;
    }
    if (interaction->type == 2U) {
        if (interaction->command_type == 2U) {
            return DCC_EVENT_USER_CONTEXT_MENU;
        }
        if (interaction->command_type == 3U) {
            return DCC_EVENT_MESSAGE_CONTEXT_MENU;
        }
        return DCC_EVENT_SLASH_COMMAND;
    }
    if (interaction->type == 3U) {
        if (interaction->component_type == 2U) {
            return DCC_EVENT_BUTTON_CLICK;
        }
        if (dcc_interaction_component_is_select(interaction->component_type)) {
            return DCC_EVENT_SELECT_CLICK;
        }
    }
    if (interaction->type == 4U) {
        return DCC_EVENT_AUTOCOMPLETE;
    }
    if (interaction->type == 5U) {
        return DCC_EVENT_FORM_SUBMIT;
    }
    return DCC_EVENT_INTERACTION_CREATE;
}

dcc_status_t dcc_interaction_call_route(
    const dcc_interaction_route_entry_t *route,
    dcc_interaction_request_t *request
) {
    if (route == NULL || request == NULL || route->callback == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    route->callback(request->server, request, route->user_data);
    if (!request->response_set) {
        return dcc_interaction_request_set_response_copy(
            request,
            204,
            "text/plain; charset=utf-8",
            "",
            0U
        );
    }
    return DCC_OK;
}

dcc_status_t dcc_interaction_set_default_response(dcc_interaction_request_t *request) {
    const dcc_interaction_t *interaction = dcc_interaction_request_interaction(request);
    if (interaction != NULL && interaction->type == 1U) {
        return dcc_interaction_request_reply_pong(request);
    }
    return dcc_interaction_request_reply_deferred_message(request);
}
