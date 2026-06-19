#include "gateway_smoke_server.h"

#if !defined(_WIN32)

#include "gateway_smoke_ws.h"

int gateway_server_write_core_dispatches(int client) {
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_CREATE\",\"s\":3,\"op\":0,\"d\":{"
        "\"id\":\"111\",\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"content\":\"hello DCC\",\"flags\":64,"
        "\"timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"edited_timestamp\":\"2026-06-15T01:00:00.000000+00:00\","
        "\"tts\":false,\"mention_everyone\":true,\"pinned\":true,"
        "\"type\":19,\"webhook_id\":\"777\","
        "\"author\":{\"id\":\"444\",\"username\":\"feral\","
        "\"global_name\":\"Feral\",\"discriminator\":\"0\",\"bot\":false}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_UPDATE\",\"s\":4,\"op\":0,\"d\":{"
        "\"id\":\"111\",\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"content\":\"edited\",\"edited_timestamp\":\"2026-06-15T02:00:00.000000+00:00\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_DELETE\",\"s\":5,\"op\":0,\"d\":{"
        "\"id\":\"111\",\"channel_id\":\"222\",\"guild_id\":\"333\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_CREATE\",\"s\":6,\"op\":0,\"d\":{"
        "\"id\":\"333\",\"name\":\"DCC Guild\",\"large\":false,"
        "\"owner_id\":\"446\",\"description\":\"guild description\","
        "\"icon\":\"guild-icon\",\"banner\":\"guild-banner\","
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
        "\"member_count\":42,\"unavailable\":false,"
        "\"roles\":[{\"id\":\"555\",\"name\":\"mods\",\"permissions\":\"1024\","
        "\"color\":16711680,\"position\":4,\"hoist\":true,"
        "\"managed\":false,\"mentionable\":true}],"
        "\"channels\":[{\"id\":\"222\",\"guild_id\":\"333\",\"type\":0,"
        "\"name\":\"general\",\"topic\":\"chat\",\"position\":2}],"
        "\"threads\":[{\"id\":\"888\",\"guild_id\":\"333\",\"parent_id\":\"222\","
        "\"type\":11,\"name\":\"thread\"}],"
        "\"guild_scheduled_events\":[{\"id\":\"889\",\"guild_id\":\"333\","
        "\"channel_id\":\"222\",\"name\":\"event\",\"description\":\"guild event\","
        "\"status\":1,\"user_count\":5,"
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
        "\"stickers\":[{\"id\":\"9010\",\"guild_id\":\"333\",\"name\":\"sticker\","
        "\"description\":\"desc\",\"tags\":\"tag\",\"type\":2,\"format_type\":1,"
        "\"user\":{\"id\":\"446\",\"username\":\"sticker-user\","
        "\"avatar\":\"sticker-user-avatar\",\"public_flags\":256}}],"
        "\"soundboard_sounds\":[{\"id\":\"5678\",\"name\":\"spark\",\"volume\":0.75,"
        "\"emoji_id\":\"9999\",\"emoji_name\":\"spark\",\"available\":true,"
        "\"user\":{\"id\":\"446\",\"username\":\"sound-user\","
        "\"avatar\":\"sound-user-avatar\",\"public_flags\":512}}]}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_DELETE\",\"s\":7,\"op\":0,\"d\":{"
        "\"id\":\"333\",\"unavailable\":true}}"
    ) != 0) {
        return -1;
    }
    return write_text_frame(
        client,
        "{\"t\":\"INTERACTION_CREATE\",\"s\":8,\"op\":0,\"d\":{"
        "\"id\":\"555\",\"application_id\":\"666\",\"type\":2,"
        "\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"interaction-token\","
        "\"message_id\":\"111\",\"version\":1,"
        "\"locale\":\"ko\",\"guild_locale\":\"en-US\","
        "\"app_permissions\":\"8192\",\"attachment_size_limit\":26214400,"
        "\"context\":0,\"authorizing_integration_owners\":{\"0\":\"333\",\"1\":\"446\"},"
        "\"entitlements\":[{\"id\":\"700\",\"sku_id\":\"123\","
        "\"application_id\":\"666\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"type\":8,\"starts_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"ends_at\":null,\"flags\":2}],"
        "\"data\":{\"id\":\"777\",\"name\":\"ping\",\"type\":1,"
        "\"options\":[{\"name\":\"text\",\"type\":3,\"value\":\"hello\"},"
        "{\"name\":\"count\",\"type\":4,\"value\":42},"
        "{\"name\":\"ratio\",\"type\":10,\"value\":1.5},"
        "{\"name\":\"visible\",\"type\":5,\"value\":true},"
        "{\"name\":\"target\",\"type\":6,\"value\":\"446\"}],"
        "\"resolved\":{"
        "\"users\":{\"446\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"}},"
        "\"members\":{\"446\":{\"user\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"},"
        "\"roles\":[\"555\"],\"nick\":\"TargetNick\","
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":false,"
        "\"communication_disabled_until\":null,\"flags\":2,\"permissions\":\"2048\"}},"
        "\"roles\":{\"555\":{\"id\":\"555\",\"name\":\"mods\",\"permissions\":\"8\","
        "\"color\":123,\"position\":4,\"hoist\":true,\"managed\":false,"
        "\"mentionable\":true}},"
        "\"channels\":{\"222\":{\"id\":\"222\",\"guild_id\":\"333\",\"type\":0,"
        "\"name\":\"general\",\"topic\":\"ops\",\"position\":2}},"
        "\"messages\":{\"111\":{\"id\":\"111\",\"channel_id\":\"222\","
        "\"guild_id\":\"333\",\"content\":\"hello resolved\","
        "\"timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"edited_timestamp\":null,"
        "\"author\":{\"id\":\"446\",\"username\":\"target-user\","
        "\"global_name\":\"Target\",\"avatar\":\"target-avatar\"},"
        "\"type\":0,\"flags\":64,\"tts\":false,\"mention_everyone\":false,"
        "\"pinned\":true}},"
        "\"attachments\":{\"888\":{\"id\":\"888\",\"filename\":\"clip.ogg\","
        "\"description\":\"voice clip\","
        "\"content_type\":\"audio/ogg\",\"size\":4096,"
        "\"url\":\"https://cdn.discordapp.com/attachments/clip.ogg\","
        "\"proxy_url\":\"https://media.discordapp.net/attachments/clip.ogg\","
        "\"height\":480,\"width\":640,\"ephemeral\":true,"
        "\"duration_secs\":1.25,\"waveform\":\"AAEC\",\"flags\":4}}}}}}"
    );
}

#endif
