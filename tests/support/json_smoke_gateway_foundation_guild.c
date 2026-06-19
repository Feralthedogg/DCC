#include "json_smoke_support.h"

int dcc_json_smoke_gateway_foundation_guild_create(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t payload;
    const char guild_create_input[] =
        "{\"t\":\"GUILD_CREATE\",\"s\":103,\"op\":0,\"d\":{\"id\":\"333\","
        "\"version\":1750090157123,"
        "\"name\":\"DCC Guild\",\"owner_id\":\"446\","
        "\"description\":\"guild description\",\"icon\":\"guild-icon\",\"banner\":\"guild-banner\","
        "\"application_id\":\"666\",\"afk_channel_id\":\"224\","
        "\"system_channel_id\":\"225\",\"rules_channel_id\":\"226\","
        "\"public_updates_channel_id\":\"227\",\"widget_channel_id\":\"228\","
        "\"safety_alerts_channel_id\":\"229\","
        "\"vanity_url_code\":\"dcc\",\"preferred_locale\":\"en-US\","
        "\"splash\":\"guild-splash\",\"discovery_splash\":\"guild-discovery\","
        "\"flags\":16,\"afk_timeout\":300,\"max_presences\":5000,"
        "\"max_members\":250000,\"premium_subscription_count\":12,"
        "\"max_video_channel_users\":25,\"default_message_notifications\":1,"
        "\"premium_tier\":2,\"verification_level\":3,"
        "\"explicit_content_filter\":2,\"mfa_level\":1,\"nsfw_level\":0,"
        "\"widget_enabled\":true,\"premium_progress_bar_enabled\":true,"
        "\"large\":false,\"member_count\":42,\"unavailable\":false,"
        "\"roles\":[{\"id\":\"555\",\"name\":\"mods\",\"permissions\":\"1024\","
        "\"color\":16711680,\"position\":4,\"hoist\":true,"
        "\"managed\":false,\"mentionable\":true}],"
        "\"channels\":[{\"id\":\"222\",\"type\":0,\"name\":\"general\","
        "\"topic\":\"chat\",\"position\":2}],"
        "\"threads\":[{\"id\":\"888\",\"parent_id\":\"222\",\"type\":11,\"name\":\"thread\"}],"
        "\"guild_scheduled_events\":[{\"id\":\"889\",\"channel_id\":\"222\","
        "\"name\":\"event\",\"description\":\"guild event\",\"status\":1,\"user_count\":5,"
        "\"image\":\"imagehash\",\"entity_metadata\":{\"location\":\"guild hall\"},"
        "\"creator\":{\"id\":\"446\",\"username\":\"creator\","
        "\"global_name\":\"Creator\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"creator-avatar\",\"public_flags\":32},"
        "\"scheduled_start_time\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"scheduled_end_time\":null}],"
        "\"stage_instances\":[{\"id\":\"890\",\"channel_id\":\"222\","
        "\"topic\":\"stage topic\",\"privacy_level\":2,"
        "\"discoverable_disabled\":true,\"guild_scheduled_event_id\":\"889\"}],"
        "\"presences\":[{\"user\":{\"id\":\"447\",\"username\":\"watcher\","
        "\"global_name\":\"Watcher\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"watcher-avatar\",\"public_flags\":64},"
        "\"status\":\"idle\",\"since\":654321,"
        "\"client_status\":{\"desktop\":\"idle\",\"mobile\":\"online\",\"web\":\"offline\"},"
        "\"activities\":[{\"name\":\"review\",\"type\":0,\"state\":\"schema\","
        "\"details\":\"guild create\",\"created_at\":987654321,"
        "\"application_id\":\"667\",\"flags\":2,"
        "\"emoji\":{\"id\":\"9998\",\"name\":\"eyes\"}}]}],"
        "\"emojis\":[{\"id\":\"9999\",\"name\":\"spark\","
        "\"user\":{\"id\":\"446\",\"username\":\"emoji-user\","
        "\"avatar\":\"emoji-user-avatar\",\"public_flags\":128}}],"
        "\"stickers\":[{\"id\":\"9010\",\"name\":\"sticker\",\"description\":\"desc\","
        "\"tags\":\"tag\",\"type\":2,\"format_type\":1,"
        "\"user\":{\"id\":\"446\",\"username\":\"sticker-user\","
        "\"avatar\":\"sticker-user-avatar\",\"public_flags\":256}}],"
        "\"soundboard_sounds\":[{\"id\":\"5678\",\"name\":\"spark\",\"volume\":0.75,"
        "\"emoji_id\":\"9999\",\"emoji_name\":\"spark\",\"available\":true,"
        "\"user\":{\"id\":\"446\",\"username\":\"sound-user\","
        "\"avatar\":\"sound-user-avatar\",\"public_flags\":512}}]}}";

    st = dcc_json_parse_gateway_payload(guild_create_input, sizeof(guild_create_input) - 1, &payload);
    if (st != DCC_OK ||
        !payload.has_guild ||
        payload.guild.id != 333 ||
        payload.guild.name == NULL ||
        strcmp(payload.guild.name, "DCC Guild") != 0 ||
        payload.guild.owner_id != 446 ||
        payload.guild.description == NULL ||
        strcmp(payload.guild.description, "guild description") != 0 ||
        payload.guild.icon == NULL ||
        strcmp(payload.guild.icon, "guild-icon") != 0 ||
        payload.guild.banner == NULL ||
        strcmp(payload.guild.banner, "guild-banner") != 0 ||
        payload.guild.application_id != 666 ||
        payload.guild.afk_channel_id != 224 ||
        payload.guild.system_channel_id != 225 ||
        payload.guild.rules_channel_id != 226 ||
        payload.guild.public_updates_channel_id != 227 ||
        payload.guild.widget_channel_id != 228 ||
        payload.guild.safety_alerts_channel_id != 229 ||
        payload.guild.vanity_url_code == NULL ||
        strcmp(payload.guild.vanity_url_code, "dcc") != 0 ||
        payload.guild.preferred_locale == NULL ||
        strcmp(payload.guild.preferred_locale, "en-US") != 0 ||
        payload.guild.splash == NULL ||
        strcmp(payload.guild.splash, "guild-splash") != 0 ||
        payload.guild.discovery_splash == NULL ||
        strcmp(payload.guild.discovery_splash, "guild-discovery") != 0 ||
        payload.guild.flags != 16 ||
        payload.guild.afk_timeout != 300 ||
        payload.guild.max_presences != 5000 ||
        payload.guild.max_members != 250000 ||
        payload.guild.premium_subscription_count != 12 ||
        payload.guild.max_video_channel_users != 25 ||
        payload.guild.default_message_notifications != 1 ||
        payload.guild.premium_tier != 2 ||
        payload.guild.verification_level != 3 ||
        payload.guild.explicit_content_filter != 2 ||
        payload.guild.mfa_level != 1 ||
        payload.guild.nsfw_level != 0 ||
        payload.guild.widget_enabled != 1 ||
        payload.guild.premium_progress_bar_enabled != 1 ||
        payload.gateway.owner_id != 446 ||
        payload.gateway.application_id != 666 ||
        payload.gateway.afk_channel_id != 224 ||
        payload.gateway.system_channel_id != 225 ||
        payload.gateway.rules_channel_id != 226 ||
        payload.gateway.public_updates_channel_id != 227 ||
        payload.gateway.widget_channel_id != 228 ||
        payload.gateway.safety_alerts_channel_id != 229 ||
        payload.gateway.description == NULL ||
        strcmp(payload.gateway.description, "guild description") != 0 ||
        payload.gateway.icon == NULL ||
        strcmp(payload.gateway.icon, "guild-icon") != 0 ||
        payload.gateway.banner == NULL ||
        strcmp(payload.gateway.banner, "guild-banner") != 0 ||
        payload.gateway.vanity_url_code == NULL ||
        strcmp(payload.gateway.vanity_url_code, "dcc") != 0 ||
        payload.gateway.preferred_locale == NULL ||
        strcmp(payload.gateway.preferred_locale, "en-US") != 0 ||
        payload.gateway.splash == NULL ||
        strcmp(payload.gateway.splash, "guild-splash") != 0 ||
        payload.gateway.discovery_splash == NULL ||
        strcmp(payload.gateway.discovery_splash, "guild-discovery") != 0 ||
        payload.gateway.flags != 16 ||
        payload.gateway.afk_timeout != 300 ||
        payload.gateway.max_presences != 5000 ||
        payload.gateway.max_members != 250000 ||
        payload.gateway.premium_subscription_count != 12 ||
        payload.gateway.max_video_channel_users != 25 ||
        payload.gateway.default_message_notifications != 1 ||
        payload.gateway.premium_tier != 2 ||
        payload.gateway.verification_level != 3 ||
        payload.gateway.explicit_content_filter != 2 ||
        payload.gateway.mfa_level != 1 ||
        payload.gateway.nsfw_level != 0 ||
        payload.gateway.widget_enabled != 1 ||
        payload.gateway.premium_progress_bar_enabled != 1 ||
        payload.guild.roles == NULL ||
        payload.guild.roles_count != 1 ||
        payload.guild.roles[0].id != 555 ||
        payload.guild.roles[0].guild_id != 333 ||
        payload.guild.roles[0].name == NULL ||
        strcmp(payload.guild.roles[0].name, "mods") != 0 ||
        payload.guild.channels == NULL ||
        payload.guild.channels_count != 1 ||
        payload.guild.channels[0].id != 222 ||
        payload.guild.channels[0].guild_id != 333 ||
        payload.guild.channels[0].name == NULL ||
        strcmp(payload.guild.channels[0].name, "general") != 0 ||
        payload.guild.threads == NULL ||
        payload.guild.threads_count != 1 ||
        payload.guild.threads[0].id != 888 ||
        payload.guild.threads[0].guild_id != 333 ||
        payload.guild.scheduled_events == NULL ||
        payload.guild.scheduled_events_count != 1 ||
        payload.guild.scheduled_events[0].id != 889 ||
        payload.guild.scheduled_events[0].guild_id != 333 ||
        payload.guild.scheduled_events[0].name == NULL ||
        strcmp(payload.guild.scheduled_events[0].name, "event") != 0 ||
        payload.guild.scheduled_events[0].description == NULL ||
        strcmp(payload.guild.scheduled_events[0].description, "guild event") != 0 ||
        payload.guild.scheduled_events[0].image == NULL ||
        strcmp(payload.guild.scheduled_events[0].image, "imagehash") != 0 ||
        payload.guild.scheduled_events[0].location == NULL ||
        strcmp(payload.guild.scheduled_events[0].location, "guild hall") != 0 ||
        payload.guild.scheduled_events[0].creator.id != 446 ||
        payload.guild.scheduled_events[0].creator.username == NULL ||
        strcmp(payload.guild.scheduled_events[0].creator.username, "creator") != 0 ||
        payload.guild.scheduled_events[0].creator.avatar == NULL ||
        strcmp(payload.guild.scheduled_events[0].creator.avatar, "creator-avatar") != 0 ||
        payload.guild.scheduled_events[0].creator.flags != 32 ||
        payload.guild.scheduled_events[0].status != 1 ||
        payload.guild.scheduled_events[0].user_count != 5 ||
        payload.guild.stage_instances == NULL ||
        payload.guild.stage_instances_count != 1 ||
        payload.guild.stage_instances[0].id != 890 ||
        payload.guild.stage_instances[0].guild_id != 333 ||
        payload.guild.stage_instances[0].channel_id != 222 ||
        payload.guild.stage_instances[0].scheduled_event_id != 889 ||
        payload.guild.stage_instances[0].privacy_level != 2 ||
        payload.guild.stage_instances[0].discoverable_disabled != 1 ||
        payload.guild.stage_instances[0].topic == NULL ||
        strcmp(payload.guild.stage_instances[0].topic, "stage topic") != 0 ||
        payload.guild.presences == NULL ||
        payload.guild.presences_count != 1 ||
        payload.guild.presences[0].guild_id != 333 ||
        payload.guild.presences[0].user.id != 447 ||
        payload.guild.presences[0].user.username == NULL ||
        strcmp(payload.guild.presences[0].user.username, "watcher") != 0 ||
        payload.guild.presences[0].user.avatar == NULL ||
        strcmp(payload.guild.presences[0].user.avatar, "watcher-avatar") != 0 ||
        payload.guild.presences[0].user.flags != 64 ||
        payload.guild.presences[0].status == NULL ||
        strcmp(payload.guild.presences[0].status, "idle") != 0 ||
        payload.guild.presences[0].since != 654321 ||
        payload.guild.presences[0].desktop_status == NULL ||
        strcmp(payload.guild.presences[0].desktop_status, "idle") != 0 ||
        payload.guild.presences[0].mobile_status == NULL ||
        strcmp(payload.guild.presences[0].mobile_status, "online") != 0 ||
        payload.guild.presences[0].web_status == NULL ||
        strcmp(payload.guild.presences[0].web_status, "offline") != 0 ||
        payload.guild.presences[0].activities == NULL ||
        payload.guild.presences[0].activities_count != 1 ||
        payload.guild.presences[0].activities[0].name == NULL ||
        strcmp(payload.guild.presences[0].activities[0].name, "review") != 0 ||
        payload.guild.presences[0].activities[0].state == NULL ||
        strcmp(payload.guild.presences[0].activities[0].state, "schema") != 0 ||
        payload.guild.presences[0].activities[0].details == NULL ||
        strcmp(payload.guild.presences[0].activities[0].details, "guild create") != 0 ||
        payload.guild.presences[0].activities[0].created_at != 987654321 ||
        payload.guild.presences[0].activities[0].application_id != 667 ||
        payload.guild.presences[0].activities[0].flags != 2 ||
        payload.guild.presences[0].activities[0].emoji_id != 9998 ||
        payload.guild.presences[0].activities[0].emoji_name == NULL ||
        strcmp(payload.guild.presences[0].activities[0].emoji_name, "eyes") != 0 ||
        payload.guild.emojis == NULL ||
        payload.guild.emojis_count != 1 ||
        payload.guild.emojis[0].id != 9999 ||
        payload.guild.emojis[0].name == NULL ||
        strcmp(payload.guild.emojis[0].name, "spark") != 0 ||
        payload.guild.emojis[0].user.id != 446 ||
        payload.guild.emojis[0].user.avatar == NULL ||
        strcmp(payload.guild.emojis[0].user.avatar, "emoji-user-avatar") != 0 ||
        payload.guild.emojis[0].user.flags != 128 ||
        payload.guild.stickers == NULL ||
        payload.guild.stickers_count != 1 ||
        payload.guild.stickers[0].id != 9010 ||
        payload.guild.stickers[0].guild_id != 333 ||
        payload.guild.stickers[0].user.id != 446 ||
        payload.guild.stickers[0].user.avatar == NULL ||
        strcmp(payload.guild.stickers[0].user.avatar, "sticker-user-avatar") != 0 ||
        payload.guild.stickers[0].user.flags != 256 ||
        payload.guild.soundboard_sounds == NULL ||
        payload.guild.soundboard_sounds_count != 1 ||
        payload.guild.soundboard_sounds[0].id != 5678 ||
        payload.guild.soundboard_sounds[0].guild_id != 333 ||
        payload.guild.soundboard_sounds[0].user_id != 446 ||
        payload.guild.soundboard_sounds[0].user.avatar == NULL ||
        strcmp(payload.guild.soundboard_sounds[0].user.avatar, "sound-user-avatar") != 0 ||
        payload.guild.soundboard_sounds[0].user.flags != 512 ||
        payload.guild.soundboard_sounds[0].emoji_name == NULL ||
        strcmp(payload.guild.soundboard_sounds[0].emoji_name, "spark") != 0) {
        fprintf(stderr, "gateway guild create collection projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    return 0;
}
