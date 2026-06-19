#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_presence_list_activity_field(
    dcc_json_parser_t *parser,
    dcc_presence_t *presence,
    dcc_json_key_id_t key_id,
    const dcc_json_gateway_presence_list_buffers_t *buffers,
    int *handled
) {
    if (parser == NULL || presence == NULL || buffers == NULL || handled == NULL) {
        return DCC_ERR_JSON;
    }

    if (key_id != DCC_JSON_KEY_ACTIVITIES) {
        *handled = 0;
        return DCC_OK;
    }

    const dcc_gateway_activity_t *parsed_activities = NULL;
    size_t activities_count = 0;
    uint8_t activities_truncated = 0;
    dcc_status_t status = dcc_json_gateway_parse_activity_array_into(
        parser,
        buffers->activities,
        DCC_JSON_GATEWAY_ACTIVITY_CAP,
        buffers->activity_names,
        buffers->activity_name_stride,
        buffers->activity_states,
        buffers->activity_state_stride,
        buffers->activity_details,
        buffers->activity_detail_stride,
        buffers->activity_urls,
        buffers->activity_url_stride,
        buffers->activity_emoji_names,
        buffers->activity_emoji_name_stride,
        &parsed_activities,
        &activities_count,
        &activities_truncated
    );
    if (status != DCC_OK) {
        return status;
    }

    presence->activities = parsed_activities;
    presence->activities_count = activities_count;
    presence->activities_truncated = activities_truncated;
    *handled = 1;
    return DCC_OK;
}
