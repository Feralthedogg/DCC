#ifndef DCC_OBJECTS_INTERACTIONS_H
#define DCC_OBJECTS_INTERACTIONS_H

/* This header is part of the DCC public object model. */

#include <dcc/objects/resources.h>

#define DCC_INTERACTION_DEFAULT_ATTACHMENT_SIZE_LIMIT (10U * 1024U * 1024U)

typedef struct dcc_interaction_resolved_permission {
    dcc_snowflake_t user_id;
    dcc_permission_t permissions;
} dcc_interaction_resolved_permission_t;

typedef struct dcc_interaction_authorizing_integration_owner {
    uint32_t type;
    dcc_snowflake_t owner_id;
} dcc_interaction_authorizing_integration_owner_t;

typedef enum dcc_interaction_option_value_type {
    DCC_INTERACTION_OPTION_VALUE_NONE = 0,
    DCC_INTERACTION_OPTION_VALUE_STRING,
    DCC_INTERACTION_OPTION_VALUE_INTEGER,
    DCC_INTERACTION_OPTION_VALUE_NUMBER,
    DCC_INTERACTION_OPTION_VALUE_BOOLEAN,
    DCC_INTERACTION_OPTION_VALUE_SNOWFLAKE
} dcc_interaction_option_value_type_t;

typedef struct dcc_interaction_option {
    const char *name;
    uint32_t type;
    dcc_interaction_option_value_type_t value_type;
    const char *string_value;
    int64_t integer_value;
    double number_value;
    dcc_snowflake_t snowflake_value;
    uint8_t boolean_value;
    uint8_t focused;
    const struct dcc_interaction_option *options;
    size_t options_count;
    uint8_t options_truncated;
} dcc_interaction_option_t;

typedef struct dcc_interaction_resolved {
    const dcc_user_t *users;
    size_t users_count;
    uint8_t users_truncated;
    const dcc_member_t *members;
    size_t members_count;
    uint8_t members_truncated;
    const dcc_role_t *roles;
    size_t roles_count;
    uint8_t roles_truncated;
    const dcc_channel_t *channels;
    size_t channels_count;
    uint8_t channels_truncated;
    const dcc_message_t *messages;
    size_t messages_count;
    uint8_t messages_truncated;
    const dcc_attachment_t *attachments;
    size_t attachments_count;
    uint8_t attachments_truncated;
    const dcc_interaction_resolved_permission_t *member_permissions;
    size_t member_permissions_count;
    uint8_t member_permissions_truncated;
} dcc_interaction_resolved_t;

typedef struct dcc_interaction {
    dcc_snowflake_t id;
    dcc_snowflake_t application_id;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t command_id;
    dcc_snowflake_t target_id;
    dcc_snowflake_t message_id;
    dcc_permission_t app_permissions;
    uint32_t attachment_size_limit;
    uint32_t version;
    uint32_t context;
    uint32_t type;
    uint32_t command_type;
    uint32_t component_type;
    uint8_t has_context;
    const char *token;
    const char *name;
    const char *custom_id;
    const char *locale;
    const char *guild_locale;
    const char **values;
    size_t values_count;
    uint8_t values_truncated;
    const dcc_interaction_option_t *options;
    size_t options_count;
    uint8_t options_truncated;
    const dcc_interaction_authorizing_integration_owner_t *authorizing_integration_owners;
    size_t authorizing_integration_owners_count;
    uint8_t authorizing_integration_owners_truncated;
    const dcc_entitlement_t *entitlements;
    size_t entitlements_count;
    uint8_t entitlements_truncated;
    const dcc_interaction_option_t *focused_option;
    dcc_interaction_resolved_t resolved;
} dcc_interaction_t;

#endif
