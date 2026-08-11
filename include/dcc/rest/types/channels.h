#ifndef DCC_REST_TYPES_CHANNELS_H
#define DCC_REST_TYPES_CHANNELS_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#define DCC_CHANNEL_RECORD_INIT_VALUE(type_, initializer_) type_ initializer_
#else
#define DCC_CHANNEL_RECORD_INIT_VALUE(type_, initializer_) (type_)initializer_
#endif

#define DCC_CHANNEL_PARAMS_VERSION 1U
#define DCC_CHANNEL_POSITION_VERSION 1U
#define DCC_CHANNEL_POSITIONS_PARAMS_VERSION 1U
#define DCC_CHANNEL_PERMISSION_OVERWRITE_VERSION 1U
#define DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION 1U
#define DCC_REST_FOLLOW_NEWS_CHANNEL_VERSION 1U
#define DCC_CHANNEL_VOICE_STATUS_PARAMS_VERSION 1U
#define DCC_DM_CHANNEL_PARAMS_VERSION 1U

typedef enum dcc_channel_type {
    DCC_CHANNEL_TEXT = 0, DCC_CHANNEL_DM = 1, DCC_CHANNEL_VOICE = 2,
    DCC_CHANNEL_GROUP_DM = 3, DCC_CHANNEL_CATEGORY = 4,
    DCC_CHANNEL_ANNOUNCEMENT = 5, DCC_CHANNEL_STORE = 6,
    DCC_CHANNEL_ANNOUNCEMENT_THREAD = 10, DCC_CHANNEL_PUBLIC_THREAD = 11,
    DCC_CHANNEL_PRIVATE_THREAD = 12, DCC_CHANNEL_STAGE = 13,
    DCC_CHANNEL_DIRECTORY = 14, DCC_CHANNEL_FORUM = 15,
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

typedef enum dcc_channel_params_kind {
    DCC_CHANNEL_PARAMS_GUILD = 1,
    DCC_CHANNEL_PARAMS_GROUP_DM = 2,
    DCC_CHANNEL_PARAMS_THREAD = 3
} dcc_channel_params_kind_t;

enum {
    DCC_CHANNEL_FLAG_PINNED = UINT64_C(1) << 1U,
    DCC_CHANNEL_FLAG_REQUIRE_TAG = UINT64_C(1) << 4U,
    DCC_CHANNEL_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS = UINT64_C(1) << 15U
};

enum {
    DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_ALLOW = UINT64_C(1),
    DCC_CHANNEL_PERMISSION_OVERWRITE_PRESENT_DENY = UINT64_C(1) << 1U
};

typedef struct dcc_channel_permission_overwrite {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t id;
    dcc_channel_permission_overwrite_type_t type;
    uint64_t allow;
    uint64_t deny;
} dcc_channel_permission_overwrite_t;

#define DCC_CHANNEL_PERMISSION_OVERWRITE_INIT \
    { sizeof(dcc_channel_permission_overwrite_t), \
      DCC_CHANNEL_PERMISSION_OVERWRITE_VERSION, UINT64_C(0), 0U, \
      DCC_CHANNEL_OVERWRITE_ROLE, UINT64_C(0), UINT64_C(0) }

static inline void dcc_channel_permission_overwrite_init(
    dcc_channel_permission_overwrite_t *value
) { if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_permission_overwrite_t, DCC_CHANNEL_PERMISSION_OVERWRITE_INIT); }

typedef struct dcc_channel_forum_tag_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *name;
    dcc_snowflake_t emoji_id;
    const char *emoji_name;
    uint8_t moderated;
} dcc_channel_forum_tag_params_t;

enum {
    DCC_CHANNEL_FORUM_TAG_PRESENT_NAME = UINT64_C(1),
    DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_ID = UINT64_C(1) << 1U,
    DCC_CHANNEL_FORUM_TAG_PRESENT_EMOJI_NAME = UINT64_C(1) << 2U,
    DCC_CHANNEL_FORUM_TAG_PRESENT_MODERATED = UINT64_C(1) << 3U
};

