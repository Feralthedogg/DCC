#include "internal/events/dcc_events_internal.h"

void dcc_event_waiter_free_owned(dcc_event_wait_kind_t kind, void *owned) {
    if (owned == NULL) {
        return;
    }
    switch (kind) {
        case DCC_EVENT_WAIT_GATEWAY_DATA:
            dcc_gateway_event_data_free((dcc_gateway_event_data_t *)owned);
            break;
        case DCC_EVENT_WAIT_MESSAGE:
            dcc_message_free((dcc_message_t *)owned);
            break;
        case DCC_EVENT_WAIT_GUILD:
            dcc_guild_free((dcc_guild_t *)owned);
            break;
        case DCC_EVENT_WAIT_CHANNEL:
            dcc_channel_free((dcc_channel_t *)owned);
            break;
        case DCC_EVENT_WAIT_INTERACTION:
        case DCC_EVENT_WAIT_INTERACTION_OR_CLOSE:
            dcc_interaction_free((dcc_interaction_t *)owned);
            break;
        case DCC_EVENT_WAIT_VOICE_STATE:
            dcc_voice_state_free((dcc_voice_state_t *)owned);
            break;
        case DCC_EVENT_WAIT_MEMBER:
            dcc_member_free((dcc_member_t *)owned);
            break;
        case DCC_EVENT_WAIT_ROLE:
            dcc_role_free((dcc_role_t *)owned);
            break;
        case DCC_EVENT_WAIT_USER:
            dcc_user_free((dcc_user_t *)owned);
            break;
        case DCC_EVENT_WAIT_SNAPSHOT:
        default:
            break;
    }
}
