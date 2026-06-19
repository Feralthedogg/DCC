#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_client_status(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    char *desktop_status,
    size_t desktop_status_cap,
    char *mobile_status,
    size_t mobile_status_cap,
    char *web_status,
    size_t web_status_cap
) {
    const dcc_json_gateway_presence_status_buffers_t buffers = {
        desktop_status,
        desktop_status_cap,
        mobile_status,
        mobile_status_cap,
        web_status,
        web_status_cap
    };

    if (parser == NULL || presence == NULL || desktop_status == NULL ||
        mobile_status == NULL || web_status == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    return dcc_json_gateway_parse_presence_client_status_fields(parser, presence, &buffers);
}
