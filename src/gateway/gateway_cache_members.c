#include "internal/gateway/dcc_gateway_cache_internal.h"

uint8_t dcc_gateway_cache_apply_member_event(dcc_client_t *client, const dcc_event_t *event) {
    switch (event->type) {
        case DCC_EVENT_GUILD_ROLE_CREATE:
            dcc_gateway_cache_put_role_if_valid(client, &event->data.role);
            return 1U;
        case DCC_EVENT_GUILD_ROLE_UPDATE:
            if (event->data.role.guild_id != 0 && event->data.role.id != 0) {
                (void)dcc_cache_merge_role(client, &event->data.role, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_GUILD_ROLE_DELETE:
            if (event->data.role.guild_id != 0 && event->data.role.id != 0) {
                (void)dcc_cache_remove_role(client, event->data.role.guild_id, event->data.role.id);
            } else if (event->gateway != NULL && event->gateway->guild_id != 0 && event->gateway->role_id != 0) {
                (void)dcc_cache_remove_role(client, event->gateway->guild_id, event->gateway->role_id);
            }
            return 1U;
        case DCC_EVENT_GUILD_MEMBER_ADD:
            dcc_gateway_cache_put_member_if_valid(client, &event->data.member);
            return 1U;
        case DCC_EVENT_GUILD_MEMBER_UPDATE:
            if (event->data.member.guild_id != 0 && event->data.member.user.id != 0) {
                dcc_gateway_cache_put_user_if_valid(client, &event->data.member.user);
                (void)dcc_cache_merge_member(client, &event->data.member, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_GUILD_MEMBER_REMOVE:
            dcc_gateway_cache_put_user_if_valid(client, &event->data.member.user);
            if (event->data.member.guild_id != 0 && event->data.member.user.id != 0) {
                (void)dcc_cache_remove_member(client, event->data.member.guild_id, event->data.member.user.id);
            }
            return 1U;
        case DCC_EVENT_GUILD_MEMBERS_CHUNK:
            dcc_gateway_cache_put_members(
                client,
                event->data.guild_members_chunk.members,
                event->data.guild_members_chunk.members_count
            );
            return 1U;
        default:
            return 0U;
    }
}
