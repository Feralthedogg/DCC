#include "gateway_smoke_cache.h"

#if !defined(_WIN32)

#include <stdio.h>
#include <string.h>

int gateway_smoke_cache_ok(dcc_client_t *client) {
    if (!dcc_cache_enabled(client)) {
        fprintf(stderr, "gateway cache should be enabled\n");
        return 0;
    }

    const dcc_guild_t *guild = dcc_cache_get_guild(client, 333);
    const dcc_channel_t *channel = dcc_cache_get_channel(client, 222);
    const dcc_user_t *self_user = dcc_cache_get_user(client, 447);
    const dcc_user_t *member_user = dcc_cache_get_user(client, 446);
    const dcc_message_t *message = dcc_cache_get_message(client, 113);
    dcc_cache_counts_t counts = {
        .size = sizeof(counts),
    };

    if (guild == NULL ||
        guild->name == NULL ||
        strcmp(guild->name, "DCC Guild Updated") != 0 ||
        guild->owner_id != 447 ||
        guild->default_message_notifications != 0 ||
        guild->widget_enabled != 0 ||
        channel == NULL ||
        channel->topic == NULL ||
        strcmp(channel->topic, "updated topic") != 0 ||
        self_user == NULL ||
        self_user->username == NULL ||
        strcmp(self_user->username, "self") != 0 ||
        member_user == NULL ||
        member_user->username == NULL ||
        strcmp(member_user->username, "member") != 0 ||
        message == NULL ||
        message->content == NULL ||
        strcmp(message->content, "persistent edited") != 0 ||
        message->timestamp == NULL ||
        strcmp(message->timestamp, "2026-06-15T04:00:00.000000+00:00") != 0 ||
        message->edited_timestamp == NULL ||
        strcmp(message->edited_timestamp, "2026-06-15T05:00:00.000000+00:00") != 0 ||
        message->author.username == NULL ||
        strcmp(message->author.username, "cache-author") != 0 ||
        message->author.avatar == NULL ||
        strcmp(message->author.avatar, "cache-author-avatar") != 0 ||
        message->flags != 0 ||
        message->type != 0 ||
        message->tts != 0 ||
        message->mention_everyone != 0 ||
        message->pinned != 0 ||
        dcc_cache_get_message(client, 111) != NULL ||
        dcc_cache_get_role(client, 333, 555) != NULL ||
        dcc_cache_get_member(client, 333, 446) != NULL ||
        dcc_cache_get_voice_state(client, 333, 444) != NULL ||
        dcc_cache_counts(client, &counts) != DCC_OK ||
        counts.guilds != 1 ||
        counts.channels != 1 ||
        counts.roles != 0 ||
        counts.messages != 1 ||
        counts.voice_states != 0 ||
        counts.users < 3) {
        fprintf(stderr,
                "gateway cache invalid: guilds=%zu channels=%zu roles=%zu members=%zu users=%zu messages=%zu voice=%zu\n",
                counts.guilds,
                counts.channels,
                counts.roles,
                counts.members,
                counts.users,
                counts.messages,
                counts.voice_states);
        return 0;
    }

    return 1;
}

#endif
