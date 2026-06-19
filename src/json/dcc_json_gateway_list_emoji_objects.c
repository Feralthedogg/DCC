#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_emoji_list_object(
    dcc_json_parser_t *parser,
    dcc_emoji_t *emoji,
    char *name,
    size_t name_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (parser == NULL || emoji == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_emoji_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_emoji_list_buffers(
        &buffers,
        name,
        name_cap,
        username,
        username_cap,
        global_name,
        global_name_cap,
        avatar,
        avatar_cap
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(emoji, 0, sizeof(*emoji));
    parser->cur++;
    return dcc_json_gateway_parse_emoji_list_object_fields(parser, emoji, &buffers);
}
