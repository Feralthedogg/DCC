#ifndef DCC_GATEWAY_CACHE_INTERNAL_H
#define DCC_GATEWAY_CACHE_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"
#include "internal/gateway/dcc_gateway_dispatch_internal.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_gateway_cache_put_user_if_valid(dcc_client_t *client, const dcc_user_t *user);
void dcc_gateway_cache_put_member_if_valid(dcc_client_t *client, const dcc_member_t *member);
void dcc_gateway_cache_put_role_if_valid(dcc_client_t *client, const dcc_role_t *role);
void dcc_gateway_cache_put_channel_if_valid(dcc_client_t *client, const dcc_channel_t *channel);
void dcc_gateway_cache_put_message_if_valid(dcc_client_t *client, const dcc_message_t *message);
void dcc_gateway_cache_put_guild_children(dcc_client_t *client, const dcc_guild_t *guild);
void dcc_gateway_cache_put_guild_if_valid(dcc_client_t *client, const dcc_guild_t *guild);
void dcc_gateway_cache_put_members(
    dcc_client_t *client,
    const dcc_member_t *members,
    size_t members_count
);
void dcc_gateway_cache_put_channels(
    dcc_client_t *client,
    const dcc_channel_t *channels,
    size_t channels_count
);
void dcc_gateway_cache_remove_messages(
    dcc_client_t *client,
    const dcc_snowflake_t *message_ids,
    size_t message_ids_count
);
uint8_t dcc_gateway_cache_apply_message_event(dcc_client_t *client, const dcc_event_t *event);
uint8_t dcc_gateway_cache_apply_guild_event(dcc_client_t *client, const dcc_event_t *event);
uint8_t dcc_gateway_cache_apply_channel_event(dcc_client_t *client, const dcc_event_t *event);
uint8_t dcc_gateway_cache_apply_member_event(dcc_client_t *client, const dcc_event_t *event);
uint8_t dcc_gateway_cache_apply_user_voice_event(dcc_client_t *client, const dcc_event_t *event);

#ifdef __cplusplus
}
#endif

#endif
