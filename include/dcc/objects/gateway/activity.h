#ifndef DCC_OBJECTS_GATEWAY_ACTIVITY_H
#define DCC_OBJECTS_GATEWAY_ACTIVITY_H

/* Gateway presence activity object. */

#include <dcc/objects/base.h>

typedef struct dcc_gateway_activity {
    dcc_snowflake_t application_id;
    dcc_snowflake_t emoji_id;
    uint64_t created_at;
    uint64_t flags;
    uint32_t type;
    const char *name;
    const char *state;
    const char *details;
    const char *url;
    const char *emoji_name;
} dcc_gateway_activity_t;

#endif
