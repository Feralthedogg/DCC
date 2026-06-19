#include "gateway_smoke_server.h"

#if !defined(_WIN32)

#include "gateway_smoke_ws.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

static int gateway_server_handshake(gateway_server_t *server, int client) {
    (void)server;

    char request[4096];
    if (read_http_upgrade(client, request, sizeof(request)) != 0) {
        return -1;
    }

    char *key = find_header(request, "Sec-WebSocket-Key");
    char accept_value[64];
    if (key == NULL || ws_accept_for_key(key, accept_value) != 0) {
        return -1;
    }

    char response[512];
    int response_len = snprintf(
        response,
        sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n"
        "\r\n",
        accept_value
    );
    if (response_len <= 0 || write(client, response, (size_t)response_len) != response_len) {
        return -1;
    }

    return write_text_frame(client, "{\"op\":10,\"d\":{\"heartbeat_interval\":20}}");
}

static int gateway_server_first_connection(gateway_server_t *server, int client) {
    if (gateway_server_handshake(server, client) != 0) {
        return -1;
    }

    char identify[2048];
    if (read_text_frame(client, identify, sizeof(identify)) == 0 &&
        strstr(identify, "\"op\":2") != NULL &&
        strstr(identify, "\"token\":\"mock-token\"") != NULL) {
        server->saw_identify = 1;
    }

    char heartbeat[256];
    if (server->saw_identify &&
        read_text_frame(client, heartbeat, sizeof(heartbeat)) == 0 &&
        strstr(heartbeat, "\"op\":1") != NULL) {
        server->saw_heartbeat = 1;
        (void)write_text_frame(client, "{\"op\":11,\"d\":null}");
    }

    char ready[768];
    int ready_len = snprintf(
        ready,
        sizeof(ready),
        "{\"t\":\"READY\",\"s\":1,\"op\":0,\"d\":{\"session_id\":\"mock-session\","
        "\"user\":{\"id\":\"444\",\"username\":\"self\",\"discriminator\":\"0\",\"bot\":true},"
        "\"guilds\":[{\"id\":\"333\",\"unavailable\":false},{\"id\":\"444\",\"unavailable\":true}],"
        "\"resume_gateway_url\":\"ws://127.0.0.1:%u/?v=10&encoding=json\"}}",
        (unsigned)server->port
    );
    if (ready_len <= 0 || (size_t)ready_len >= sizeof(ready)) {
        return -1;
    }

    if (write_text_frame(client, ready) != 0) {
        return -1;
    }
    char voice_state_update[512];
    if (read_text_frame_timeout(client, voice_state_update, sizeof(voice_state_update), 2000) == 0 &&
        strstr(voice_state_update, "\"op\":4") != NULL &&
        strstr(voice_state_update, "\"guild_id\":\"333\"") != NULL &&
        strstr(voice_state_update, "\"channel_id\":\"222\"") != NULL &&
        strstr(voice_state_update, "\"self_mute\":false") != NULL &&
        strstr(voice_state_update, "\"self_deaf\":false") != NULL) {
        server->saw_voice_state_update = 1;
    }
    if (!server->saw_voice_state_update) {
        return -1;
    }
    return write_text_frame(client, "{\"op\":7,\"d\":null}");
}