#define DCC_CHANNEL_FORUM_TAG_PARAMS_INIT \
    { sizeof(dcc_channel_forum_tag_params_t), \
      DCC_CHANNEL_FORUM_TAG_PARAMS_VERSION, UINT64_C(0), NULL, 0U, NULL, 0U }

static inline void dcc_channel_forum_tag_params_init(
    dcc_channel_forum_tag_params_t *value
) { if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_forum_tag_params_t, DCC_CHANNEL_FORUM_TAG_PARAMS_INIT); }

typedef struct dcc_channel_default_reaction {
    dcc_snowflake_t emoji_id;
    const char *emoji_name;
} dcc_channel_default_reaction_t;

enum {
    DCC_CHANNEL_GUILD_PRESENT_NAME = UINT64_C(1),
    DCC_CHANNEL_GUILD_PRESENT_TYPE = UINT64_C(1) << 1U,
    DCC_CHANNEL_GUILD_PRESENT_TOPIC = UINT64_C(1) << 2U,
    DCC_CHANNEL_GUILD_PRESENT_BITRATE = UINT64_C(1) << 3U,
    DCC_CHANNEL_GUILD_PRESENT_USER_LIMIT = UINT64_C(1) << 4U,
    DCC_CHANNEL_GUILD_PRESENT_RATE_LIMIT_PER_USER = UINT64_C(1) << 5U,
    DCC_CHANNEL_GUILD_PRESENT_POSITION = UINT64_C(1) << 6U,
    DCC_CHANNEL_GUILD_PRESENT_PERMISSION_OVERWRITES = UINT64_C(1) << 7U,
    DCC_CHANNEL_GUILD_PRESENT_PARENT_ID = UINT64_C(1) << 8U,
    DCC_CHANNEL_GUILD_PRESENT_NSFW = UINT64_C(1) << 9U,
    DCC_CHANNEL_GUILD_PRESENT_RTC_REGION = UINT64_C(1) << 10U,
    DCC_CHANNEL_GUILD_PRESENT_VIDEO_QUALITY_MODE = UINT64_C(1) << 11U,
    DCC_CHANNEL_GUILD_PRESENT_DEFAULT_AUTO_ARCHIVE_DURATION = UINT64_C(1) << 12U,
    DCC_CHANNEL_GUILD_PRESENT_DEFAULT_REACTION_EMOJI = UINT64_C(1) << 13U,
    DCC_CHANNEL_GUILD_PRESENT_AVAILABLE_TAGS = UINT64_C(1) << 14U,
    DCC_CHANNEL_GUILD_PRESENT_DEFAULT_SORT_ORDER = UINT64_C(1) << 15U,
    DCC_CHANNEL_GUILD_PRESENT_DEFAULT_FORUM_LAYOUT = UINT64_C(1) << 16U,
    DCC_CHANNEL_GUILD_PRESENT_DEFAULT_THREAD_RATE_LIMIT_PER_USER = UINT64_C(1) << 17U,
    DCC_CHANNEL_GUILD_PRESENT_FLAGS = UINT64_C(1) << 18U
};

typedef struct dcc_channel_guild_params {
    uint64_t present;
    const char *name;
    dcc_channel_type_t type;
    const char *topic;
    uint32_t bitrate;
    uint32_t user_limit;
    uint32_t rate_limit_per_user;
    int32_t position;
    const dcc_channel_permission_overwrite_t *permission_overwrites;
    size_t permission_overwrite_count;
    dcc_snowflake_t parent_id;
    uint8_t nsfw;
    const char *rtc_region;
    uint32_t video_quality_mode;
    dcc_channel_auto_archive_duration_t default_auto_archive_duration;
    dcc_channel_default_reaction_t default_reaction_emoji;
    const dcc_channel_forum_tag_params_t *available_tags;
    size_t available_tag_count;
    uint32_t default_sort_order;
    dcc_channel_forum_layout_t default_forum_layout;
    uint32_t default_thread_rate_limit_per_user;
    uint64_t flags;
    uint64_t nulls;
} dcc_channel_guild_params_t;

