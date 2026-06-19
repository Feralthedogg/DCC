#ifndef DCC_REST_TYPES_AUTO_MODERATION_H
#define DCC_REST_TYPES_AUTO_MODERATION_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_auto_moderation_preset_type {
    DCC_AUTO_MODERATION_PRESET_PROFANITY = 1,
    DCC_AUTO_MODERATION_PRESET_SEXUAL_CONTENT = 2,
    DCC_AUTO_MODERATION_PRESET_SLURS = 3
} dcc_auto_moderation_preset_type_t;

typedef enum dcc_auto_moderation_action_type {
    DCC_AUTO_MODERATION_ACTION_BLOCK_MESSAGE = 1,
    DCC_AUTO_MODERATION_ACTION_SEND_ALERT = 2,
    DCC_AUTO_MODERATION_ACTION_TIMEOUT = 3
} dcc_auto_moderation_action_type_t;

typedef enum dcc_auto_moderation_event_type {
    DCC_AUTO_MODERATION_EVENT_MESSAGE_SEND = 1
} dcc_auto_moderation_event_type_t;

typedef enum dcc_auto_moderation_trigger_type {
    DCC_AUTO_MODERATION_TRIGGER_KEYWORD = 1,
    DCC_AUTO_MODERATION_TRIGGER_HARMFUL_LINK = 2,
    DCC_AUTO_MODERATION_TRIGGER_SPAM = 3,
    DCC_AUTO_MODERATION_TRIGGER_KEYWORD_PRESET = 4,
    DCC_AUTO_MODERATION_TRIGGER_MENTION_SPAM = 5
} dcc_auto_moderation_trigger_type_t;

typedef struct dcc_auto_moderation_metadata_params {
    size_t size;
    const char * const *keywords;
    size_t keyword_count;
    const char * const *regex_patterns;
    size_t regex_pattern_count;
    const dcc_auto_moderation_preset_type_t *presets;
    size_t preset_count;
    const char * const *allow_list;
    size_t allow_list_count;
    uint8_t mention_total_limit;
    uint8_t mention_raid_protection_enabled;
} dcc_auto_moderation_metadata_params_t;

typedef struct dcc_auto_moderation_action_params {
    size_t size;
    dcc_auto_moderation_action_type_t type;
    dcc_snowflake_t channel_id;
    const char *custom_message;
    uint32_t duration_seconds;
} dcc_auto_moderation_action_params_t;

typedef struct dcc_auto_moderation_rule_params {
    size_t size;
    dcc_snowflake_t rule_id;
    const char *name;
    dcc_auto_moderation_event_type_t event_type;
    dcc_auto_moderation_trigger_type_t trigger_type;
    dcc_auto_moderation_metadata_params_t trigger_metadata;
    const dcc_auto_moderation_action_params_t *actions;
    size_t action_count;
    const dcc_snowflake_t *exempt_roles;
    size_t exempt_role_count;
    const dcc_snowflake_t *exempt_channels;
    size_t exempt_channel_count;
    uint8_t enabled;
} dcc_auto_moderation_rule_params_t;

#ifdef __cplusplus
}
#endif

#endif
