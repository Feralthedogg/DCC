#ifndef DCC_OBJECTS_RESOURCES_GUILDS_H
#define DCC_OBJECTS_RESOURCES_GUILDS_H

/* Guild resource object. */

#include <dcc/objects/base.h>

typedef struct dcc_guild {
    dcc_snowflake_t id;
    dcc_snowflake_t owner_id;
    dcc_snowflake_t application_id;
    dcc_snowflake_t afk_channel_id;
    dcc_snowflake_t system_channel_id;
    dcc_snowflake_t rules_channel_id;
    dcc_snowflake_t public_updates_channel_id;
    dcc_snowflake_t widget_channel_id;
    dcc_snowflake_t safety_alerts_channel_id;
    const char *name;
    const char *description;
    const char *icon;
    const char *banner;
    const char *vanity_url_code;
    const char *preferred_locale;
    const char *splash;
    const char *discovery_splash;
    const struct dcc_role *roles;
    size_t roles_count;
    uint8_t roles_truncated;
    const struct dcc_channel *channels;
    size_t channels_count;
    uint8_t channels_truncated;
    const struct dcc_channel *threads;
    size_t threads_count;
    uint8_t threads_truncated;
    const struct dcc_scheduled_event *scheduled_events;
    size_t scheduled_events_count;
    uint8_t scheduled_events_truncated;
    const struct dcc_stage_instance *stage_instances;
    size_t stage_instances_count;
    uint8_t stage_instances_truncated;
    const struct dcc_presence *presences;
    size_t presences_count;
    uint8_t presences_truncated;
    const struct dcc_emoji *emojis;
    size_t emojis_count;
    uint8_t emojis_truncated;
    const struct dcc_sticker *stickers;
    size_t stickers_count;
    uint8_t stickers_truncated;
    const struct dcc_soundboard_sound *soundboard_sounds;
    size_t soundboard_sounds_count;
    uint8_t soundboard_sounds_truncated;
    uint64_t flags;
    uint32_t member_count;
    uint32_t max_presences;
    uint32_t max_members;
    uint32_t premium_subscription_count;
    uint32_t afk_timeout;
    uint32_t max_video_channel_users;
    uint32_t default_message_notifications;
    uint32_t premium_tier;
    uint32_t verification_level;
    uint32_t explicit_content_filter;
    uint32_t mfa_level;
    uint32_t nsfw_level;
    uint8_t unavailable;
    uint8_t large;
    uint8_t widget_enabled;
    uint8_t premium_progress_bar_enabled;
} dcc_guild_t;

#endif
