#ifndef DCC_OBJECTS_GATEWAY_UPDATES_CHANNELS_H
#define DCC_OBJECTS_GATEWAY_UPDATES_CHANNELS_H

/* Channel, integration, presence, and embedded activity update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_channel_pins_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    const char *last_pin_timestamp;
} dcc_channel_pins_update_t;

typedef struct dcc_typing_start {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t user_id;
    uint64_t timestamp;
    dcc_user_t user;
    dcc_member_t member;
} dcc_typing_start_t;

typedef struct dcc_integration {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t application_id;
    dcc_snowflake_t role_id;
    uint8_t enabled;
    const char *name;
    const char *type_text;
} dcc_integration_t;

typedef struct dcc_presence {
    dcc_snowflake_t guild_id;
    dcc_user_t user;
    uint64_t since;
    const struct dcc_gateway_activity *activities;
    size_t activities_count;
    uint8_t activities_truncated;
    const char *status;
    const char *desktop_status;
    const char *mobile_status;
    const char *web_status;
} dcc_presence_t;

typedef struct dcc_webhooks_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
} dcc_webhooks_update_t;

typedef struct dcc_embedded_activity_update {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t application_id;
} dcc_embedded_activity_update_t;

#endif
