#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_presence_list_object_with_buffers(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    const dcc_json_gateway_presence_list_buffers_t *buffers
) {
    if (parser == NULL || presence == NULL || buffers == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(presence, 0, sizeof(*presence));
    parser->cur++;
    return dcc_json_gateway_parse_presence_list_object_fields(parser, presence, buffers);
}
