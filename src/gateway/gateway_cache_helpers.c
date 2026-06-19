#include "internal/gateway/dcc_gateway_cache_internal.h"

void dcc_gateway_cache_put_user_if_valid(dcc_client_t *client, const dcc_user_t *user) {
    if (user != NULL && user->id != 0) {
        (void)dcc_cache_put_user(client, user);
    }
}

void dcc_gateway_cache_put_member_if_valid(dcc_client_t *client, const dcc_member_t *member) {
    if (member != NULL && member->guild_id != 0 && member->user.id != 0) {
        dcc_gateway_cache_put_user_if_valid(client, &member->user);
        (void)dcc_cache_put_member(client, member);
    }
}

void dcc_gateway_cache_put_role_if_valid(dcc_client_t *client, const dcc_role_t *role) {
    if (role != NULL && role->guild_id != 0 && role->id != 0) {
        (void)dcc_cache_put_role(client, role);
    }
}

void dcc_gateway_cache_put_channel_if_valid(dcc_client_t *client, const dcc_channel_t *channel) {
    if (channel != NULL && channel->id != 0) {
        (void)dcc_cache_put_channel(client, channel);
    }
}

void dcc_gateway_cache_put_message_if_valid(dcc_client_t *client, const dcc_message_t *message) {
    if (message != NULL && message->id != 0) {
        dcc_gateway_cache_put_user_if_valid(client, &message->author);
        (void)dcc_cache_put_message(client, message);
    }
}

void dcc_gateway_cache_put_guild_children(dcc_client_t *client, const dcc_guild_t *guild) {
    if (guild == NULL) {
        return;
    }

    for (size_t i = 0; i < guild->roles_count; i++) {
        dcc_gateway_cache_put_role_if_valid(client, &guild->roles[i]);
    }
    for (size_t i = 0; i < guild->channels_count; i++) {
        dcc_gateway_cache_put_channel_if_valid(client, &guild->channels[i]);
    }
    for (size_t i = 0; i < guild->threads_count; i++) {
        dcc_gateway_cache_put_channel_if_valid(client, &guild->threads[i]);
    }
}

void dcc_gateway_cache_put_guild_if_valid(dcc_client_t *client, const dcc_guild_t *guild) {
    if (guild != NULL && guild->id != 0) {
        (void)dcc_cache_put_guild(client, guild);
        dcc_gateway_cache_put_guild_children(client, guild);
    }
}

void dcc_gateway_cache_put_members(
    dcc_client_t *client,
    const dcc_member_t *members,
    size_t members_count
) {
    if (members == NULL) {
        return;
    }
    for (size_t i = 0; i < members_count; i++) {
        dcc_gateway_cache_put_member_if_valid(client, &members[i]);
    }
}

void dcc_gateway_cache_put_channels(
    dcc_client_t *client,
    const dcc_channel_t *channels,
    size_t channels_count
) {
    if (channels == NULL) {
        return;
    }
    for (size_t i = 0; i < channels_count; i++) {
        dcc_gateway_cache_put_channel_if_valid(client, &channels[i]);
    }
}

void dcc_gateway_cache_remove_messages(
    dcc_client_t *client,
    const dcc_snowflake_t *message_ids,
    size_t message_ids_count
) {
    if (message_ids == NULL) {
        return;
    }
    for (size_t i = 0; i < message_ids_count; i++) {
        if (message_ids[i] != 0) {
            (void)dcc_cache_remove_message(client, message_ids[i]);
        }
    }
}
