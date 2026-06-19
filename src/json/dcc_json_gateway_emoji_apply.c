#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_apply_emoji_id(dcc_json_gateway_payload_t *out, uint64_t value) {
    if (out == NULL) {
        return;
    }

    out->gateway.emoji_id = value;
    out->soundboard_sound.emoji_id = value;
    out->message_reaction.emoji_id = value;
    out->message_reaction.emoji.id = value;
    out->voice_channel_effect.emoji_id = value;
    out->has_soundboard_sound = 1;
    out->has_message_reaction = 1;
    out->has_voice_channel_effect = 1;
}

void dcc_json_gateway_apply_emoji_name(dcc_json_gateway_payload_t *out, const char *value) {
    if (out == NULL) {
        return;
    }

    out->gateway.emoji_name = value;
    out->soundboard_sound.emoji_name = value;
    out->message_reaction.emoji_name = value;
    out->message_reaction.emoji.name = value;
    out->voice_channel_effect.emoji_name = value;
    out->has_soundboard_sound = 1;
    out->has_message_reaction = 1;
    out->has_voice_channel_effect = 1;
}

void dcc_json_gateway_apply_emoji_animated(dcc_json_gateway_payload_t *out, int value) {
    if (out == NULL) {
        return;
    }

    out->message_reaction.emoji.animated = value ? 1U : 0U;
    out->has_message_reaction = 1;
}
