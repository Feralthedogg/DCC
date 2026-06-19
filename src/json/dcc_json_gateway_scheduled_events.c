#include "internal/json/dcc_json_internal.h"

#include <string.h>

dcc_status_t dcc_json_gateway_parse_scheduled_event_list_object(
    dcc_json_parser_t *parser,
    dcc_scheduled_event_t *event,
    char *name,
    size_t name_cap,
    char *description,
    size_t description_cap,
    char *image,
    size_t image_cap,
    char *location,
    size_t location_cap,
    char *start_time,
    size_t start_time_cap,
    char *end_time,
    size_t end_time_cap,
    char *creator_username,
    size_t creator_username_cap,
    char *creator_global_name,
    size_t creator_global_name_cap,
    char *creator_avatar,
    size_t creator_avatar_cap
) {
    if (parser == NULL || event == NULL) {
        return DCC_ERR_JSON;
    }
    dcc_json_gateway_scheduled_event_buffers_t buffers;
    dcc_status_t status = dcc_json_gateway_init_scheduled_event_buffers(
        &buffers,
        name,
        name_cap,
        description,
        description_cap,
        image,
        image_cap,
        location,
        location_cap,
        start_time,
        start_time_cap,
        end_time,
        end_time_cap,
        creator_username,
        creator_username_cap,
        creator_global_name,
        creator_global_name_cap,
        creator_avatar,
        creator_avatar_cap
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

    memset(event, 0, sizeof(*event));
    parser->cur++;
    return dcc_json_gateway_parse_scheduled_event_object_fields(parser, event, &buffers);
}
