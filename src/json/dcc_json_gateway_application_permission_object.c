#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_application_command_permission_object(
    dcc_json_parser_t *parser,
    dcc_application_command_permission_t *permission
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || permission == NULL) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(permission, 0, sizeof(*permission));
    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    return dcc_json_gateway_parse_application_command_permission_object_fields(parser, permission);
}
