#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_soundboard_list_object(
    dcc_json_parser_t *parser,
    dcc_soundboard_sound_t *sound,
    char *name,
    size_t name_cap,
    char *emoji_name,
    size_t emoji_name_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (parser == NULL || sound == NULL || name == NULL || emoji_name == NULL ||
        username == NULL || global_name == NULL || avatar == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    const dcc_json_gateway_soundboard_buffers_t buffers = {
        name,
        name_cap,
        emoji_name,
        emoji_name_cap,
        username,
        username_cap,
        global_name,
        global_name_cap,
        avatar,
        avatar_cap
    };

    memset(sound, 0, sizeof(*sound));
    parser->cur++;
    return dcc_json_gateway_parse_soundboard_list_object_fields(parser, sound, &buffers);
}
