#include "internal/gateway/dcc_gateway_cache_internal.h"

uint8_t dcc_gateway_cache_apply_guild_event(dcc_client_t *client, const dcc_event_t *event) {
    switch (event->type) {
        case DCC_EVENT_GUILD_CREATE:
            dcc_gateway_cache_put_guild_if_valid(client, &event->data.guild);
            if (event->gateway != NULL) {
                dcc_gateway_cache_put_members(client, event->gateway->members, event->gateway->members_count);
            }
            return 1U;
        case DCC_EVENT_GUILD_UPDATE:
            if (event->data.guild.id != 0) {
                (void)dcc_cache_merge_guild(client, &event->data.guild, event->cache_fields);
                dcc_gateway_cache_put_guild_children(client, &event->data.guild);
            }
            if (event->gateway != NULL) {
                dcc_gateway_cache_put_members(client, event->gateway->members, event->gateway->members_count);
            }
            return 1U;
        case DCC_EVENT_GUILD_DELETE:
            if (event->data.guild.id != 0 && !event->data.guild.unavailable) {
                (void)dcc_cache_remove_guild(client, event->data.guild.id);
            }
            return 1U;
        default:
            return 0U;
    }
}
