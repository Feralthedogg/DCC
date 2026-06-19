#ifndef DCC_JSON_GATEWAY_ACTIVITY_OBJECT_BUFFERS_H
#define DCC_JSON_GATEWAY_ACTIVITY_OBJECT_BUFFERS_H

#include <stddef.h>

typedef struct dcc_json_gateway_activity_buffers {
    char *name;
    size_t name_cap;
    char *state;
    size_t state_cap;
    char *details;
    size_t details_cap;
    char *url;
    size_t url_cap;
    char *emoji_name;
    size_t emoji_name_cap;
} dcc_json_gateway_activity_buffers_t;

#endif
