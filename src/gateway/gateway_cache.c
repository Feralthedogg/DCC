#include "internal/gateway/dcc_gateway_cache_internal.h"

void dcc_gateway_cache_apply_event(dcc_client_t *client, const dcc_event_t *event) {
    if (!dcc_cache_enabled(client) || event == NULL) {
        return;
    }

    switch (event->type) {
        case DCC_EVENT_READY:
            dcc_cache_clear(client);
            break;
        default:
            (void)(dcc_gateway_cache_apply_message_event(client, event) ||
                   dcc_gateway_cache_apply_guild_event(client, event) ||
                   dcc_gateway_cache_apply_channel_event(client, event) ||
                   dcc_gateway_cache_apply_member_event(client, event) ||
                   dcc_gateway_cache_apply_user_voice_event(client, event));
            break;
    }
}
