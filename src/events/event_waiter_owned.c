#include "internal/events/dcc_events_internal.h"

void *dcc_event_waiter_clone_owned(
    dcc_event_wait_kind_t kind,
    const dcc_event_t *event,
    dcc_status_t *out_status
) {
    if (out_status == NULL) {
        return NULL;
    }
    *out_status = DCC_OK;
    switch (kind) {
        case DCC_EVENT_WAIT_SNAPSHOT:
            return NULL;
        case DCC_EVENT_WAIT_GATEWAY_DATA:
            return dcc_event_waiter_clone_gateway_data(event, out_status);
        case DCC_EVENT_WAIT_MESSAGE:
            return dcc_event_waiter_clone_message(event, out_status);
        case DCC_EVENT_WAIT_GUILD:
            return dcc_event_waiter_clone_guild(event, out_status);
        case DCC_EVENT_WAIT_CHANNEL:
            return dcc_event_waiter_clone_channel(event, out_status);
        case DCC_EVENT_WAIT_INTERACTION:
            return dcc_event_waiter_clone_interaction(event, out_status);
        case DCC_EVENT_WAIT_INTERACTION_OR_CLOSE: {
            if (event != NULL && event->type == DCC_EVENT_SOCKET_CLOSE) {
                return NULL;
            }
            return dcc_event_waiter_clone_interaction(event, out_status);
        }
        case DCC_EVENT_WAIT_VOICE_STATE:
            return dcc_event_waiter_clone_voice_state(event, out_status);
        case DCC_EVENT_WAIT_MEMBER:
            return dcc_event_waiter_clone_member(event, out_status);
        case DCC_EVENT_WAIT_ROLE:
            return dcc_event_waiter_clone_role(event, out_status);
        case DCC_EVENT_WAIT_USER:
            return dcc_event_waiter_clone_user(event, out_status);
        default:
            *out_status = DCC_ERR_INVALID_ARG;
            return NULL;
    }
}
