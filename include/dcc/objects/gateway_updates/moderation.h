#ifndef DCC_OBJECTS_GATEWAY_UPDATES_MODERATION_H
#define DCC_OBJECTS_GATEWAY_UPDATES_MODERATION_H

/* Auto moderation gateway update payload objects. */

#include <dcc/objects/resources.h>

typedef struct dcc_auto_moderation_action {
    dcc_snowflake_t guild_id;
    dcc_snowflake_t rule_id;
    dcc_snowflake_t user_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t message_id;
    dcc_snowflake_t alert_system_message_id;
    uint32_t action_type;
    uint32_t trigger_type;
    const char *content;
    const char *matched_keyword;
    const char *matched_content;
} dcc_auto_moderation_action_t;

typedef struct dcc_auto_moderation_rule {
    dcc_snowflake_t id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t creator_id;
    uint32_t event_type;
    uint32_t trigger_type;
    uint8_t enabled;
    const dcc_snowflake_t *role_ids;
    size_t role_ids_count;
    uint8_t role_ids_truncated;
    const dcc_snowflake_t *channel_ids;
    size_t channel_ids_count;
    uint8_t channel_ids_truncated;
    const char *name;
} dcc_auto_moderation_rule_t;

#endif
