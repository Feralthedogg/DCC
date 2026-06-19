#ifndef DCC_OBJECTS_RESOURCES_SCHEDULED_EVENTS_H
#define DCC_OBJECTS_RESOURCES_SCHEDULED_EVENTS_H

/* Guild scheduled event resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_scheduled_event {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t creator_id;
    dcc_snowflake_t entity_id;
    dcc_user_t creator;
    uint32_t privacy_level;
    uint32_t status;
    uint32_t entity_type;
    uint32_t user_count;
    const char *name;
    const char *description;
    const char *image;
    const char *location;
    const char *start_time;
    const char *end_time;
} dcc_scheduled_event_t;

#endif