static int gateway_server_second_connection(gateway_server_t *server, int client) {
    if (gateway_server_handshake(server, client) != 0) {
        return -1;
    }

    char resume[2048];
    if (read_text_frame(client, resume, sizeof(resume)) == 0 &&
        strstr(resume, "\"op\":6") != NULL &&
        strstr(resume, "\"session_id\":\"mock-session\"") != NULL &&
        strstr(resume, "\"seq\":1") != NULL) {
        server->saw_resume = 1;
    }

    if (!server->saw_resume) {
        return -1;
    }

    if (write_text_frame(client, "{\"t\":\"RESUMED\",\"s\":2,\"op\":0,\"d\":{}}") != 0) {
        return -1;
    }
    if (gateway_server_write_core_dispatches(client) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"CHANNEL_CREATE\",\"s\":9,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"guild_id\":\"333\",\"parent_id\":\"222\","
        "\"last_message_id\":\"111\",\"type\":0,\"name\":\"general\","
        "\"topic\":\"chat\",\"position\":2,\"nsfw\":true,"
        "\"rate_limit_per_user\":5,\"bitrate\":64000,\"user_limit\":25}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"SUBSCRIPTION_CREATE\",\"s\":10,\"op\":0,\"d\":{"
        "\"id\":\"999\",\"user_id\":\"444\",\"sku_id\":\"123\","
        "\"sku_ids\":[\"123\",\"124\"],\"entitlement_ids\":[\"700\"],"
        "\"current_period_start\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"current_period_end\":\"2026-07-15T00:00:00.000000+00:00\","
        "\"canceled_at\":null,\"status\":1}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_REACTION_ADD\",\"s\":11,\"op\":0,\"d\":{"
        "\"user_id\":\"444\",\"channel_id\":\"222\",\"message_id\":\"111\","
        "\"guild_id\":\"333\",\"message_author_id\":\"445\","
        "\"member\":{\"user\":{\"id\":\"444\",\"username\":\"reactor\","
        "\"global_name\":\"Reactor\",\"discriminator\":\"0\",\"bot\":false},"
        "\"nick\":\"spark\",\"roles\":[\"555\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":false,"
        "\"communication_disabled_until\":null,\"flags\":4},"
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\",\"animated\":true}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"TYPING_START\",\"s\":12,\"op\":0,\"d\":{"
        "\"channel_id\":\"222\",\"guild_id\":\"333\","
        "\"user_id\":\"444\",\"timestamp\":123456,"
        "\"member\":{\"user\":{\"id\":\"444\",\"username\":\"typer\","
        "\"global_name\":null,\"discriminator\":\"0\",\"bot\":false},"
        "\"nick\":\"tap\",\"roles\":[\"555\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"flags\":2}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_DELETE_BULK\",\"s\":13,\"op\":0,\"d\":{"
        "\"ids\":[\"111\",\"112\"],\"channel_id\":\"222\",\"guild_id\":\"333\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"THREAD_MEMBERS_UPDATE\",\"s\":14,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"guild_id\":\"333\",\"member_count\":2,"
        "\"added_members\":[{\"id\":\"888\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"join_timestamp\":\"2026-06-15T00:00:00.000000+00:00\",\"flags\":1}],"
        "\"removed_member_ids\":[\"444\",\"445\"]}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_MEMBER_ADD\",\"s\":15,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"global_name\":\"Member\",\"discriminator\":\"0\",\"bot\":false}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_ROLE_CREATE\",\"s\":16,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"role\":{\"id\":\"555\",\"name\":\"mods\","
        "\"permissions\":\"1024\",\"color\":16711680,\"position\":4,"
        "\"hoist\":true,\"managed\":false,\"mentionable\":true}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"USER_UPDATE\",\"s\":17,\"op\":0,\"d\":{"
        "\"id\":\"447\",\"username\":\"self\",\"global_name\":null,"
        "\"avatar\":\"user-avatar\",\"discriminator\":\"0\",\"bot\":false,"
        "\"public_flags\":64}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"AUTO_MODERATION_ACTION_EXECUTION\",\"s\":18,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"action\":{\"type\":1},\"rule_id\":\"777\","
        "\"rule_trigger_type\":4,\"user_id\":\"446\",\"channel_id\":\"222\","
        "\"message_id\":\"111\",\"alert_system_message_id\":\"112\","
        "\"content\":\"blocked text\",\"matched_keyword\":\"bad\","
        "\"matched_content\":\"bad text\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"MESSAGE_POLL_VOTE_ADD\",\"s\":19,\"op\":0,\"d\":{"
        "\"user_id\":\"446\",\"channel_id\":\"222\",\"message_id\":\"111\","
        "\"guild_id\":\"333\",\"answer_id\":3}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"INVITE_CREATE\",\"s\":20,\"op\":0,\"d\":{"
        "\"channel_id\":\"222\",\"guild_id\":\"333\",\"code\":\"abc\","
        "\"inviter\":{\"id\":\"446\",\"username\":\"member\"},\"target_type\":2,"
        "\"max_age\":60,\"max_uses\":5,\"uses\":1,"
        "\"created_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"expires_at\":\"2026-06-16T00:00:00.000000+00:00\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"VOICE_CHANNEL_EFFECT_SEND\",\"s\":21,\"op\":0,\"d\":{"
        "\"channel_id\":\"222\",\"guild_id\":\"333\",\"user_id\":\"446\","
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"},\"animation_type\":1,"
        "\"animation_id\":\"1234\",\"sound_id\":\"5678\",\"sound_volume\":0.5}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_SCHEDULED_EVENT_CREATE\",\"s\":22,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"guild_id\":\"333\",\"channel_id\":\"222\",\"name\":\"event\","
        "\"description\":\"desc\",\"status\":2,\"user_count\":7,"
        "\"image\":\"directhash\",\"entity_metadata\":{\"location\":\"stage\"},"
        "\"creator\":{\"id\":\"446\",\"username\":\"creator\","
        "\"global_name\":\"Creator\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"creator-avatar\",\"public_flags\":32},"
        "\"scheduled_start_time\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"scheduled_end_time\":null,\"privacy_level\":2,\"entity_type\":3,"
        "\"entity_id\":null,\"creator_id\":\"446\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_MEMBER_UPDATE\",\"s\":23,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"user\":{\"id\":\"446\",\"username\":\"member\"},\"nick\":\"nick\","
        "\"roles\":[\"555\",\"556\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_MEMBERS_CHUNK\",\"s\":24,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"chunk_index\":1,\"chunk_count\":2,"
        "\"members\":[{\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"global_name\":\"Member\",\"discriminator\":\"0\",\"bot\":false,"
        "\"avatar\":\"member-user-avatar\",\"public_flags\":64},"
        "\"nick\":\"nick\",\"roles\":[\"555\",\"556\"],"
        "\"joined_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"premium_since\":null,\"pending\":true,"
        "\"communication_disabled_until\":\"2026-06-16T00:00:00.000000+00:00\","
        "\"flags\":2}],"
        "\"not_found\":[\"447\"],\"nonce\":\"n1\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"CHANNEL_PINS_UPDATE\",\"s\":25,\"op\":0,\"d\":{\"guild_id\":\"333\","
        "\"channel_id\":\"222\",\"last_pin_timestamp\":\"2026-06-15T00:00:00.000000+00:00\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"STAGE_INSTANCE_CREATE\",\"s\":26,\"op\":0,\"d\":{"
        "\"id\":\"900\",\"guild_id\":\"333\",\"channel_id\":\"222\","
        "\"topic\":\"town hall\",\"privacy_level\":2,"
        "\"discoverable_disabled\":true,\"guild_scheduled_event_id\":\"888\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_SOUNDBOARD_SOUND_CREATE\",\"s\":27,\"op\":0,\"d\":{"
        "\"id\":\"5678\",\"guild_id\":\"333\",\"name\":\"spark\","
        "\"volume\":0.75,\"emoji_id\":\"9999\",\"emoji_name\":\"spark\","
        "\"available\":true,\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"avatar\":\"member-avatar\",\"public_flags\":64}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"ENTITLEMENT_CREATE\",\"s\":28,\"op\":0,\"d\":{"
        "\"id\":\"700\",\"sku_id\":\"123\",\"application_id\":\"666\","
        "\"subscription_id\":\"800\",\"promotion_id\":\"900\","
        "\"gift_code_flags\":3,\"flags\":2,"
        "\"user_id\":\"446\",\"guild_id\":\"333\",\"type\":8,"
        "\"starts_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"ends_at\":null}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"VOICE_SERVER_UPDATE\",\"s\":29,\"op\":0,\"d\":{"
        "\"token\":\"voice-token\",\"guild_id\":\"333\","
        "\"endpoint\":\"voice.example.com\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"INTEGRATION_CREATE\",\"s\":30,\"op\":0,\"d\":{"
        "\"id\":\"321\",\"guild_id\":\"333\",\"name\":\"builds\","
        "\"type\":\"discord\",\"enabled\":true,\"role_id\":\"555\"}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"PRESENCE_UPDATE\",\"s\":31,\"op\":0,\"d\":{"
        "\"user\":{\"id\":\"446\",\"username\":\"member\","
        "\"avatar\":\"member-avatar\",\"public_flags\":64},\"guild_id\":\"333\","
        "\"status\":\"online\",\"since\":123456,"
        "\"activities\":[{\"name\":\"coding\",\"type\":0,\"state\":\"deep\","
        "\"details\":\"parser\",\"url\":null,\"created_at\":123456789,"
        "\"application_id\":\"666\",\"flags\":1,"
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"}}],"
        "\"client_status\":{\"desktop\":\"online\",\"mobile\":\"idle\",\"web\":\"dnd\"}}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"THREAD_MEMBER_UPDATE\",\"s\":32,\"op\":0,\"d\":{"
        "\"id\":\"888\",\"user_id\":\"446\",\"guild_id\":\"333\","
        "\"join_timestamp\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"flags\":1}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"AUTO_MODERATION_RULE_CREATE\",\"s\":33,\"op\":0,\"d\":{"
        "\"id\":\"778\",\"guild_id\":\"333\",\"name\":\"links\","
        "\"creator_id\":\"446\",\"event_type\":1,\"trigger_type\":3,"
        "\"enabled\":true,\"exempt_roles\":[\"555\"],"
        "\"exempt_channels\":[\"222\",\"223\"]}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_EMOJIS_UPDATE\",\"s\":34,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"emojis\":["
        "{\"id\":\"9999\",\"name\":\"spark\","
        "\"user\":{\"id\":\"446\",\"username\":\"emoji-user\","
        "\"avatar\":\"emoji-user-avatar\",\"public_flags\":128}},"
        "{\"id\":\"10000\",\"name\":\"wave\"}]}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"THREAD_LIST_SYNC\",\"s\":35,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"channel_ids\":[\"222\"],"
        "\"threads\":[{\"id\":\"888\",\"guild_id\":\"333\"}],"
        "\"members\":[{\"id\":\"888\",\"user_id\":\"446\"}]}}"
    ) != 0) {
        return -1;
    }
    if (write_text_frame(
        client,
        "{\"t\":\"GUILD_SOUNDBOARD_SOUNDS_UPDATE\",\"s\":36,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"soundboard_sounds\":["
        "{\"id\":\"5678\",\"name\":\"spark\","
        "\"user\":{\"id\":\"446\",\"username\":\"sound-user\","
        "\"avatar\":\"sound-user-avatar\",\"public_flags\":512}},"
        "{\"id\":\"5679\",\"name\":\"wave\"}]}}"
    ) != 0) {
        return -1;
    }
    return gateway_server_write_extra_dispatches(client, 37U);
}

