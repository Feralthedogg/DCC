#include "gateway_smoke_server.h"

#if !defined(_WIN32)

#include "gateway_smoke_ws.h"

#include <stddef.h>

int gateway_server_write_extra_dispatches(int client, unsigned seq) {
    static const struct {
        const char *name;
        const char *payload;
    } extra_dispatches[] = {
        {"APPLICATION_COMMAND_PERMISSIONS_UPDATE", "{\"id\":\"777\",\"application_id\":\"666\",\"guild_id\":\"333\",\"permissions\":[{\"id\":\"555\",\"type\":1,\"permission\":true},{\"id\":\"446\",\"type\":2,\"permission\":false}]}"},
        {"AUTO_MODERATION_RULE_UPDATE", "{\"id\":\"778\",\"guild_id\":\"333\",\"name\":\"links\",\"enabled\":true}"},
        {"AUTO_MODERATION_RULE_DELETE", "{\"id\":\"778\",\"guild_id\":\"333\",\"name\":\"links\"}"},
        {"CHANNEL_UPDATE", "{\"id\":\"222\",\"guild_id\":\"333\",\"type\":0,\"name\":\"general\"}"},
        {"CHANNEL_DELETE", "{\"id\":\"222\",\"guild_id\":\"333\",\"type\":0,\"name\":\"general\"}"},
        {"CHANNEL_INFO", "{\"id\":\"222\",\"guild_id\":\"333\",\"topic\":\"general info\"}"},
        {"THREAD_CREATE", "{\"id\":\"888\",\"guild_id\":\"333\",\"parent_id\":\"222\",\"type\":11,\"name\":\"thread\"}"},
        {"THREAD_UPDATE", "{\"id\":\"888\",\"guild_id\":\"333\",\"parent_id\":\"222\",\"type\":11,\"name\":\"thread\"}"},
        {"THREAD_DELETE", "{\"id\":\"888\",\"guild_id\":\"333\",\"parent_id\":\"222\",\"type\":11,\"name\":\"thread\"}"},
        {"ENTITLEMENT_UPDATE", "{\"id\":\"700\",\"sku_id\":\"123\",\"application_id\":\"666\",\"user_id\":\"446\",\"guild_id\":\"333\",\"type\":8}"},
        {"ENTITLEMENT_DELETE", "{\"id\":\"700\",\"sku_id\":\"123\",\"application_id\":\"666\",\"user_id\":\"446\",\"guild_id\":\"333\",\"type\":8}"},
        {"GUILD_AUDIT_LOG_ENTRY_CREATE", "{\"id\":\"901\",\"guild_id\":\"333\",\"user_id\":\"446\",\"target_id\":\"111\",\"action_type\":1}"},
        {"GUILD_BAN_ADD", "{\"guild_id\":\"333\",\"user\":{\"id\":\"446\",\"username\":\"member\"}}"},
        {"GUILD_BAN_REMOVE", "{\"guild_id\":\"333\",\"user\":{\"id\":\"446\",\"username\":\"member\"}}"},
        {"GUILD_STICKERS_UPDATE", "{\"guild_id\":\"333\",\"stickers\":[{\"id\":\"9010\",\"name\":\"sticker\",\"user\":{\"id\":\"446\",\"username\":\"sticker-user\",\"avatar\":\"sticker-user-avatar\",\"public_flags\":256}}]}"},
        {"GUILD_INTEGRATIONS_UPDATE", "{\"guild_id\":\"333\"}"},
        {"GUILD_MEMBER_REMOVE", "{\"guild_id\":\"333\",\"user\":{\"id\":\"446\",\"username\":\"member\"}}"},
        {"GUILD_ROLE_UPDATE", "{\"guild_id\":\"333\",\"role\":{\"id\":\"555\",\"name\":\"mods\"}}"},
        {"GUILD_ROLE_DELETE", "{\"guild_id\":\"333\",\"role_id\":\"555\"}"},
        {"GUILD_SCHEDULED_EVENT_UPDATE", "{\"id\":\"888\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"name\":\"event\",\"scheduled_start_time\":\"2026-06-15T00:00:00.000000+00:00\",\"scheduled_end_time\":null}"},
        {"GUILD_SCHEDULED_EVENT_DELETE", "{\"id\":\"888\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"name\":\"event\"}"},
        {"GUILD_SCHEDULED_EVENT_USER_ADD", "{\"guild_scheduled_event_id\":\"888\",\"guild_id\":\"333\",\"user_id\":\"446\"}"},
        {"GUILD_SCHEDULED_EVENT_USER_REMOVE", "{\"guild_scheduled_event_id\":\"888\",\"guild_id\":\"333\",\"user_id\":\"446\"}"},
        {"GUILD_SOUNDBOARD_SOUND_UPDATE", "{\"id\":\"5678\",\"guild_id\":\"333\",\"name\":\"spark\",\"volume\":0.75,\"emoji_id\":\"9999\",\"emoji_name\":\"spark\",\"available\":true}"},
        {"GUILD_SOUNDBOARD_SOUND_DELETE", "{\"id\":\"5678\",\"guild_id\":\"333\"}"},
        {"SOUNDBOARD_SOUNDS", "{\"guild_id\":\"333\",\"soundboard_sounds\":[{\"id\":\"5678\",\"name\":\"spark\"}]}"},
        {"INTEGRATION_UPDATE", "{\"id\":\"321\",\"guild_id\":\"333\",\"name\":\"builds\",\"type\":\"discord\",\"enabled\":true,\"role_id\":\"555\"}"},
        {"INTEGRATION_DELETE", "{\"id\":\"321\",\"guild_id\":\"333\",\"application_id\":\"666\"}"},
        {"INVITE_DELETE", "{\"channel_id\":\"222\",\"guild_id\":\"333\",\"code\":\"abc\"}"},
        {"MESSAGE_REACTION_REMOVE", "{\"user_id\":\"446\",\"channel_id\":\"222\",\"message_id\":\"111\",\"guild_id\":\"333\",\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"}}"},
        {"MESSAGE_REACTION_REMOVE_ALL", "{\"channel_id\":\"222\",\"message_id\":\"111\",\"guild_id\":\"333\"}"},
        {"MESSAGE_REACTION_REMOVE_EMOJI", "{\"channel_id\":\"222\",\"message_id\":\"111\",\"guild_id\":\"333\",\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"}}"},
        {"MESSAGE_POLL_VOTE_REMOVE", "{\"user_id\":\"446\",\"channel_id\":\"222\",\"message_id\":\"111\",\"guild_id\":\"333\",\"answer_id\":3}"},
        {"STAGE_INSTANCE_UPDATE", "{\"id\":\"900\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"topic\":\"town hall\",\"privacy_level\":2,\"discoverable_disabled\":true}"},
        {"STAGE_INSTANCE_DELETE", "{\"id\":\"900\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"topic\":\"town hall\"}"},
        {"SUBSCRIPTION_UPDATE", "{\"id\":\"999\",\"user_id\":\"446\",\"sku_ids\":[\"123\"],\"entitlement_ids\":[\"700\"],\"status\":1}"},
        {"SUBSCRIPTION_DELETE", "{\"id\":\"999\",\"user_id\":\"446\",\"sku_ids\":[\"123\"],\"entitlement_ids\":[\"700\"],\"status\":2}"},
        {"VOICE_CHANNEL_START_TIME_UPDATE", "{\"guild_id\":\"333\",\"channel_id\":\"222\",\"voice_start_time\":123456789}"},
        {"VOICE_CHANNEL_STATUS_UPDATE", "{\"guild_id\":\"333\",\"channel_id\":\"222\",\"status\":\"live\"}"},
        {"WEBHOOKS_UPDATE", "{\"guild_id\":\"333\",\"channel_id\":\"222\"}"},
        {"GUILD_JOIN_REQUEST_DELETE", "{\"guild_id\":\"333\",\"user_id\":\"446\"}"},
        {"GUILD_JOIN_REQUEST_UPDATE", "{\"guild_id\":\"333\",\"user_id\":\"446\",\"status\":\"pending\"}"},
        {"GUILD_APPLICATION_COMMAND_COUNTS_UPDATE", "{\"guild_id\":\"333\",\"application_id\":\"666\"}"},
        {"GUILD_APPLICATION_COMMAND_INDEX_UPDATE", "{\"guild_id\":\"333\",\"application_id\":\"666\"}"},
        {"EMBEDDED_ACTIVITY_UPDATE", "{\"guild_id\":\"333\",\"channel_id\":\"222\",\"application_id\":\"666\"}"},
        {"CHANNEL_TOPIC_UPDATE", "{\"id\":\"222\",\"guild_id\":\"333\",\"topic\":\"updated topic\"}"},
        {"RATE_LIMITED", "{\"retry_after\":1.25}"},
        {"INTERACTION_CREATE", "{\"id\":\"556\",\"application_id\":\"666\",\"type\":3,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"button-token\",\"data\":{\"custom_id\":\"button-primary\",\"component_type\":2}}"},
        {"INTERACTION_CREATE", "{\"id\":\"557\",\"application_id\":\"666\",\"type\":3,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"select-token\",\"data\":{\"custom_id\":\"select-colour\",\"component_type\":3,\"values\":[\"red\",\"blue\"]}}"},
        {"INTERACTION_CREATE", "{\"id\":\"558\",\"application_id\":\"666\",\"type\":4,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"autocomplete-token\",\"data\":{\"id\":\"780\",\"name\":\"complete\",\"type\":1,\"options\":[{\"focused\":true,\"name\":\"query\",\"type\":3,\"value\":\"r\"}]}}"},
        {"INTERACTION_CREATE", "{\"id\":\"559\",\"application_id\":\"666\",\"type\":5,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"modal-token\",\"data\":{\"custom_id\":\"modal-submit\",\"components\":[]}}"},
        {"INTERACTION_CREATE", "{\"id\":\"560\",\"application_id\":\"666\",\"type\":2,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"user-menu-token\",\"data\":{\"id\":\"781\",\"name\":\"inspect_user\",\"type\":2,\"target_id\":\"446\"}}"},
        {"INTERACTION_CREATE", "{\"id\":\"561\",\"application_id\":\"666\",\"type\":2,\"guild_id\":\"333\",\"channel_id\":\"222\",\"token\":\"message-menu-token\",\"data\":{\"id\":\"782\",\"name\":\"inspect_message\",\"type\":3,\"target_id\":\"111\"}}"}
    };
    for (size_t i = 0; i < sizeof(extra_dispatches) / sizeof(extra_dispatches[0]); i++) {
        if (write_dispatch(client, extra_dispatches[i].name, seq++, extra_dispatches[i].payload) != 0) {
            return -1;
        }
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_UPDATE\",\"s\":83,\"op\":0,\"d\":{"
        "\"id\":\"333\",\"name\":\"DCC Guild Updated\",\"large\":true,"
        "\"owner_id\":\"447\",\"description\":\"updated guild description\","
        "\"icon\":\"guild-updated-icon\",\"banner\":\"guild-updated-banner\","
        "\"application_id\":\"667\",\"afk_channel_id\":\"324\","
        "\"system_channel_id\":\"325\",\"rules_channel_id\":\"326\","
        "\"public_updates_channel_id\":\"327\",\"widget_channel_id\":\"328\","
        "\"safety_alerts_channel_id\":\"329\","
        "\"vanity_url_code\":\"dcc-updated\",\"preferred_locale\":\"ko\","
        "\"splash\":\"guild-updated-splash\","
        "\"discovery_splash\":\"guild-updated-discovery\","
        "\"flags\":32,\"afk_timeout\":600,\"max_presences\":6000,"
        "\"max_members\":260000,\"premium_subscription_count\":13,"
        "\"max_video_channel_users\":50,\"default_message_notifications\":0,"
        "\"premium_tier\":3,\"verification_level\":4,"
        "\"explicit_content_filter\":1,\"mfa_level\":1,\"nsfw_level\":2,"
        "\"widget_enabled\":false,\"premium_progress_bar_enabled\":true,"
        "\"member_count\":43,\"unavailable\":false}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"VOICE_STATE_UPDATE\",\"s\":84,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_id\":\"222\",\"user_id\":\"444\","
        "\"session_id\":\"voice-session\",\"deaf\":false,\"mute\":true,"
        "\"request_to_speak_timestamp\":\"2026-06-15T03:00:00.000000+00:00\","
        "\"self_deaf\":false,\"self_mute\":true,\"self_stream\":false,"
        "\"self_video\":false,\"suppress\":true}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"VOICE_STATE_UPDATE\",\"s\":85,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_id\":null,\"user_id\":\"444\","
        "\"session_id\":\"voice-session\",\"deaf\":false,\"mute\":true,"
        "\"request_to_speak_timestamp\":\"2026-06-15T03:00:00.000000+00:00\","
        "\"self_deaf\":false,\"self_mute\":true,\"self_stream\":false,"
        "\"self_video\":false,\"suppress\":true}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_CREATE\",\"s\":86,\"op\":0,\"d\":{"
        "\"id\":\"113\",\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"content\":\"persistent hello\",\"flags\":64,"
        "\"timestamp\":\"2026-06-15T04:00:00.000000+00:00\","
        "\"edited_timestamp\":null,\"type\":1,"
        "\"tts\":true,\"mention_everyone\":true,\"pinned\":true,"
        "\"author\":{\"id\":\"448\",\"username\":\"cache-author\","
        "\"global_name\":\"Cache Author\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"cache-author-avatar\",\"public_flags\":8}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_UPDATE\",\"s\":87,\"op\":0,\"d\":{"
        "\"id\":\"113\",\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"content\":\"persistent edited\",\"flags\":0,\"type\":0,"
        "\"tts\":false,\"mention_everyone\":false,\"pinned\":false,"
        "\"edited_timestamp\":\"2026-06-15T05:00:00.000000+00:00\"}}"
    ) != 0) {
        return -1;
    }
    if (write_close_frame(client, 4000, "gateway smoke close") != 0) {
        return -1;
    }
    drain_client_close(client);
    return 0;
}

#endif
