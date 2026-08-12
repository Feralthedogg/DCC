#ifndef DCC_REST_TYPES_GUILDS_H
#define DCC_REST_TYPES_GUILDS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#define DCC_GUILD_RECORD_INIT(type_, value_) type_ value_
#else
#define DCC_GUILD_RECORD_INIT(type_, value_) (type_) value_
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

typedef enum dcc_guild_verification_level {
  DCC_GUILD_VERIFY_NONE = 0,
  DCC_GUILD_VERIFY_LOW = 1,
  DCC_GUILD_VERIFY_MEDIUM = 2,
  DCC_GUILD_VERIFY_HIGH = 3,
  DCC_GUILD_VERIFY_VERY_HIGH = 4
} dcc_guild_verification_level_t;

typedef enum dcc_rest_guild_widget_style {
  DCC_REST_GUILD_WIDGET_STYLE_SHIELD = 0,
  DCC_REST_GUILD_WIDGET_STYLE_BANNER1 = 1,
  DCC_REST_GUILD_WIDGET_STYLE_BANNER2 = 2,
  DCC_REST_GUILD_WIDGET_STYLE_BANNER3 = 3,
  DCC_REST_GUILD_WIDGET_STYLE_BANNER4 = 4
} dcc_rest_guild_widget_style_t;

enum {
  DCC_REST_GUILD_QUERY_VERSION = 1U,
  DCC_REST_CURRENT_USER_GUILDS_QUERY_VERSION = 1U,
  DCC_REST_GUILD_AUDIT_LOG_QUERY_VERSION = 1U,
  DCC_REST_GUILD_BANS_QUERY_VERSION = 1U,
  DCC_REST_GUILD_PRUNE_QUERY_VERSION = 1U,
  DCC_REST_GUILD_WIDGET_IMAGE_QUERY_VERSION = 1U,
  DCC_REST_GUILD_UPDATE_VERSION = 1U,
  DCC_REST_GUILD_INCIDENT_ACTIONS_VERSION = 1U,
  DCC_REST_GUILD_WIDGET_UPDATE_VERSION = 1U,
  DCC_REST_WELCOME_CHANNEL_VERSION = 1U,
  DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_VERSION = 1U,
  DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_VERSION = 1U,
  DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_VERSION = 1U
};

enum { DCC_REST_GUILD_QUERY_PRESENT_WITH_COUNTS = UINT64_C(1) };
typedef struct dcc_rest_guild_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint8_t with_counts;
} dcc_rest_guild_query_t;
#define DCC_REST_GUILD_QUERY_INIT                                              \
  {sizeof(dcc_rest_guild_query_t), DCC_REST_GUILD_QUERY_VERSION, UINT64_C(0),  \
   0U}
static inline void dcc_rest_guild_query_init(dcc_rest_guild_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_query_t,
                                   DCC_REST_GUILD_QUERY_INIT);
}

enum {
  DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_BEFORE = UINT64_C(1),
  DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_AFTER = UINT64_C(1) << 1U,
  DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_LIMIT = UINT64_C(1) << 2U,
  DCC_REST_CURRENT_USER_GUILDS_QUERY_PRESENT_WITH_COUNTS = UINT64_C(1) << 3U
};
typedef struct dcc_rest_current_user_guilds_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
  uint16_t limit;
  uint8_t with_counts;
} dcc_rest_current_user_guilds_query_t;
#define DCC_REST_CURRENT_USER_GUILDS_QUERY_INIT                                \
  {sizeof(dcc_rest_current_user_guilds_query_t),                               \
   DCC_REST_CURRENT_USER_GUILDS_QUERY_VERSION,                                 \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U}
static inline void dcc_rest_current_user_guilds_query_init(
    dcc_rest_current_user_guilds_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_current_user_guilds_query_t,
                                   DCC_REST_CURRENT_USER_GUILDS_QUERY_INIT);
}

enum {
  DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_USER_ID = UINT64_C(1),
  DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_ACTION_TYPE = UINT64_C(1) << 1U,
  DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_BEFORE = UINT64_C(1) << 2U,
  DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_AFTER = UINT64_C(1) << 3U,
  DCC_REST_GUILD_AUDIT_LOG_QUERY_PRESENT_LIMIT = UINT64_C(1) << 4U
};
typedef struct dcc_rest_guild_audit_log_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t user_id;
  uint32_t action_type;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
  uint16_t limit;
} dcc_rest_guild_audit_log_query_t;
#define DCC_REST_GUILD_AUDIT_LOG_QUERY_INIT                                    \
  {sizeof(dcc_rest_guild_audit_log_query_t),                                   \
   DCC_REST_GUILD_AUDIT_LOG_QUERY_VERSION,                                     \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U,                                                                         \
   0U}