static void *gateway_server_main(void *arg) {
    gateway_server_t *server = (gateway_server_t *)arg;
    for (int i = 0; i < 2; ++i) {
        int client = accept(server->fd, NULL, NULL);
        if (client < 0) {
            return NULL;
        }

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        (void)setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        (void)setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        int rc = i == 0
            ? gateway_server_first_connection(server, client)
            : gateway_server_second_connection(server, client);
        close(client);
        if (rc != 0) {
            return NULL;
        }
    }
    return NULL;
}

int start_server(gateway_server_t *server, pthread_t *thread) {
    memset(server, 0, sizeof(*server));
    server->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->fd < 0) {
        return -1;
    }

    int yes = 1;
    (void)setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = 0;

    if (bind(server->fd, (const struct sockaddr *)&addr, sizeof(addr)) != 0 ||
        listen(server->fd, 2) != 0) {
        close(server->fd);
        return -1;
    }

    socklen_t len = sizeof(addr);
    if (getsockname(server->fd, (struct sockaddr *)&addr, &len) != 0) {
        close(server->fd);
        return -1;
    }
    server->port = ntohs(addr.sin_port);

    if (pthread_create(thread, NULL, gateway_server_main, server) != 0) {
        close(server->fd);
        return -1;
    }
    return 0;
}

#endif
