#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_sticker_list_object(
    dcc_json_parser_t *parser,
    dcc_sticker_t *sticker,
    char *name,
    size_t name_cap,
    char *description,
    size_t description_cap,
    char *tags,
    size_t tags_cap,
    char *asset,
    size_t asset_cap,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (parser == NULL || sticker == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_sticker_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_sticker_list_buffers(
        &buffers,
        name,
        name_cap,
        description,
        description_cap,
        tags,
        tags_cap,
        asset,
        asset_cap,
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

    memset(sticker, 0, sizeof(*sticker));
    parser->cur++;
    return dcc_json_gateway_parse_sticker_list_object_fields(parser, sticker, &buffers);
}
