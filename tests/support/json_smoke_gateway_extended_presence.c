#include "json_smoke_gateway_extended_internal.h"
#include "json_smoke_support.h"

int dcc_json_smoke_gateway_extended_presence_lists(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t payload;

    const char presence_input[] =
        "{\"t\":\"PRESENCE_UPDATE\",\"s\":118,\"op\":0,\"d\":{"
        "\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"avatar\":\"member-avatar\",\"public_flags\":64},\"guild_id\":\"333\","
        "\"status\":\"online\",\"since\":123456,"
        "\"activities\":[{\"name\":\"coding\",\"type\":0,\"state\":\"deep\","
        "\"details\":\"parser\",\"url\":null,\"created_at\":123456789,"
        "\"application_id\":\"666\",\"flags\":1,"
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"}}],"
        "\"client_status\":{\"desktop\":\"online\",\"mobile\":\"idle\",\"web\":\"dnd\"}}}";
    st = dcc_json_parse_gateway_payload(presence_input, sizeof(presence_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.user_id != 446 ||
        payload.gateway.user.id != 446 ||
        payload.gateway.user.avatar == NULL ||
        strcmp(payload.gateway.user.avatar, "member-avatar") != 0 ||
        payload.gateway.user.flags != 64 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.status == NULL ||
        strcmp(payload.gateway.status, "online") != 0 ||
        payload.gateway.since != 123456 ||
        payload.gateway.desktop_status == NULL ||
        strcmp(payload.gateway.desktop_status, "online") != 0 ||
        payload.gateway.mobile_status == NULL ||
        strcmp(payload.gateway.mobile_status, "idle") != 0 ||
        payload.gateway.web_status == NULL ||
        strcmp(payload.gateway.web_status, "dnd") != 0 ||
        payload.gateway.activities == NULL ||
        payload.gateway.activities_count != 1 ||
        payload.gateway.activities[0].name == NULL ||
        strcmp(payload.gateway.activities[0].name, "coding") != 0 ||
        payload.gateway.activities[0].state == NULL ||
        strcmp(payload.gateway.activities[0].state, "deep") != 0 ||
        payload.gateway.activities[0].details == NULL ||
        strcmp(payload.gateway.activities[0].details, "parser") != 0 ||
        payload.gateway.activities[0].url != NULL ||
        payload.gateway.activities[0].created_at != 123456789 ||
        payload.gateway.activities[0].application_id != 666 ||
        payload.gateway.activities[0].flags != 1 ||
        payload.gateway.activities[0].emoji_id != 9999 ||
        payload.gateway.activities[0].emoji_name == NULL ||
        strcmp(payload.gateway.activities[0].emoji_name, "spark") != 0 ||
        payload.presence.user.id != 446 ||
        payload.presence.user.username == NULL ||
        strcmp(payload.presence.user.username, "member") != 0 ||
        payload.presence.user.avatar == NULL ||
        strcmp(payload.presence.user.avatar, "member-avatar") != 0 ||
        payload.presence.user.flags != 64 ||
        payload.presence.guild_id != 333 ||
        payload.presence.status == NULL ||
        strcmp(payload.presence.status, "online") != 0 ||
        payload.presence.since != 123456 ||
        payload.presence.desktop_status == NULL ||
        strcmp(payload.presence.desktop_status, "online") != 0 ||
        payload.presence.mobile_status == NULL ||
        strcmp(payload.presence.mobile_status, "idle") != 0 ||
        payload.presence.web_status == NULL ||
        strcmp(payload.presence.web_status, "dnd") != 0 ||
        payload.presence.activities == NULL ||
        payload.presence.activities_count != 1 ||
        payload.presence.activities[0].name == NULL ||
        strcmp(payload.presence.activities[0].name, "coding") != 0 ||
        payload.presence.activities[0].emoji_id != 9999 ||
        payload.presence.activities[0].emoji_name == NULL ||
        strcmp(payload.presence.activities[0].emoji_name, "spark") != 0) {
        fprintf(stderr, "gateway presence projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char thread_member_input[] =
        "{\"t\":\"THREAD_MEMBER_UPDATE\",\"s\":119,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"join_timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"flags\":1}}";
    st = dcc_json_parse_gateway_payload(thread_member_input, sizeof(thread_member_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.id != 888 ||
        payload.gateway.user_id != 446 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.flags != 1 ||
        payload.gateway.join_timestamp == NULL ||
        strcmp(payload.gateway.join_timestamp, "2026-06-15T00:00:00.000000+00:00") != 0) {
        fprintf(stderr, "gateway thread member projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char emojis_input[] =
        "{\"t\":\"GUILD_EMOJIS_UPDATE\",\"s\":120,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"emojis\":["
        "{\"id\":\"9999\",\"name\":\"spark\","
        "\"user\":{\"id\":\"446\",\"username\":\"emoji-user\","
        "\"avatar\":\"emoji-user-avatar\",\"public_flags\":128}},"
        "{\"id\":\"10000\",\"name\":\"wave\"}]}}";
    st = dcc_json_parse_gateway_payload(emojis_input, sizeof(emojis_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.ids == NULL ||
        payload.gateway.ids_count != 2 ||
        payload.gateway.ids[0] != 9999 ||
        payload.gateway.ids[1] != 10000 ||
        !payload.has_guild_id_list_update ||
        payload.guild_id_list_update.guild_id != 333 ||
        payload.guild_id_list_update.ids == NULL ||
        payload.guild_id_list_update.ids_count != 2 ||
        payload.guild_id_list_update.ids[1] != 10000 ||
        payload.guild_id_list_update.emojis == NULL ||
        payload.guild_id_list_update.emojis_count != 2 ||
        payload.guild_id_list_update.emojis[0].id != 9999 ||
        payload.guild_id_list_update.emojis[0].name == NULL ||
        strcmp(payload.guild_id_list_update.emojis[0].name, "spark") != 0 ||
        payload.guild_id_list_update.emojis[0].user.id != 446 ||
        payload.guild_id_list_update.emojis[0].user.avatar == NULL ||
        strcmp(payload.guild_id_list_update.emojis[0].user.avatar, "emoji-user-avatar") != 0 ||
        payload.guild_id_list_update.emojis[0].user.flags != 128 ||
        payload.guild_id_list_update.emojis[1].id != 10000 ||
        payload.guild_id_list_update.emojis[1].name == NULL ||
        strcmp(payload.guild_id_list_update.emojis[1].name, "wave") != 0) {
        fprintf(stderr, "gateway emoji array projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char stickers_input[] =
        "{\"t\":\"GUILD_STICKERS_UPDATE\",\"s\":120,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"stickers\":["
        "{\"id\":\"9010\",\"guild_id\":\"333\",\"name\":\"sticker\","
        "\"description\":\"desc\",\"tags\":\"tag\",\"type\":2,\"format_type\":1,\"sort_value\":7,"
        "\"user\":{\"id\":\"446\",\"username\":\"sticker-user\","
        "\"avatar\":\"sticker-user-avatar\",\"public_flags\":256}}]}}";
    st = dcc_json_parse_gateway_payload(stickers_input, sizeof(stickers_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.ids == NULL ||
        payload.gateway.ids_count != 1 ||
        payload.gateway.ids[0] != 9010 ||
        !payload.has_guild_id_list_update ||
        payload.guild_id_list_update.stickers == NULL ||
        payload.guild_id_list_update.stickers_count != 1 ||
        payload.guild_id_list_update.stickers[0].id != 9010 ||
        payload.guild_id_list_update.stickers[0].guild_id != 333 ||
        payload.guild_id_list_update.stickers[0].name == NULL ||
        strcmp(payload.guild_id_list_update.stickers[0].name, "sticker") != 0 ||
        payload.guild_id_list_update.stickers[0].user.id != 446 ||
        payload.guild_id_list_update.stickers[0].user.avatar == NULL ||
        strcmp(payload.guild_id_list_update.stickers[0].user.avatar, "sticker-user-avatar") != 0 ||
        payload.guild_id_list_update.stickers[0].user.flags != 256 ||
        payload.guild_id_list_update.stickers[0].description == NULL ||
        strcmp(payload.guild_id_list_update.stickers[0].description, "desc") != 0 ||
        payload.guild_id_list_update.stickers[0].tags == NULL ||
        strcmp(payload.guild_id_list_update.stickers[0].tags, "tag") != 0 ||
        payload.guild_id_list_update.stickers[0].type != 2 ||
        payload.guild_id_list_update.stickers[0].format_type != 1 ||
        payload.guild_id_list_update.stickers[0].sort_value != 7) {
        fprintf(
            stderr,
            "gateway sticker array projection validation failed: %s has=%d gid=%llu ids=%zu first=%llu "
            "stickers=%zu sid=%llu sgid=%llu name=%s desc=%s tags=%s type=%u format=%u sort=%u\n",
            dcc_status_string(st),
            payload.has_guild_id_list_update,
            (unsigned long long)payload.gateway.guild_id,
            payload.gateway.ids_count,
            payload.gateway.ids != NULL ? (unsigned long long)payload.gateway.ids[0] : 0ULL,
            payload.guild_id_list_update.stickers_count,
            payload.guild_id_list_update.stickers != NULL
                ? (unsigned long long)payload.guild_id_list_update.stickers[0].id
                : 0ULL,
            payload.guild_id_list_update.stickers != NULL
                ? (unsigned long long)payload.guild_id_list_update.stickers[0].guild_id
                : 0ULL,
            payload.guild_id_list_update.stickers != NULL && payload.guild_id_list_update.stickers[0].name != NULL
                ? payload.guild_id_list_update.stickers[0].name
                : "(null)",
            payload.guild_id_list_update.stickers != NULL &&
                    payload.guild_id_list_update.stickers[0].description != NULL
                ? payload.guild_id_list_update.stickers[0].description
                : "(null)",
            payload.guild_id_list_update.stickers != NULL && payload.guild_id_list_update.stickers[0].tags != NULL
                ? payload.guild_id_list_update.stickers[0].tags
                : "(null)",
            payload.guild_id_list_update.stickers != NULL ? payload.guild_id_list_update.stickers[0].type : 0,
            payload.guild_id_list_update.stickers != NULL
                ? payload.guild_id_list_update.stickers[0].format_type
                : 0,
            payload.guild_id_list_update.stickers != NULL
                ? payload.guild_id_list_update.stickers[0].sort_value
                : 0
        );
        return 1;
    }

    const char thread_list_input[] =
        "{\"t\":\"THREAD_LIST_SYNC\",\"s\":121,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_ids\":[\"222\"],"
        "\"threads\":[{\"id\":\"888\",\"guild_id\":\"333\"}],"
        "\"members\":[{\"id\":\"888\",\"user_id\":\"446\"}]}}";
    st = dcc_json_parse_gateway_payload(thread_list_input, sizeof(thread_list_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.channel_ids == NULL ||
        payload.gateway.channel_ids_count != 1 ||
        payload.gateway.channel_ids[0] != 222 ||
        payload.gateway.ids == NULL ||
        payload.gateway.ids_count != 1 ||
        payload.gateway.ids[0] != 888 ||
        payload.gateway.member_ids == NULL ||
        payload.gateway.member_ids_count != 1 ||
        payload.gateway.member_ids[0] != 446 ||
        !payload.has_thread_list_sync ||
        payload.thread_list_sync.guild_id != 333 ||
        payload.thread_list_sync.channel_ids == NULL ||
        payload.thread_list_sync.channel_ids[0] != 222 ||
        payload.thread_list_sync.thread_ids == NULL ||
        payload.thread_list_sync.thread_ids[0] != 888 ||
        payload.thread_list_sync.member_ids == NULL ||
        payload.thread_list_sync.member_ids[0] != 446 ||
        payload.thread_list_sync.threads == NULL ||
        payload.thread_list_sync.threads_count != 1 ||
        payload.thread_list_sync.threads[0].id != 888 ||
        payload.thread_list_sync.threads[0].guild_id != 333 ||
        payload.thread_list_sync.members == NULL ||
        payload.thread_list_sync.members_count != 1 ||
        payload.thread_list_sync.members[0].id != 888 ||
        payload.thread_list_sync.members[0].user_id != 446) {
        fprintf(stderr, "gateway thread list projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char soundboard_list_input[] =
        "{\"t\":\"GUILD_SOUNDBOARD_SOUNDS_UPDATE\",\"s\":122,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"soundboard_sounds\":["
        "{\"id\":\"5678\",\"name\":\"spark\","
        "\"user\":{\"id\":\"446\",\"username\":\"sound-user\","
        "\"avatar\":\"sound-user-avatar\",\"public_flags\":512}},"
        "{\"id\":\"5679\",\"name\":\"wave\"}]}}";
    st = dcc_json_parse_gateway_payload(soundboard_list_input, sizeof(soundboard_list_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.ids == NULL ||
        payload.gateway.ids_count != 2 ||
        payload.gateway.ids[0] != 5678 ||
        payload.gateway.ids[1] != 5679 ||
        !payload.has_guild_id_list_update ||
        payload.guild_id_list_update.guild_id != 333 ||
        payload.guild_id_list_update.ids == NULL ||
        payload.guild_id_list_update.ids_count != 2 ||
        payload.guild_id_list_update.ids[0] != 5678 ||
        payload.guild_id_list_update.soundboard_sounds == NULL ||
        payload.guild_id_list_update.soundboard_sounds_count != 2 ||
        payload.guild_id_list_update.soundboard_sounds[0].id != 5678 ||
        payload.guild_id_list_update.soundboard_sounds[0].name == NULL ||
        strcmp(payload.guild_id_list_update.soundboard_sounds[0].name, "spark") != 0 ||
        payload.guild_id_list_update.soundboard_sounds[0].user_id != 446 ||
        payload.guild_id_list_update.soundboard_sounds[0].user.avatar == NULL ||
        strcmp(payload.guild_id_list_update.soundboard_sounds[0].user.avatar, "sound-user-avatar") != 0 ||
        payload.guild_id_list_update.soundboard_sounds[0].user.flags != 512 ||
        payload.guild_id_list_update.soundboard_sounds[1].id != 5679 ||
        payload.guild_id_list_update.soundboard_sounds[1].name == NULL ||
        strcmp(payload.guild_id_list_update.soundboard_sounds[1].name, "wave") != 0) {
        fprintf(stderr, "gateway soundboard array projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char simple_events_input[] =
        "{\"t\":\"VOICE_CHANNEL_STATUS_UPDATE\",\"s\":123,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_id\":\"222\",\"status\":\"live\"}}";
    st = dcc_json_parse_gateway_payload(simple_events_input, sizeof(simple_events_input) - 1, &payload);
    if (st != DCC_OK ||
        !payload.has_voice_channel_status_update ||
        payload.voice_channel_status_update.guild_id != 333 ||
        payload.voice_channel_status_update.channel_id != 222 ||
        payload.voice_channel_status_update.status == NULL ||
        strcmp(payload.voice_channel_status_update.status, "live") != 0) {
        fprintf(stderr, "gateway voice channel status projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char app_command_input[] =
        "{\"t\":\"APPLICATION_COMMAND_PERMISSIONS_UPDATE\",\"s\":124,\"op\":0,\"d\":{"
        "\"id\":\"777\",\"application_id\":\"666\",\"guild_id\":\"333\","
        "\"permissions\":[{\"id\":\"555\",\"type\":1,\"permission\":true},"
        "{\"id\":\"446\",\"type\":2,\"permission\":false}]}}";
    st = dcc_json_parse_gateway_payload(app_command_input, sizeof(app_command_input) - 1, &payload);
    if (st != DCC_OK ||
        !payload.has_application_command_permissions_update ||
        payload.application_command_permissions_update.id != 777 ||
        payload.application_command_permissions_update.application_id != 666 ||
        payload.application_command_permissions_update.guild_id != 333 ||
        payload.application_command_permissions_update.permissions == NULL ||
        payload.application_command_permissions_update.permissions_count != 2 ||
        payload.application_command_permissions_update.permissions[0].id != 555 ||
        payload.application_command_permissions_update.permissions[0].type != 1 ||
        payload.application_command_permissions_update.permissions[0].permission != 1 ||
        payload.application_command_permissions_update.permissions[1].id != 446 ||
        payload.application_command_permissions_update.permissions[1].type != 2 ||
        payload.application_command_permissions_update.permissions[1].permission != 0) {
        fprintf(stderr, "gateway application command projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    return 0;
}
