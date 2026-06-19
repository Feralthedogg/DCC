#ifndef DCC_JSON_GATEWAY_ACTIVITY_ARRAY_ENTRY_H
#define DCC_JSON_GATEWAY_ACTIVITY_ARRAY_ENTRY_H

#include "internal/json/dcc_json_core.h"

#include <stddef.h>
#include <stdint.h>

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
);

#endif