static inline void
dcc_rest_guild_audit_log_query_init(dcc_rest_guild_audit_log_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_audit_log_query_t,
                                   DCC_REST_GUILD_AUDIT_LOG_QUERY_INIT);
}

enum {
  DCC_REST_GUILD_BANS_QUERY_PRESENT_BEFORE = UINT64_C(1),
  DCC_REST_GUILD_BANS_QUERY_PRESENT_AFTER = UINT64_C(1) << 1U,
  DCC_REST_GUILD_BANS_QUERY_PRESENT_LIMIT = UINT64_C(1) << 2U
};
typedef struct dcc_rest_guild_bans_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_snowflake_t before;
  dcc_snowflake_t after;
  uint16_t limit;
} dcc_rest_guild_bans_query_t;
#define DCC_REST_GUILD_BANS_QUERY_INIT                                         \
  {sizeof(dcc_rest_guild_bans_query_t),                                        \
   DCC_REST_GUILD_BANS_QUERY_VERSION,                                          \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   0U}
static inline void
dcc_rest_guild_bans_query_init(dcc_rest_guild_bans_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_bans_query_t,
                                   DCC_REST_GUILD_BANS_QUERY_INIT);
}

enum {
  DCC_REST_GUILD_PRUNE_QUERY_PRESENT_DAYS = UINT64_C(1),
  DCC_REST_GUILD_PRUNE_QUERY_PRESENT_INCLUDE_ROLES = UINT64_C(1) << 1U
};
typedef struct dcc_rest_guild_prune_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint32_t days;
  const dcc_snowflake_t *include_roles;
  size_t include_role_count;
} dcc_rest_guild_prune_query_t;
#define DCC_REST_GUILD_PRUNE_QUERY_INIT                                        \
  {sizeof(dcc_rest_guild_prune_query_t),                                       \
   DCC_REST_GUILD_PRUNE_QUERY_VERSION,                                         \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   NULL,                                                                       \
   0U}
static inline void
dcc_rest_guild_prune_query_init(dcc_rest_guild_prune_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_prune_query_t,
                                   DCC_REST_GUILD_PRUNE_QUERY_INIT);
}

enum { DCC_REST_GUILD_WIDGET_IMAGE_QUERY_PRESENT_STYLE = UINT64_C(1) };
typedef struct dcc_rest_guild_widget_image_query {
  size_t size;
  uint32_t version;
  uint64_t present;
  dcc_rest_guild_widget_style_t style;
} dcc_rest_guild_widget_image_query_t;
#define DCC_REST_GUILD_WIDGET_IMAGE_QUERY_INIT                                 \
  {sizeof(dcc_rest_guild_widget_image_query_t),                                \
   DCC_REST_GUILD_WIDGET_IMAGE_QUERY_VERSION, UINT64_C(0),                     \
   DCC_REST_GUILD_WIDGET_STYLE_SHIELD}
static inline void dcc_rest_guild_widget_image_query_init(
    dcc_rest_guild_widget_image_query_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_widget_image_query_t,
                                   DCC_REST_GUILD_WIDGET_IMAGE_QUERY_INIT);
}

