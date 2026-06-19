#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_resolved_message_object(
    dcc_json_parser_t *parser,
    dcc_message_t *message,
    char *content,
    size_t content_cap,
    char *timestamp,
    size_t timestamp_cap,
    char *edited_timestamp,
    size_t edited_timestamp_cap,
    char *author_username,
    size_t author_username_cap,
    char *author_global_name,
    size_t author_global_name_cap,
    char *author_avatar,
    size_t author_avatar_cap
) {
    if (parser == NULL || message == NULL || content == NULL || timestamp == NULL ||
        edited_timestamp == NULL || author_username == NULL || author_global_name == NULL ||
        author_avatar == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    dcc_json_gateway_resolved_message_buffers_t buffers = {
        content,
        content_cap,
        timestamp,
        timestamp_cap,
        edited_timestamp,
        edited_timestamp_cap,
        author_username,
        author_username_cap,
        author_global_name,
        author_global_name_cap,
        author_avatar,
        author_avatar_cap
    };

    memset(message, 0, sizeof(*message));
    parser->cur++;
    return dcc_json_gateway_parse_resolved_message_object_fields(parser, message, &buffers);
}
