#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_soundboard_resource(dcc_json_gateway_payload_t *out) {
    if (out->soundboard_sound.id == 0 && out->gateway.sound_id != 0) {
        out->soundboard_sound.id = out->gateway.sound_id;
    }
    if (out->soundboard_sound.guild_id == 0 && out->gateway.guild_id != 0) {
        out->soundboard_sound.guild_id = out->gateway.guild_id;
    }
    if (out->soundboard_sound.user_id == 0 && out->gateway.user_id != 0) {
        out->soundboard_sound.user_id = out->gateway.user_id;
    }
    if (out->soundboard_sound.user.id == 0 && out->gateway.user.id != 0) {
        out->soundboard_sound.user = out->gateway.user;
    }
    if (out->soundboard_sound.emoji_id == 0 && out->gateway.emoji_id != 0) {
        out->soundboard_sound.emoji_id = out->gateway.emoji_id;
    }
}
