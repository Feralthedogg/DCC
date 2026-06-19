#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_user_object(
    dcc_json_parser_t *parser,
    dcc_user_t *user,
    char *username,
    size_t username_cap,
    char *global_name,
    size_t global_name_cap,
    char *avatar,
    size_t avatar_cap
) {
    if (parser == NULL || user == NULL || username == NULL || global_name == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_gateway_user_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_user_buffers(
        &buffers,
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

    parser->cur++;
    return dcc_json_gateway_parse_user_object_fields(parser, user, &buffers);
}
