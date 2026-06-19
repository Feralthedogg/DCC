#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_d_media_id_key(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    dcc_json_key_id_t key_id,
    int *handled
) {
    if (handled == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    switch (key_id) {
        case DCC_JSON_KEY_SOUND_ID:
        case DCC_JSON_KEY_ANIMATION_ID:
        case DCC_JSON_KEY_EMOJI_ID:
            break;
        default:
            *handled = 0;
            return DCC_OK;
    }

    if (key_id == DCC_JSON_KEY_SOUND_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.sound_id = value;
        out->soundboard_sound.id = value;
        out->voice_channel_effect.sound_id = value;
        out->has_soundboard_sound = 1;
        out->has_voice_channel_effect = 1;
        (void)has_value;
    } else if (key_id == DCC_JSON_KEY_ANIMATION_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        out->gateway.animation_id = value;
        out->voice_channel_effect.animation_id = value;
        out->has_voice_channel_effect = 1;
        (void)has_value;
    } else if (key_id == DCC_JSON_KEY_EMOJI_ID) {
        uint64_t value = 0;
        int has_value = 0;
        dcc_status_t status = dcc_json_parse_nullable_u64_token(parser, &value, &has_value);
        if (status != DCC_OK) {
            return status;
        }
        dcc_json_gateway_apply_emoji_id(out, value);
        (void)has_value;
    } else {
        *handled = 0;
        return DCC_OK;
    }

    *handled = 1;
    return DCC_OK;
}
