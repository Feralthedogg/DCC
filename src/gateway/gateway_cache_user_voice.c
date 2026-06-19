#include "internal/gateway/dcc_gateway_cache_internal.h"

uint8_t dcc_gateway_cache_apply_user_voice_event(dcc_client_t *client, const dcc_event_t *event) {
    switch (event->type) {
        case DCC_EVENT_MESSAGE_REACTION_ADD:
            dcc_gateway_cache_put_member_if_valid(client, &event->data.message_reaction.member);
            dcc_gateway_cache_put_user_if_valid(client, &event->data.message_reaction.user);
            return 1U;
        case DCC_EVENT_TYPING_START:
            dcc_gateway_cache_put_member_if_valid(client, &event->data.typing_start.member);
            dcc_gateway_cache_put_user_if_valid(client, &event->data.typing_start.user);
            return 1U;
        case DCC_EVENT_USER_UPDATE:
            if (event->data.user.id != 0) {
                (void)dcc_cache_merge_user(client, &event->data.user, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_PRESENCE_UPDATE:
            if (event->data.presence.user.id != 0) {
                (void)dcc_cache_merge_user(client, &event->data.presence.user, event->cache_fields);
            }
            return 1U;
        case DCC_EVENT_GUILD_BAN_ADD:
        case DCC_EVENT_GUILD_BAN_REMOVE:
            dcc_gateway_cache_put_user_if_valid(client, &event->data.guild_ban.user);
            return 1U;
        case DCC_EVENT_VOICE_STATE_UPDATE:
            if (event->data.voice_state.guild_id == 0 || event->data.voice_state.user_id == 0) {
                return 1U;
            }
            if (event->data.voice_state.channel_id == 0) {
                (void)dcc_cache_remove_voice_state(
                    client,
                    event->data.voice_state.guild_id,
                    event->data.voice_state.user_id
                );
            } else {
                (void)dcc_cache_merge_voice_state(client, &event->data.voice_state, event->cache_fields);
            }
            return 1U;
        default:
            return 0U;
    }
}
