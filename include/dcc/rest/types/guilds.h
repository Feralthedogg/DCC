#ifndef DCC_REST_TYPES_GUILDS_H
#define DCC_REST_TYPES_GUILDS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_guild_widget_params {
    size_t size;
    dcc_snowflake_t channel_id;
    uint8_t enabled;
} dcc_guild_widget_params_t;

typedef struct dcc_welcome_channel_params {
    dcc_snowflake_t channel_id;
    const char *description;
    const char *emoji_name;
    dcc_snowflake_t emoji_id;
} dcc_welcome_channel_params_t;

typedef struct dcc_welcome_screen_params {
    size_t size;
    const char *description;
    const dcc_welcome_channel_params_t *welcome_channels;
    size_t welcome_channel_count;
    uint8_t enabled;
} dcc_welcome_screen_params_t;

typedef enum dcc_guild_afk_timeout {
    DCC_GUILD_AFK_OFF = 0,
    DCC_GUILD_AFK_60 = 60,
    DCC_GUILD_AFK_300 = 300,
    DCC_GUILD_AFK_900 = 900,
    DCC_GUILD_AFK_1800 = 1800,
    DCC_GUILD_AFK_3600 = 3600
} dcc_guild_afk_timeout_t;

typedef enum dcc_guild_default_message_notification {
    DCC_GUILD_NOTIFY_ALL = 0,
    DCC_GUILD_NOTIFY_ONLY_MENTIONS = 1
} dcc_guild_default_message_notification_t;

typedef enum dcc_guild_explicit_content_filter {
    DCC_GUILD_EXPLICIT_CONTENT_DISABLED = 0,
    DCC_GUILD_EXPLICIT_CONTENT_MEMBERS_WITHOUT_ROLES = 1,
    DCC_GUILD_EXPLICIT_CONTENT_ALL_MEMBERS = 2
} dcc_guild_explicit_content_filter_t;

typedef enum dcc_guild_mfa_level {
    DCC_GUILD_MFA_NONE = 0,
    DCC_GUILD_MFA_ELEVATED = 1
} dcc_guild_mfa_level_t;

typedef struct dcc_guild_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const char *name;
    uint8_t widget_enabled;
    dcc_snowflake_t afk_channel_id;
    dcc_guild_afk_timeout_t afk_timeout;
    dcc_snowflake_t widget_channel_id;
    dcc_guild_default_message_notification_t default_message_notifications;
    dcc_guild_explicit_content_filter_t explicit_content_filter;
    dcc_guild_mfa_level_t mfa_level;
    dcc_snowflake_t system_channel_id;
    uint8_t premium_progress_bar_enabled;
    dcc_snowflake_t rules_channel_id;
    const char *vanity_url_code;
    const char *description;
    dcc_snowflake_t safety_alerts_channel_id;
    const char *banner;
    const char *discovery_splash;
    const char *splash;
    const char *icon;
} dcc_guild_params_t;

#ifdef __cplusplus
}
#endif

#endif
