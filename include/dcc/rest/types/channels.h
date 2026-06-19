#ifndef DCC_REST_TYPES_CHANNELS_H
#define DCC_REST_TYPES_CHANNELS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_channel_position {
    dcc_snowflake_t channel_id;
    uint32_t position;
    dcc_snowflake_t parent_id;
    uint8_t lock_permissions;
} dcc_channel_position_t;

typedef struct dcc_channel_positions_params {
    size_t size;
    dcc_snowflake_t guild_id;
    const dcc_channel_position_t *positions;
    size_t position_count;
} dcc_channel_positions_params_t;

typedef struct dcc_channel_voice_status_params {
    size_t size;
    dcc_snowflake_t channel_id;
    const char *status;
} dcc_channel_voice_status_params_t;

typedef struct dcc_dm_channel_params {
    size_t size;
    dcc_snowflake_t user_id;
} dcc_dm_channel_params_t;

typedef struct dcc_group_dm_recipient_params {
    size_t size;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t user_id;
    const char *access_token;
    const char *nick;
} dcc_group_dm_recipient_params_t;

typedef enum dcc_channel_type {
    DCC_CHANNEL_TEXT = 0,
    DCC_CHANNEL_DM = 1,
    DCC_CHANNEL_VOICE = 2,
    DCC_CHANNEL_GROUP_DM = 3,
    DCC_CHANNEL_CATEGORY = 4,
    DCC_CHANNEL_ANNOUNCEMENT = 5,
    DCC_CHANNEL_STORE = 6,
    DCC_CHANNEL_ANNOUNCEMENT_THREAD = 10,
    DCC_CHANNEL_PUBLIC_THREAD = 11,
    DCC_CHANNEL_PRIVATE_THREAD = 12,
    DCC_CHANNEL_STAGE = 13,
    DCC_CHANNEL_DIRECTORY = 14,
    DCC_CHANNEL_FORUM = 15,
    DCC_CHANNEL_MEDIA = 16
} dcc_channel_type_t;

typedef enum dcc_channel_permission_overwrite_type {
    DCC_CHANNEL_OVERWRITE_ROLE = 0,
    DCC_CHANNEL_OVERWRITE_MEMBER = 1
} dcc_channel_permission_overwrite_type_t;

typedef enum dcc_channel_auto_archive_duration {
    DCC_CHANNEL_AUTO_ARCHIVE_1_HOUR = 60,
    DCC_CHANNEL_AUTO_ARCHIVE_1_DAY = 1440,
    DCC_CHANNEL_AUTO_ARCHIVE_3_DAYS = 4320,
    DCC_CHANNEL_AUTO_ARCHIVE_1_WEEK = 10080
} dcc_channel_auto_archive_duration_t;

typedef enum dcc_channel_forum_layout {
    DCC_CHANNEL_FORUM_LAYOUT_NOT_SET = 0,
    DCC_CHANNEL_FORUM_LAYOUT_LIST_VIEW = 1,
    DCC_CHANNEL_FORUM_LAYOUT_GALLERY_VIEW = 2
} dcc_channel_forum_layout_t;

typedef struct dcc_channel_permission_overwrite {
    dcc_snowflake_t id;
    uint64_t allow;
    uint64_t deny;
    dcc_channel_permission_overwrite_type_t type;
} dcc_channel_permission_overwrite_t;

typedef struct dcc_channel_forum_tag_params {
    size_t size;
    const char *name;
    dcc_snowflake_t emoji_id;
    const char *emoji_name;
    uint8_t moderated;
} dcc_channel_forum_tag_params_t;

typedef struct dcc_channel_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t channel_id;
    dcc_channel_type_t type;
    const char *name;
    const char *topic;
    const dcc_channel_permission_overwrite_t *permission_overwrites;
    size_t permission_overwrite_count;
    uint16_t position;
    uint16_t rate_limit_per_user;
    uint16_t default_thread_rate_limit_per_user;
    dcc_snowflake_t parent_id;
    uint8_t nsfw;
    dcc_channel_auto_archive_duration_t default_auto_archive_duration;
    uint8_t user_limit;
    uint16_t bitrate_kbps;
    uint8_t default_sort_order;
    uint8_t lock_permissions;
    const dcc_channel_forum_tag_params_t *available_tags;
    size_t available_tag_count;
    dcc_snowflake_t default_reaction_emoji_id;
    const char *default_reaction_emoji_name;
    dcc_channel_forum_layout_t default_forum_layout;
    uint8_t require_tag;
    uint8_t hide_media_download_options;
} dcc_channel_params_t;

#ifdef __cplusplus
}
#endif

#endif
