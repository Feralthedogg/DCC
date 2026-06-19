#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activity_array_item(
    dcc_json_parser_t *parser,
    const dcc_json_gateway_activity_array_t *array,
    size_t *stored,
    uint8_t *truncated
) {
    if (parser == NULL || array == NULL || stored == NULL || truncated == NULL) {
        return DCC_ERR_JSON;
    }

    if (parser->cur < parser->end && *parser->cur == '{' && *stored < array->activities_cap) {
        size_t index = *stored;
        dcc_status_t status = dcc_json_gateway_parse_activity_object(
            parser,
            &array->activities[index],
            array->names + (index * array->name_stride),
            array->name_stride,
            array->states + (index * array->state_stride),
            array->state_stride,
            array->details + (index * array->detail_stride),
            array->detail_stride,
            array->urls + (index * array->url_stride),
            array->url_stride,
            array->emoji_names + (index * array->emoji_name_stride),
            array->emoji_name_stride
        );
        if (status != DCC_OK) {
            return status;
        }
        *stored = index + 1U;
        return DCC_OK;
    }

    dcc_status_t status = dcc_json_skip_value(parser);
    if (status != DCC_OK) {
        return status;
    }
    if (*stored >= array->activities_cap) {
        *truncated = 1;
    }
    return DCC_OK;
}
