#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_array_into(
    dcc_json_parser_t *parser,
    dcc_gateway_activity_t *activities,
    size_t activities_cap,
    char *names,
    size_t name_stride,
    char *states,
    size_t state_stride,
    char *details,
    size_t details_stride,
    char *urls,
    size_t url_stride,
    char *emoji_names,
    size_t emoji_name_stride,
    const dcc_gateway_activity_t **out_activities,
    size_t *out_count,
    uint8_t *out_truncated
) {
    const dcc_json_gateway_activity_array_t array = {
        activities,
        activities_cap,
        names,
        name_stride,
        states,
        state_stride,
        details,
        details_stride,
        urls,
        url_stride,
        emoji_names,
        emoji_name_stride,
        out_activities,
        out_count,
        out_truncated
    };

    if (parser == NULL || activities == NULL || names == NULL ||
        states == NULL || details == NULL || urls == NULL || emoji_names == NULL ||
        out_activities == NULL || out_count == NULL || out_truncated == NULL) {
        return DCC_ERR_JSON;
    }

    dcc_json_skip_ws(parser);
    if (parser->cur >= parser->end || *parser->cur != '[') {
        return DCC_ERR_JSON;
    }

    parser->cur++;
    return dcc_json_gateway_parse_activity_array_items(parser, &array);
}
