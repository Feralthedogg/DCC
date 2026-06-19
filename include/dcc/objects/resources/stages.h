#ifndef DCC_OBJECTS_RESOURCES_STAGES_H
#define DCC_OBJECTS_RESOURCES_STAGES_H

/* Stage instance resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_stage_instance {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t scheduled_event_id;
    uint32_t privacy_level;
    uint8_t discoverable_disabled;
    const char *topic;
} dcc_stage_instance_t;

#endif
