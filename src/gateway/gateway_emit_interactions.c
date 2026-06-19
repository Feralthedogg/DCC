#include "internal/gateway/dcc_gateway_dispatch_internal.h"
#include "internal/gateway/dcc_gateway_emit_interactions_internal.h"

#include <limits.h>
#include <string.h>

static int dcc_gateway_interaction_component_is_select(uint32_t component_type) {
    return component_type == 3U ||
        component_type == 5U ||
        component_type == 6U ||
        component_type == 7U ||
        component_type == 8U;
}
static dcc_event_type_t dcc_gateway_interaction_subtype(const dcc_interaction_t *interaction) {
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
        if (dcc_gateway_interaction_component_is_select(interaction->component_type)) {
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
dcc_status_t dcc_gateway_emit_interaction(
    dcc_gateway_session_t *session,
    const dcc_json_gateway_payload_t *payload,
    const char *raw_json,
    size_t raw_json_len
) {
    dcc_event_type_t subtype = dcc_gateway_interaction_subtype(payload != NULL ? &payload->interaction : NULL);
    if (subtype != DCC_EVENT_INTERACTION_CREATE) {
        dcc_event_t subtype_event;
        dcc_gateway_init_event(&subtype_event, subtype, payload, raw_json, raw_json_len);
        subtype_event.data.interaction = payload->interaction;
        dcc_status_t status = dcc_gateway_dispatch_event(session, &subtype_event);
        if (status != DCC_OK) {
            return status;
        }
    }

    dcc_event_t event;
    dcc_gateway_init_event(&event, DCC_EVENT_INTERACTION_CREATE, payload, raw_json, raw_json_len);
    event.data.interaction = payload->interaction;
    return dcc_gateway_dispatch_event(session, &event);
}
