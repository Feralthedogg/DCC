#include "gateway_smoke_handlers.h"

#include <string.h>

void on_guild_create(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    gateway_state_t *state = (gateway_state_t *)user_data;
    const dcc_gateway_event_data_t *data = dcc_event_data(event);
    const dcc_guild_t *guild = dcc_event_guild(event);
    if (data != NULL &&
        guild != NULL &&
        data->owner_id == 446 &&
        data->description != NULL &&
        strcmp(data->description, "guild description") == 0 &&
        data->icon != NULL &&
        strcmp(data->icon, "guild-icon") == 0 &&
        data->banner != NULL &&
        strcmp(data->banner, "guild-banner") == 0 &&
        data->application_id == 666 &&
        data->afk_channel_id == 224 &&
        data->system_channel_id == 225 &&
        data->rules_channel_id == 226 &&
        data->public_updates_channel_id == 227 &&
        data->widget_channel_id == 228 &&
        data->safety_alerts_channel_id == 229 &&
        data->vanity_url_code != NULL &&
        strcmp(data->vanity_url_code, "dcc") == 0 &&
        data->preferred_locale != NULL &&
        strcmp(data->preferred_locale, "en-US") == 0 &&
        data->splash != NULL &&
        strcmp(data->splash, "guild-splash") == 0 &&
        data->discovery_splash != NULL &&
        strcmp(data->discovery_splash, "guild-discovery") == 0 &&
        data->flags == 16 &&
        data->afk_timeout == 300 &&
        data->max_presences == 5000 &&
        data->max_members == 250000 &&
        data->premium_subscription_count == 12 &&
        data->max_video_channel_users == 25 &&
        data->default_message_notifications == 1 &&
        data->premium_tier == 2 &&
        data->verification_level == 3 &&
        data->explicit_content_filter == 2 &&
        data->mfa_level == 1 &&
        data->nsfw_level == 0 &&
        data->widget_enabled == 1 &&
        data->premium_progress_bar_enabled == 1 &&
        guild->id == 333 &&
        guild->name != NULL &&
        strcmp(guild->name, "DCC Guild") == 0 &&
        guild->owner_id == 446 &&
        guild->description != NULL &&
        strcmp(guild->description, "guild description") == 0 &&
        guild->icon != NULL &&
        strcmp(guild->icon, "guild-icon") == 0 &&
        guild->banner != NULL &&
        strcmp(guild->banner, "guild-banner") == 0 &&
        guild->application_id == 666 &&
        guild->afk_channel_id == 224 &&
        guild->system_channel_id == 225 &&
        guild->rules_channel_id == 226 &&
        guild->public_updates_channel_id == 227 &&
        guild->widget_channel_id == 228 &&
        guild->safety_alerts_channel_id == 229 &&
        guild->vanity_url_code != NULL &&
        strcmp(guild->vanity_url_code, "dcc") == 0 &&
        guild->preferred_locale != NULL &&
        strcmp(guild->preferred_locale, "en-US") == 0 &&
        guild->splash != NULL &&
        strcmp(guild->splash, "guild-splash") == 0 &&
        guild->discovery_splash != NULL &&
        strcmp(guild->discovery_splash, "guild-discovery") == 0 &&
        guild->flags == 16 &&
        guild->afk_timeout == 300 &&
        guild->max_presences == 5000 &&
        guild->max_members == 250000 &&
        guild->premium_subscription_count == 12 &&
        guild->max_video_channel_users == 25 &&
        guild->default_message_notifications == 1 &&
        guild->premium_tier == 2 &&
        guild->verification_level == 3 &&
        guild->explicit_content_filter == 2 &&
        guild->mfa_level == 1 &&
        guild->nsfw_level == 0 &&
        guild->widget_enabled == 1 &&
        guild->premium_progress_bar_enabled == 1 &&
        guild->member_count == 42 &&
        guild->large == 0 &&
        guild->unavailable == 0 &&
        guild->roles_count == 1 &&
        guild->roles[0].id == 555 &&
        guild->roles[0].guild_id == 333 &&
        guild->roles[0].name != NULL &&
        strcmp(guild->roles[0].name, "mods") == 0 &&
        guild->roles[0].permissions == 1024 &&
        guild->channels_count == 1 &&
        guild->channels[0].id == 222 &&
        guild->channels[0].guild_id == 333 &&
        guild->channels[0].name != NULL &&
        strcmp(guild->channels[0].name, "general") == 0 &&
        guild->channels[0].topic != NULL &&
        strcmp(guild->channels[0].topic, "chat") == 0 &&
        guild->threads_count == 1 &&
        guild->threads[0].id == 888 &&
        guild->threads[0].guild_id == 333 &&
        guild->threads[0].parent_id == 222 &&
        guild->threads[0].name != NULL &&
        strcmp(guild->threads[0].name, "thread") == 0 &&
        guild->scheduled_events_count == 1 &&
        guild->scheduled_events[0].id == 889 &&
        guild->scheduled_events[0].guild_id == 333 &&
        guild->scheduled_events[0].channel_id == 222 &&
        guild->scheduled_events[0].name != NULL &&
        strcmp(guild->scheduled_events[0].name, "event") == 0 &&
        guild->scheduled_events[0].description != NULL &&
        strcmp(guild->scheduled_events[0].description, "guild event") == 0 &&
        guild->scheduled_events[0].image != NULL &&
        strcmp(guild->scheduled_events[0].image, "imagehash") == 0 &&
        guild->scheduled_events[0].location != NULL &&
        strcmp(guild->scheduled_events[0].location, "guild hall") == 0 &&
        guild->scheduled_events[0].creator.id == 446 &&
        guild->scheduled_events[0].creator.username != NULL &&
        strcmp(guild->scheduled_events[0].creator.username, "creator") == 0 &&
        guild->scheduled_events[0].creator.avatar != NULL &&
        strcmp(guild->scheduled_events[0].creator.avatar, "creator-avatar") == 0 &&
        guild->scheduled_events[0].creator.flags == 32 &&
        guild->scheduled_events[0].status == 1 &&
        guild->scheduled_events[0].user_count == 5 &&
        guild->scheduled_events[0].start_time != NULL &&
        strcmp(guild->scheduled_events[0].start_time, "2026-06-15T00:00:00.000000+00:00") == 0 &&
        guild->stage_instances != NULL &&
        guild->stage_instances_count == 1 &&
        guild->stage_instances[0].id == 890 &&
        guild->stage_instances[0].guild_id == 333 &&
        guild->stage_instances[0].channel_id == 222 &&
        guild->stage_instances[0].scheduled_event_id == 889 &&
        guild->stage_instances[0].privacy_level == 2 &&
        guild->stage_instances[0].discoverable_disabled == 1 &&
        guild->stage_instances[0].topic != NULL &&
        strcmp(guild->stage_instances[0].topic, "stage topic") == 0 &&
        guild->presences != NULL &&
        guild->presences_count == 1 &&
        guild->presences[0].guild_id == 333 &&
        guild->presences[0].user.id == 447 &&
        guild->presences[0].user.username != NULL &&
        strcmp(guild->presences[0].user.username, "watcher") == 0 &&
        guild->presences[0].user.avatar != NULL &&
        strcmp(guild->presences[0].user.avatar, "watcher-avatar") == 0 &&
        guild->presences[0].user.flags == 64 &&
        guild->presences[0].status != NULL &&
        strcmp(guild->presences[0].status, "idle") == 0 &&
        guild->presences[0].since == 654321 &&
        guild->presences[0].desktop_status != NULL &&
        strcmp(guild->presences[0].desktop_status, "idle") == 0 &&
        guild->presences[0].mobile_status != NULL &&
        strcmp(guild->presences[0].mobile_status, "online") == 0 &&
        guild->presences[0].web_status != NULL &&
        strcmp(guild->presences[0].web_status, "offline") == 0 &&
        guild->presences[0].activities != NULL &&
        guild->presences[0].activities_count == 1 &&
        guild->presences[0].activities[0].name != NULL &&
        strcmp(guild->presences[0].activities[0].name, "review") == 0 &&
        guild->presences[0].activities[0].state != NULL &&
        strcmp(guild->presences[0].activities[0].state, "schema") == 0 &&
        guild->presences[0].activities[0].details != NULL &&
        strcmp(guild->presences[0].activities[0].details, "guild create") == 0 &&
        guild->presences[0].activities[0].created_at == 987654321 &&
        guild->presences[0].activities[0].application_id == 667 &&
        guild->presences[0].activities[0].flags == 2 &&
        guild->presences[0].activities[0].emoji_id == 9998 &&
        guild->presences[0].activities[0].emoji_name != NULL &&
        strcmp(guild->presences[0].activities[0].emoji_name, "eyes") == 0 &&
        guild->emojis_count == 1 &&
        guild->emojis[0].id == 9999 &&
        guild->emojis[0].name != NULL &&
        strcmp(guild->emojis[0].name, "spark") == 0 &&
        guild->emojis[0].user.id == 446 &&
        guild->emojis[0].user.avatar != NULL &&
        strcmp(guild->emojis[0].user.avatar, "emoji-user-avatar") == 0 &&
        guild->emojis[0].user.flags == 128 &&
        guild->stickers_count == 1 &&
        guild->stickers[0].id == 9010 &&
        guild->stickers[0].guild_id == 333 &&
        guild->stickers[0].name != NULL &&
        strcmp(guild->stickers[0].name, "sticker") == 0 &&
        guild->stickers[0].user.id == 446 &&
        guild->stickers[0].user.avatar != NULL &&
        strcmp(guild->stickers[0].user.avatar, "sticker-user-avatar") == 0 &&
        guild->stickers[0].user.flags == 256 &&
        guild->stickers[0].description != NULL &&
        strcmp(guild->stickers[0].description, "desc") == 0 &&
        guild->stickers[0].tags != NULL &&
        strcmp(guild->stickers[0].tags, "tag") == 0 &&
        guild->soundboard_sounds_count == 1 &&
        guild->soundboard_sounds[0].id == 5678 &&
        guild->soundboard_sounds[0].guild_id == 333 &&
        guild->soundboard_sounds[0].name != NULL &&
        strcmp(guild->soundboard_sounds[0].name, "spark") == 0 &&
        guild->soundboard_sounds[0].user_id == 446 &&
        guild->soundboard_sounds[0].user.avatar != NULL &&
        strcmp(guild->soundboard_sounds[0].user.avatar, "sound-user-avatar") == 0 &&
        guild->soundboard_sounds[0].user.flags == 512 &&
        guild->soundboard_sounds[0].emoji_name != NULL &&
        strcmp(guild->soundboard_sounds[0].emoji_name, "spark") == 0) {
        dcc_gateway_event_data_t *data_copy = dcc_gateway_event_data_clone(data);
        dcc_guild_t *copy = dcc_guild_clone(guild);
        if (data_copy != NULL &&
            data_copy->description != NULL &&
            strcmp(data_copy->description, "guild description") == 0 &&
            data_copy->icon != NULL &&
            strcmp(data_copy->icon, "guild-icon") == 0 &&
            data_copy->banner != NULL &&
            strcmp(data_copy->banner, "guild-banner") == 0 &&
            data_copy->vanity_url_code != NULL &&
            strcmp(data_copy->vanity_url_code, "dcc") == 0 &&
            data_copy->preferred_locale != NULL &&
            strcmp(data_copy->preferred_locale, "en-US") == 0 &&
            data_copy->splash != NULL &&
            strcmp(data_copy->splash, "guild-splash") == 0 &&
            data_copy->discovery_splash != NULL &&
            strcmp(data_copy->discovery_splash, "guild-discovery") == 0 &&
            copy != NULL &&
            copy->name != NULL &&
            strcmp(copy->name, "DCC Guild") == 0 &&
            copy->description != NULL &&
            strcmp(copy->description, "guild description") == 0 &&
            copy->icon != NULL &&
            strcmp(copy->icon, "guild-icon") == 0 &&
            copy->banner != NULL &&
            strcmp(copy->banner, "guild-banner") == 0 &&
            copy->vanity_url_code != NULL &&
            strcmp(copy->vanity_url_code, "dcc") == 0 &&
            copy->preferred_locale != NULL &&
            strcmp(copy->preferred_locale, "en-US") == 0 &&
            copy->splash != NULL &&
            strcmp(copy->splash, "guild-splash") == 0 &&
            copy->discovery_splash != NULL &&
            strcmp(copy->discovery_splash, "guild-discovery") == 0 &&
            copy->roles_count == 1 &&
            copy->roles[0].name != NULL &&
            strcmp(copy->roles[0].name, "mods") == 0 &&
            copy->channels_count == 1 &&
            copy->channels[0].name != NULL &&
            strcmp(copy->channels[0].name, "general") == 0 &&
            copy->scheduled_events_count == 1 &&
            copy->scheduled_events[0].description != NULL &&
            strcmp(copy->scheduled_events[0].description, "guild event") == 0 &&
            copy->scheduled_events[0].image != NULL &&
            strcmp(copy->scheduled_events[0].image, "imagehash") == 0 &&
            copy->scheduled_events[0].location != NULL &&
            strcmp(copy->scheduled_events[0].location, "guild hall") == 0 &&
            copy->scheduled_events[0].creator.username != NULL &&
            strcmp(copy->scheduled_events[0].creator.username, "creator") == 0 &&
            copy->scheduled_events[0].creator.avatar != NULL &&
            strcmp(copy->scheduled_events[0].creator.avatar, "creator-avatar") == 0 &&
            copy->scheduled_events[0].creator.flags == 32 &&
            copy->scheduled_events[0].status == 1 &&
            copy->scheduled_events[0].user_count == 5 &&
            copy->stage_instances != NULL &&
            copy->stage_instances_count == 1 &&
            copy->stage_instances[0].topic != NULL &&
            strcmp(copy->stage_instances[0].topic, "stage topic") == 0 &&
            copy->stage_instances[0].scheduled_event_id == 889 &&
            copy->presences != NULL &&
            copy->presences_count == 1 &&
            copy->presences[0].user.username != NULL &&
            strcmp(copy->presences[0].user.username, "watcher") == 0 &&
            copy->presences[0].user.avatar != NULL &&
            strcmp(copy->presences[0].user.avatar, "watcher-avatar") == 0 &&
            copy->presences[0].user.flags == 64 &&
            copy->presences[0].desktop_status != NULL &&
            strcmp(copy->presences[0].desktop_status, "idle") == 0 &&
            copy->presences[0].activities != NULL &&
            copy->presences[0].activities_count == 1 &&
            copy->presences[0].activities[0].name != NULL &&
            strcmp(copy->presences[0].activities[0].name, "review") == 0 &&
            copy->presences[0].activities[0].emoji_name != NULL &&
            strcmp(copy->presences[0].activities[0].emoji_name, "eyes") == 0 &&
            copy->emojis_count == 1 &&
            copy->emojis[0].name != NULL &&
            strcmp(copy->emojis[0].name, "spark") == 0 &&
            copy->emojis[0].user.avatar != NULL &&
            strcmp(copy->emojis[0].user.avatar, "emoji-user-avatar") == 0 &&
            copy->emojis[0].user.flags == 128 &&
            copy->stickers_count == 1 &&
            copy->stickers[0].user.avatar != NULL &&
            strcmp(copy->stickers[0].user.avatar, "sticker-user-avatar") == 0 &&
            copy->stickers[0].user.flags == 256 &&
            copy->soundboard_sounds_count == 1 &&
            copy->soundboard_sounds[0].name != NULL &&
            strcmp(copy->soundboard_sounds[0].name, "spark") == 0 &&
            copy->soundboard_sounds[0].user.avatar != NULL &&
            strcmp(copy->soundboard_sounds[0].user.avatar, "sound-user-avatar") == 0 &&
            copy->soundboard_sounds[0].user.flags == 512) {
            state->guild_create_seen = 1;
        }
        dcc_guild_free(copy);
        dcc_gateway_event_data_free(data_copy);
    }
}