enum {
    DCC_CHANNEL_GUILD_NULL_PARENT_ID = UINT64_C(1),
    DCC_CHANNEL_GUILD_NULL_DEFAULT_REACTION_EMOJI = UINT64_C(1) << 1U
};

enum {
    DCC_CHANNEL_GROUP_DM_PRESENT_NAME = UINT64_C(1),
    DCC_CHANNEL_GROUP_DM_PRESENT_ICON = UINT64_C(1) << 1U
};
typedef struct dcc_channel_group_dm_params {
    uint64_t present;
    const char *name;
    const char *icon;
} dcc_channel_group_dm_params_t;

enum {
    DCC_CHANNEL_THREAD_PRESENT_NAME = UINT64_C(1),
    DCC_CHANNEL_THREAD_PRESENT_ARCHIVED = UINT64_C(1) << 1U,
    DCC_CHANNEL_THREAD_PRESENT_AUTO_ARCHIVE_DURATION = UINT64_C(1) << 2U,
    DCC_CHANNEL_THREAD_PRESENT_LOCKED = UINT64_C(1) << 3U,
    DCC_CHANNEL_THREAD_PRESENT_INVITABLE = UINT64_C(1) << 4U,
    DCC_CHANNEL_THREAD_PRESENT_RATE_LIMIT_PER_USER = UINT64_C(1) << 5U,
    DCC_CHANNEL_THREAD_PRESENT_FLAGS = UINT64_C(1) << 6U,
    DCC_CHANNEL_THREAD_PRESENT_APPLIED_TAGS = UINT64_C(1) << 7U
};
typedef struct dcc_channel_thread_params {
    uint64_t present;
    const char *name;
    uint8_t archived;
    dcc_channel_auto_archive_duration_t auto_archive_duration;
    uint8_t locked;
    uint8_t invitable;
    uint32_t rate_limit_per_user;
    uint64_t flags;
    const dcc_snowflake_t *applied_tags;
    size_t applied_tag_count;
} dcc_channel_thread_params_t;

typedef struct dcc_channel_params {
    size_t size;
    uint32_t version;
    dcc_channel_params_kind_t kind;
    union {
        dcc_channel_guild_params_t guild;
        dcc_channel_group_dm_params_t group_dm;
        dcc_channel_thread_params_t thread;
    } payload;
} dcc_channel_params_t;

#define DCC_CHANNEL_PARAMS_INIT \
    { sizeof(dcc_channel_params_t), DCC_CHANNEL_PARAMS_VERSION, \
      DCC_CHANNEL_PARAMS_GUILD, \
      { { UINT64_C(0), NULL, DCC_CHANNEL_TEXT, NULL, 0U, 0U, 0U, 0, \
          NULL, 0U, 0U, 0U, NULL, 0U, DCC_CHANNEL_AUTO_ARCHIVE_1_DAY, \
          { 0U, NULL }, NULL, 0U, 0U, DCC_CHANNEL_FORUM_LAYOUT_NOT_SET, \
          0U, UINT64_C(0), UINT64_C(0) } } }

static inline void dcc_channel_params_init(dcc_channel_params_t *value) {
    if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_params_t, DCC_CHANNEL_PARAMS_INIT);
}

enum {
    DCC_CHANNEL_POSITION_PRESENT_POSITION = UINT64_C(1),
    DCC_CHANNEL_POSITION_PRESENT_LOCK_PERMISSIONS = UINT64_C(1) << 1U,
    DCC_CHANNEL_POSITION_PRESENT_PARENT_ID = UINT64_C(1) << 2U,
    DCC_CHANNEL_POSITION_PRESENT_FLAGS = UINT64_C(1) << 3U
};
typedef struct dcc_channel_position {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_snowflake_t channel_id;
    int32_t position;
    uint8_t lock_permissions;
    dcc_snowflake_t parent_id;
    uint64_t flags;
    uint64_t nulls;
} dcc_channel_position_t;

