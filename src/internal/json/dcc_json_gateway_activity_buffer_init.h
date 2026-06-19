#ifndef DCC_JSON_GATEWAY_ACTIVITY_BUFFER_INIT_H
#define DCC_JSON_GATEWAY_ACTIVITY_BUFFER_INIT_H

#include "internal/json/dcc_json_core.h"
#include "internal/json/dcc_json_gateway_activity_buffers.h"

#include <stddef.h>

dcc_status_t dcc_json_gateway_init_activity_buffers(
    dcc_json_gateway_activity_buffers_t *buffers,
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
);

#endif
