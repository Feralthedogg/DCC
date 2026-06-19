#ifndef DCC_JSON_GATEWAY_ACTIVITY_ARRAY_BUFFERS_H
#define DCC_JSON_GATEWAY_ACTIVITY_ARRAY_BUFFERS_H

#include <dcc/objects.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_json_gateway_activity_array {
    dcc_gateway_activity_t *activities;
    size_t activities_cap;
    char *names;
    size_t name_stride;
    char *states;
    size_t state_stride;
    char *details;
    size_t detail_stride;
    char *urls;
    size_t url_stride;
    char *emoji_names;
    size_t emoji_name_stride;
    const dcc_gateway_activity_t **out_activities;
    size_t *out_count;
    uint8_t *out_truncated;
} dcc_json_gateway_activity_array_t;

#endif
