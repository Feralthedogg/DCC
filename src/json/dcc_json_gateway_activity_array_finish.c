#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_finish_activity_array(
    const dcc_json_gateway_activity_array_t *array,
    size_t stored,
    uint8_t truncated
) {
    *array->out_activities = array->activities;
    *array->out_count = stored;
    *array->out_truncated = truncated;
}
