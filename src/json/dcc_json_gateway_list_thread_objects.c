#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_thread_list_object(
    dcc_json_parser_t *parser,
    dcc_channel_t *thread,
    char *name,
    size_t name_cap,
    char *topic,
    size_t topic_cap
) {
    const dcc_json_gateway_thread_buffers_t buffers = {
        name,
        name_cap,
        topic,
        topic_cap
    };

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || thread == NULL || name == NULL || topic == NULL) {
        return DCC_ERR_JSON;
    }
    if (*parser->cur != '{') {
        return dcc_json_skip_value(parser);
    }

    memset(thread, 0, sizeof(*thread));
    parser->cur++;
    dcc_json_skip_ws(parser);
    if (parser->cur < parser->end && *parser->cur == '}') {
        parser->cur++;
        return DCC_OK;
    }

    return dcc_json_gateway_parse_thread_list_object_fields(parser, thread, &buffers);
}