enum { DCC_CHANNEL_POSITION_NULL_PARENT_ID = UINT64_C(1) };

#define DCC_CHANNEL_POSITION_INIT \
    { sizeof(dcc_channel_position_t), DCC_CHANNEL_POSITION_VERSION, \
      UINT64_C(0), 0U, 0, 0U, 0U, UINT64_C(0), UINT64_C(0) }

static inline void dcc_channel_position_init(dcc_channel_position_t *value) {
    if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_position_t, DCC_CHANNEL_POSITION_INIT);
}

typedef struct dcc_channel_positions_params {
    size_t size;
    uint32_t version;
    const dcc_channel_position_t *positions;
    size_t position_count;
} dcc_channel_positions_params_t;

#define DCC_CHANNEL_POSITIONS_PARAMS_INIT \
    { sizeof(dcc_channel_positions_params_t), \
      DCC_CHANNEL_POSITIONS_PARAMS_VERSION, NULL, 0U }

static inline void dcc_channel_positions_params_init(
    dcc_channel_positions_params_t *value
) { if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_positions_params_t, DCC_CHANNEL_POSITIONS_PARAMS_INIT); }

typedef struct dcc_rest_follow_news_channel {
    size_t size;
    uint32_t version;
    dcc_snowflake_t webhook_channel_id;
} dcc_rest_follow_news_channel_t;

#define DCC_REST_FOLLOW_NEWS_CHANNEL_INIT \
    { sizeof(dcc_rest_follow_news_channel_t), \
      DCC_REST_FOLLOW_NEWS_CHANNEL_VERSION, 0U }

static inline void dcc_rest_follow_news_channel_init(
    dcc_rest_follow_news_channel_t *value
) { if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_rest_follow_news_channel_t, DCC_REST_FOLLOW_NEWS_CHANNEL_INIT); }

enum { DCC_CHANNEL_VOICE_STATUS_PRESENT_STATUS = UINT64_C(1) };
typedef struct dcc_channel_voice_status_params {
    size_t size;
    uint32_t version;
    uint64_t present;
    const char *status;
} dcc_channel_voice_status_params_t;

#define DCC_CHANNEL_VOICE_STATUS_PARAMS_INIT \
    { sizeof(dcc_channel_voice_status_params_t), \
      DCC_CHANNEL_VOICE_STATUS_PARAMS_VERSION, UINT64_C(0), NULL }

static inline void dcc_channel_voice_status_params_init(
    dcc_channel_voice_status_params_t *value
) { if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_channel_voice_status_params_t, DCC_CHANNEL_VOICE_STATUS_PARAMS_INIT); }

typedef struct dcc_dm_channel_params {
    size_t size;
    uint32_t version;
    dcc_snowflake_t recipient_id;
} dcc_dm_channel_params_t;

#define DCC_DM_CHANNEL_PARAMS_INIT \
    { sizeof(dcc_dm_channel_params_t), DCC_DM_CHANNEL_PARAMS_VERSION, 0U }

static inline void dcc_dm_channel_params_init(dcc_dm_channel_params_t *value) {
    if (value != NULL) *value = DCC_CHANNEL_RECORD_INIT_VALUE(dcc_dm_channel_params_t, DCC_DM_CHANNEL_PARAMS_INIT);
}

typedef struct dcc_group_dm_recipient_params {
    size_t size;
    dcc_snowflake_t channel_id;
    dcc_snowflake_t user_id;
    const char *access_token;
    const char *nick;
} dcc_group_dm_recipient_params_t;

#ifdef __cplusplus
}
#endif

#undef DCC_CHANNEL_RECORD_INIT_VALUE

#endif