enum {
  DCC_REST_GUILD_UPDATE_PRESENT_NAME = UINT64_C(1),
  DCC_REST_GUILD_UPDATE_PRESENT_REGION = UINT64_C(1) << 1U,
  DCC_REST_GUILD_UPDATE_PRESENT_VERIFICATION_LEVEL = UINT64_C(1) << 2U,
  DCC_REST_GUILD_UPDATE_PRESENT_DEFAULT_MESSAGE_NOTIFICATIONS = UINT64_C(1)
                                                                << 3U,
  DCC_REST_GUILD_UPDATE_PRESENT_EXPLICIT_CONTENT_FILTER = UINT64_C(1) << 4U,
  DCC_REST_GUILD_UPDATE_PRESENT_AFK_CHANNEL_ID = UINT64_C(1) << 5U,
  DCC_REST_GUILD_UPDATE_PRESENT_AFK_TIMEOUT = UINT64_C(1) << 6U,
  DCC_REST_GUILD_UPDATE_PRESENT_ICON = UINT64_C(1) << 7U,
  DCC_REST_GUILD_UPDATE_PRESENT_SPLASH = UINT64_C(1) << 8U,
  DCC_REST_GUILD_UPDATE_PRESENT_DISCOVERY_SPLASH = UINT64_C(1) << 9U,
  DCC_REST_GUILD_UPDATE_PRESENT_BANNER = UINT64_C(1) << 10U,
  DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_ID = UINT64_C(1) << 11U,
  DCC_REST_GUILD_UPDATE_PRESENT_SYSTEM_CHANNEL_FLAGS = UINT64_C(1) << 12U,
  DCC_REST_GUILD_UPDATE_PRESENT_RULES_CHANNEL_ID = UINT64_C(1) << 13U,
  DCC_REST_GUILD_UPDATE_PRESENT_PUBLIC_UPDATES_CHANNEL_ID = UINT64_C(1) << 14U,
  DCC_REST_GUILD_UPDATE_PRESENT_PREFERRED_LOCALE = UINT64_C(1) << 15U,
  DCC_REST_GUILD_UPDATE_PRESENT_FEATURES = UINT64_C(1) << 16U,
  DCC_REST_GUILD_UPDATE_PRESENT_DESCRIPTION = UINT64_C(1) << 17U,
  DCC_REST_GUILD_UPDATE_PRESENT_PREMIUM_PROGRESS_BAR_ENABLED = UINT64_C(1)
                                                               << 18U,
  DCC_REST_GUILD_UPDATE_PRESENT_SAFETY_ALERTS_CHANNEL_ID = UINT64_C(1) << 19U
};
typedef struct dcc_rest_guild_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *name;
  const char *region;
  dcc_guild_verification_level_t verification_level;
  dcc_guild_default_message_notification_t default_message_notifications;
  dcc_guild_explicit_content_filter_t explicit_content_filter;
  dcc_snowflake_t afk_channel_id;
  dcc_guild_afk_timeout_t afk_timeout;
  const char *icon;
  const char *splash;
  const char *discovery_splash;
  const char *banner;
  dcc_snowflake_t system_channel_id;
  uint64_t system_channel_flags;
  dcc_snowflake_t rules_channel_id;
  dcc_snowflake_t public_updates_channel_id;
  const char *preferred_locale;
  const char *const *features;
  size_t feature_count;
  const char *description;
  uint8_t premium_progress_bar_enabled;
  dcc_snowflake_t safety_alerts_channel_id;
} dcc_rest_guild_update_t;
#define DCC_REST_GUILD_UPDATE_INIT                                             \
  {sizeof(dcc_rest_guild_update_t),                                            \
   DCC_REST_GUILD_UPDATE_VERSION,                                              \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL,                                                                       \
   DCC_GUILD_VERIFY_NONE,                                                      \
   DCC_GUILD_NOTIFY_ALL,                                                       \
   DCC_GUILD_EXPLICIT_CONTENT_DISABLED,                                        \
   0U,                                                                         \
   DCC_GUILD_AFK_OFF,                                                          \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U,                                                                         \
   NULL,                                                                       \
   NULL,                                                                       \
   0U,                                                                         \
   NULL,                                                                       \
   0U,                                                                         \
   0U}
static inline void dcc_rest_guild_update_init(dcc_rest_guild_update_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_update_t,
                                   DCC_REST_GUILD_UPDATE_INIT);
}

enum {
  DCC_REST_GUILD_INCIDENT_ACTIONS_PRESENT_INVITES_DISABLED_UNTIL = UINT64_C(1),
  DCC_REST_GUILD_INCIDENT_ACTIONS_PRESENT_DMS_DISABLED_UNTIL = UINT64_C(1) << 1U
};
typedef struct dcc_rest_guild_incident_actions {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *invites_disabled_until;
  const char *dms_disabled_until;
} dcc_rest_guild_incident_actions_t;
#define DCC_REST_GUILD_INCIDENT_ACTIONS_INIT                                   \
  {sizeof(dcc_rest_guild_incident_actions_t),                                  \
   DCC_REST_GUILD_INCIDENT_ACTIONS_VERSION,                                    \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   NULL}
static inline void
dcc_rest_guild_incident_actions_init(dcc_rest_guild_incident_actions_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_incident_actions_t,
                                   DCC_REST_GUILD_INCIDENT_ACTIONS_INIT);
}

enum {
  DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_ENABLED = UINT64_C(1),
  DCC_REST_GUILD_WIDGET_UPDATE_PRESENT_CHANNEL_ID = UINT64_C(1) << 1U
};
typedef struct dcc_rest_guild_widget_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  uint8_t enabled;
  dcc_snowflake_t channel_id;
} dcc_rest_guild_widget_update_t;
#define DCC_REST_GUILD_WIDGET_UPDATE_INIT                                      \
  {sizeof(dcc_rest_guild_widget_update_t),                                     \
   DCC_REST_GUILD_WIDGET_UPDATE_VERSION,                                       \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   0U}
