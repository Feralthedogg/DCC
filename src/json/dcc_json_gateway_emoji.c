#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_emoji(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    if (parser == NULL || out == NULL) {
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
    return dcc_json_gateway_parse_emoji_object_fields(parser, out);
}
