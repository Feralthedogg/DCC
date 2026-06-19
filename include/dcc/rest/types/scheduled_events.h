#ifndef DCC_REST_TYPES_SCHEDULED_EVENTS_H
#define DCC_REST_TYPES_SCHEDULED_EVENTS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_scheduled_event_privacy_level {
    DCC_SCHEDULED_EVENT_PRIVACY_GUILD_ONLY = 2
} dcc_scheduled_event_privacy_level_t;

typedef enum dcc_scheduled_event_entity_type {
    DCC_SCHEDULED_EVENT_ENTITY_STAGE_INSTANCE = 1,
    DCC_SCHEDULED_EVENT_ENTITY_VOICE = 2,
    DCC_SCHEDULED_EVENT_ENTITY_EXTERNAL = 3
} dcc_scheduled_event_entity_type_t;

typedef enum dcc_scheduled_event_status {
    DCC_SCHEDULED_EVENT_STATUS_SCHEDULED = 1,
    DCC_SCHEDULED_EVENT_STATUS_ACTIVE = 2,
    DCC_SCHEDULED_EVENT_STATUS_COMPLETED = 3,
    DCC_SCHEDULED_EVENT_STATUS_CANCELLED = 4
} dcc_scheduled_event_status_t;

typedef struct dcc_scheduled_event_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t event_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t creator_id;
    dcc_snowflake_t entity_id;
    const char *name;
    const char *description;
    const char *image;
    const char *scheduled_start_time;
    const char *scheduled_end_time;
    const char *location;
    dcc_scheduled_event_privacy_level_t privacy_level;
    dcc_scheduled_event_status_t status;
    dcc_scheduled_event_entity_type_t entity_type;
} dcc_scheduled_event_params_t;

#ifdef __cplusplus
}
#endif

#endif
