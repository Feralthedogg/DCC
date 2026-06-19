#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_member_list_object_with_buffers(
    dcc_json_parser_t *parser,
    dcc_member_t *member,
    dcc_snowflake_t guild_id,
    const dcc_json_gateway_member_buffers_t *buffers
) {
    if (parser == NULL || member == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    dcc_json_gateway_prepare_member_list_object(
        member,
        guild_id,
        buffers->permissions,
        buffers->has_permissions
    );
    parser->cur++;
    return dcc_json_gateway_parse_member_list_object_fields(parser, member, buffers);
}
