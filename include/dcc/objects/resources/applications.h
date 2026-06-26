/**
 * @file include/dcc/objects/resources/applications.h
 * @brief Public DCC objects resources applications declarations.
 */

#ifndef DCC_OBJECTS_RESOURCES_APPLICATIONS_H
#define DCC_OBJECTS_RESOURCES_APPLICATIONS_H

/* Application-adjacent resource objects. */

#include <dcc/objects/base.h>

typedef enum dcc_activity_location_kind {
    DCC_ACTIVITY_LOCATION_GUILD_CHANNEL = 0,
    DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL = 1
} dcc_activity_location_kind_t;

typedef struct dcc_activity_location {
    dcc_snowflake_t channel_id;
    dcc_snowflake_t guild_id;
    dcc_activity_location_kind_t kind;
    uint8_t has_guild_id;
    const char *id;
    const char *kind_text;
} dcc_activity_location_t;

typedef struct dcc_activity_instance {
    dcc_snowflake_t application_id;
    dcc_snowflake_t launch_id;
    dcc_activity_location_t location;
    const dcc_snowflake_t *user_ids;
    size_t user_ids_count;
    uint8_t user_ids_truncated;
    const char *instance_id;
} dcc_activity_instance_t;

#endif
