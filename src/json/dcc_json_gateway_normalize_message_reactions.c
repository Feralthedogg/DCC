#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_message_reaction(dcc_json_gateway_payload_t *out) {
    if (out->message_reaction.user_id == 0 && out->gateway.user_id != 0) {
        out->message_reaction.user_id = out->gateway.user_id;
    }
    if (out->message_reaction.channel_id == 0 && out->gateway.channel_id != 0) {
        out->message_reaction.channel_id = out->gateway.channel_id;
    }
    if (out->message_reaction.message_id == 0 && out->gateway.message_id != 0) {
        out->message_reaction.message_id = out->gateway.message_id;
    }
    if (out->message_reaction.guild_id == 0 && out->gateway.guild_id != 0) {
        out->message_reaction.guild_id = out->gateway.guild_id;
    }
    if (out->message_reaction.message_author_id == 0 && out->gateway.message_author_id != 0) {
        out->message_reaction.message_author_id = out->gateway.message_author_id;
    }
    if (out->message_reaction.emoji_id == 0 && out->gateway.emoji_id != 0) {
        out->message_reaction.emoji_id = out->gateway.emoji_id;
    }
    if (out->message_reaction.emoji.id == 0 && out->message_reaction.emoji_id != 0) {
        out->message_reaction.emoji.id = out->message_reaction.emoji_id;
    }
    if (out->message_reaction.emoji_id == 0 && out->message_reaction.emoji.id != 0) {
        out->message_reaction.emoji_id = out->message_reaction.emoji.id;
    }
    if (out->message_reaction.emoji.name == NULL && out->message_reaction.emoji_name != NULL) {
        out->message_reaction.emoji.name = out->message_reaction.emoji_name;
    }
    if (out->message_reaction.emoji_name == NULL && out->message_reaction.emoji.name != NULL) {
        out->message_reaction.emoji_name = out->message_reaction.emoji.name;
    }
    if (!out->has_event_name ||
        !dcc_json_event_name_eq(
            out->event_name,
            sizeof(out->event_name),
            "MESSAGE_REACTION_ADD",
            sizeof("MESSAGE_REACTION_ADD") - 1
        )) {
        return;
    }
    if (out->has_member) {
        if (out->member.guild_id == 0 && out->message_reaction.guild_id != 0) {
            out->member.guild_id = out->message_reaction.guild_id;
        }
        if (out->member.user.id == 0 && out->message_reaction.user_id != 0) {
            out->member.user.id = out->message_reaction.user_id;
        }
        out->message_reaction.member = out->member;
        if (out->member.user.id != 0) {
            out->message_reaction.user = out->member.user;
        }
        out->has_message_reaction = 1;
    } else if (out->message_reaction.user.id == 0 && out->gateway.user.id != 0) {
        out->message_reaction.user = out->gateway.user;
        out->has_message_reaction = 1;
    }
}
