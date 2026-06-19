#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_entitlement_object(
    dcc_json_parser_t *parser,
    dcc_entitlement_t *entitlement,
    char *start_time,
    size_t start_time_cap,
    char *end_time,
    size_t end_time_cap
) {
    if (parser == NULL || entitlement == NULL || start_time == NULL || end_time == NULL) {
        return DCC_ERR_JSON;
    }

    const dcc_json_gateway_entitlement_buffers_t buffers = {
        start_time,
        start_time_cap,
        end_time,
        end_time_cap
    };

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(entitlement, 0, sizeof(*entitlement));
    parser->cur++;
    return dcc_json_gateway_parse_entitlement_object_fields(parser, entitlement, &buffers);
}
