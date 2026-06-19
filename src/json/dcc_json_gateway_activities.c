#include "internal/json/dcc_json_internal.h"

dcc_status_t dcc_json_gateway_parse_activities(
    dcc_json_parser_t *parser,
    dcc_json_gateway_payload_t *out
) {
    const dcc_gateway_activity_t *activities = NULL;
    size_t count = 0;
    uint8_t truncated = 0;
    dcc_status_t status = dcc_json_gateway_parse_activity_array_into(
        parser,
        out->gateway_activities,
        DCC_JSON_GATEWAY_ACTIVITY_CAP,
        out->gateway_activity_names[0],
        sizeof(out->gateway_activity_names[0]),
        out->gateway_activity_states[0],
        sizeof(out->gateway_activity_states[0]),
        out->gateway_activity_details[0],
        sizeof(out->gateway_activity_details[0]),
        out->gateway_activity_urls[0],
        sizeof(out->gateway_activity_urls[0]),
        out->gateway_activity_emoji_names[0],
        sizeof(out->gateway_activity_emoji_names[0]),
        &activities,
        &count,
        &truncated
    );
    if (status != DCC_OK) {
        return status;
    }

    out->gateway.activities = activities;
    out->gateway.activities_count = count;
    out->gateway.activities_truncated = truncated;
    out->presence.activities = activities;
    out->presence.activities_count = count;
    out->presence.activities_truncated = truncated;
    out->has_presence = 1;
    return DCC_OK;
}
