#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_soundboard_array_item(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser == NULL || out == NULL || stored == NULL || truncated == NULL) {
        return DCC_ERR_JSON;
    }

    if (parser->cur < parser->end && *parser->cur == '{' && *stored < DCC_JSON_GATEWAY_OBJECT_LIST_CAP) {
        size_t index = *stored;
        dcc_status_t status = dcc_json_gateway_parse_soundboard_list_object(
            parser,
            &out->gateway_soundboard_sounds[index],
            out->gateway_soundboard_sound_names[index],
            sizeof(out->gateway_soundboard_sound_names[index]),
            out->gateway_soundboard_sound_emoji_names[index],
            sizeof(out->gateway_soundboard_sound_emoji_names[index]),
            out->gateway_soundboard_sound_usernames[index],
            sizeof(out->gateway_soundboard_sound_usernames[index]),
            out->gateway_soundboard_sound_user_global_names[index],
            sizeof(out->gateway_soundboard_sound_user_global_names[index]),
            out->gateway_soundboard_sound_user_avatars[index],
            sizeof(out->gateway_soundboard_sound_user_avatars[index])
        );
        if (status != DCC_OK) {
            return status;
        }
        if (out->gateway_soundboard_sounds[index].id != 0 && index < DCC_JSON_GATEWAY_ID_LIST_CAP) {
            out->gateway_ids[index] = out->gateway_soundboard_sounds[index].id;
        }
        *stored = index + 1U;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }
    *truncated = 1;
    return DCC_OK;
}