static inline void
dcc_rest_guild_widget_update_init(dcc_rest_guild_widget_update_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_widget_update_t,
                                   DCC_REST_GUILD_WIDGET_UPDATE_INIT);
}

enum {
  DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_ID = UINT64_C(1),
  DCC_REST_WELCOME_CHANNEL_PRESENT_EMOJI_NAME = UINT64_C(1) << 1U
};
typedef struct dcc_rest_welcome_channel {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  dcc_snowflake_t channel_id;
  const char *description;
  dcc_snowflake_t emoji_id;
  const char *emoji_name;
} dcc_rest_welcome_channel_t;
#define DCC_REST_WELCOME_CHANNEL_INIT(channel_id_, description_)               \
  {sizeof(dcc_rest_welcome_channel_t),                                         \
   DCC_REST_WELCOME_CHANNEL_VERSION,                                           \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   (channel_id_),                                                              \
   (description_),                                                             \
   0U,                                                                         \
   NULL}
static inline void
dcc_rest_welcome_channel_init(dcc_rest_welcome_channel_t *value,
                              dcc_snowflake_t channel_id,
                              const char *description) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(
        dcc_rest_welcome_channel_t,
        DCC_REST_WELCOME_CHANNEL_INIT(channel_id, description));
}

enum {
  DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_ENABLED = UINT64_C(1),
  DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_WELCOME_CHANNELS = UINT64_C(1)
                                                                  << 1U,
  DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_PRESENT_DESCRIPTION = UINT64_C(1) << 2U
};
typedef struct dcc_rest_guild_welcome_screen_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  uint8_t enabled;
  const dcc_rest_welcome_channel_t *welcome_channels;
  size_t welcome_channel_count;
  const char *description;
} dcc_rest_guild_welcome_screen_update_t;
#define DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT                              \
  {sizeof(dcc_rest_guild_welcome_screen_update_t),                             \
   DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_VERSION,                               \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   0U,                                                                         \
   NULL,                                                                       \
   0U,                                                                         \
   NULL}
static inline void dcc_rest_guild_welcome_screen_update_init(
    dcc_rest_guild_welcome_screen_update_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_welcome_screen_update_t,
                                   DCC_REST_GUILD_WELCOME_SCREEN_UPDATE_INIT);
}

enum {
  DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_VOLUME = UINT64_C(1),
  DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_EMOJI_ID = UINT64_C(1) << 1U,
  DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_PRESENT_EMOJI_NAME = UINT64_C(1) << 2U
};
typedef struct dcc_rest_guild_soundboard_sound_create {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *name;
  const char *sound;
  double volume;
  dcc_snowflake_t emoji_id;
  const char *emoji_name;
} dcc_rest_guild_soundboard_sound_create_t;
#define DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_INIT(name_, sound_)             \
  {sizeof(dcc_rest_guild_soundboard_sound_create_t),                           \
   DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_VERSION,                             \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   (name_),                                                                    \
   (sound_),                                                                   \
   0.0,                                                                        \
   0U,                                                                         \
   NULL}
static inline void dcc_rest_guild_soundboard_sound_create_init(
    dcc_rest_guild_soundboard_sound_create_t *value, const char *name,
    const char *sound) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(
        dcc_rest_guild_soundboard_sound_create_t,
        DCC_REST_GUILD_SOUNDBOARD_SOUND_CREATE_INIT(name, sound));
}

enum {
  DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_NAME = UINT64_C(1),
  DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_VOLUME = UINT64_C(1) << 1U,
  DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_EMOJI_ID = UINT64_C(1) << 2U,
  DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_PRESENT_EMOJI_NAME = UINT64_C(1) << 3U
};
typedef struct dcc_rest_guild_soundboard_sound_update {
  size_t size;
  uint32_t version;
  uint64_t present;
  uint64_t nulls;
  const char *name;
  double volume;
  dcc_snowflake_t emoji_id;
  const char *emoji_name;
} dcc_rest_guild_soundboard_sound_update_t;
#define DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_INIT                            \
  {sizeof(dcc_rest_guild_soundboard_sound_update_t),                           \
   DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_VERSION,                             \
   UINT64_C(0),                                                                \
   UINT64_C(0),                                                                \
   NULL,                                                                       \
   0.0,                                                                        \
   0U,                                                                         \
   NULL}
static inline void dcc_rest_guild_soundboard_sound_update_init(
    dcc_rest_guild_soundboard_sound_update_t *value) {
  if (value != NULL)
    *value = DCC_GUILD_RECORD_INIT(dcc_rest_guild_soundboard_sound_update_t,
                                   DCC_REST_GUILD_SOUNDBOARD_SOUND_UPDATE_INIT);
}

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

#undef DCC_GUILD_RECORD_INIT

#endif
