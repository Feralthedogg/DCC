#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_role_list_object(
    dcc_json_parser_t *parser,
    dcc_role_t *role,
    char *name,
    size_t name_cap
) {
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || role == NULL || name == NULL) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(role, 0, sizeof(*role));
    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    for (;;) {
        dcc_status_t status = dcc_json_gateway_parse_role_list_object_field(
            parser,
            role,
            name,
            name_cap
        );
        if (status != DCC_OK) {
            return status;
        }

        dcc_json_skip_ws(parser);
        if (parser->cur >= parser->end) {
            return DCC_ERR_JSON;
        }
        if (*parser->cur == '}') {
            parser->cur++;
            return DCC_OK;
        }
        if (*parser->cur != ',') {
            return DCC_ERR_JSON;
        }
        parser->cur++;
        dcc_json_skip_ws(parser);
    }
}
