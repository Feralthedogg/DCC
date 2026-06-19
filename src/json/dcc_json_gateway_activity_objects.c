#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_activity_object(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activity,
    char *name,
    size_t name_cap,
    char *state,
    size_t state_cap,
    char *details,
    size_t details_cap,
    char *url,
    size_t url_cap,
    char *emoji_name,
    size_t emoji_name_cap
) {
    if (parser == NULL || activity == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_gateway_activity_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_activity_buffers(
        &buffers,
        name,
        name_cap,
        state,
        state_cap,
        details,
        details_cap,
        url,
        url_cap,
        emoji_name,
        emoji_name_cap
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

    memset(activity, 0, sizeof(*activity));
    parser->cur++;
    return dcc_json_gateway_parse_activity_object_fields(parser, activity, &buffers);
}
