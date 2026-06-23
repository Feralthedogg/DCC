#include <dcc/dcc.h>
#include <dcc/sugar.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int env_bool(const char *name, int fallback) {
    uint8_t parsed = fallback ? 1U : 0U;
    if (DCC_ENV_BOOL_OR(name, parsed, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr, "ignoring invalid %s=%s, using %d\n", name, value, fallback ? 1 : 0);
        return fallback;
    }
    return parsed ? 1 : 0;
}

static uint32_t env_u32(const char *name, uint32_t fallback, uint32_t min_value, uint32_t max_value) {
    uint32_t parsed = fallback;
    if (DCC_ENV_U32_RANGE_OR(name, fallback, min_value, max_value, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr, "ignoring invalid %s=%s, using %u\n", name, value, (unsigned)fallback);
        return fallback;
    }
    return parsed;
}

static dcc_snowflake_t env_guild(const char *name, dcc_snowflake_t fallback) {
    dcc_snowflake_t parsed = fallback;
    if (DCC_ENV_GUILD_OR(name, fallback, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr,
                "ignoring invalid %s=%s, using %llu\n",
                name,
                value,
                (unsigned long long)fallback);
        return fallback;
    }
    return parsed;
}

static dcc_snowflake_t env_channel(const char *name, dcc_snowflake_t fallback) {
    dcc_snowflake_t parsed = fallback;
    if (DCC_ENV_CHANNEL_OR(name, fallback, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr,
                "ignoring invalid %s=%s, using %llu\n",
                name,
                value,
                (unsigned long long)fallback);
        return fallback;
    }
    return parsed;
}

static dcc_snowflake_t env_user(const char *name, dcc_snowflake_t fallback) {
    dcc_snowflake_t parsed = fallback;
    if (DCC_ENV_USER_OR(name, fallback, &parsed) != DCC_OK) {
        const char *value = NULL;
        (void)DCC_ENV_STRING_OR(name, "", &value);
        fprintf(stderr,
                "ignoring invalid %s=%s, using %llu\n",
                name,
                value,
                (unsigned long long)fallback);
        return fallback;
    }
    return parsed;
}

static const char *env_token(void) {
    const char *token = NULL;
    return DCC_ENV_TOKEN(&token) == DCC_OK ? token : NULL;
}

static int require_live_config(
    const char **out_token,
    dcc_snowflake_t *out_guild_id,
    dcc_snowflake_t *out_channel_id,
    dcc_snowflake_t *out_user_id
) {
    const char *token = env_token();
    dcc_snowflake_t guild_id = env_guild("DCC_VOICE_GUILD_ID", 0);
    dcc_snowflake_t channel_id = env_channel("DCC_VOICE_CHANNEL_ID", 0);
    dcc_snowflake_t user_id = env_user("DCC_VOICE_USER_ID", 0);

    if (token == NULL || token[0] == '\0' || guild_id == 0 || channel_id == 0 || user_id == 0) {
        fprintf(
            stderr,
            "set DCC_TOKEN, BOT_TOKEN, or DISCORD_TOKEN plus "
            "DCC_VOICE_GUILD_ID, DCC_VOICE_CHANNEL_ID, and "
            "DCC_VOICE_USER_ID to run the live voice probe\n"
        );
        return 0;
    }

    *out_token = token;
    *out_guild_id = guild_id;
    *out_channel_id = channel_id;
    *out_user_id = user_id;
    return 1;
}

static void print_event_snapshot(const char *label, const dcc_event_snapshot_t *snapshot) {
    if (snapshot == NULL || snapshot->size == 0) {
        return;
    }

    const char *name = snapshot->name[0] != '\0'
        ? snapshot->name
        : dcc_event_type_name(snapshot->type);
    printf(
        "%s: type=%s shard=%u sequence=%llu raw_len=%zu\n",
        label,
        name != NULL ? name : "UNKNOWN",
        snapshot->shard_id,
        (unsigned long long)snapshot->raw_sequence,
        snapshot->raw_json_len
    );
}

static void print_voice_health(const char *label, const dcc_voice_health_snapshot_t *snapshot) {
    if (snapshot == NULL || snapshot->size == 0) {
        return;
    }

    printf(
        "%s: health=%s state=%s action=%s ready=%u active=%u media=%u websocket=%u udp=%u "
        "ssrc=%u reason=%s\n",
        label,
        dcc_voice_health_string(snapshot->health),
        dcc_voice_session_state_string(snapshot->session.state),
        dcc_voice_reconnect_action_string(snapshot->action),
        snapshot->session.ready,
        snapshot->session.active,
        snapshot->media_ready,
        snapshot->websocket_ready,
        snapshot->udp_ready,
        snapshot->stats.ssrc,
        snapshot->reason
    );
}

static void print_voice_health_json(const char *label, const dcc_voice_health_snapshot_t *snapshot) {
    if (!env_bool("DCC_VOICE_PRINT_JSON", 1)) {
        return;
    }

    char json[4096];
    size_t json_len = 0;
    dcc_status_t status = dcc_voice_client_health_snapshot_json(snapshot, json, sizeof(json), &json_len);
    if (status == DCC_OK) {
        printf("%s json: %.*s\n", label, (int)json_len, json);
    } else {
        fprintf(stderr, "%s json failed: %s\n", label, dcc_status_string(status));
    }
}

static void stop_client(dcc_client_t *client, int started) {
    if (client == NULL || !started) {
        return;
    }
    (void)dcc_client_stop(client);
    (void)dcc_client_wait(client);
}

int main(void) {
    (void)dcc_app_load_dotenv();

    const char *token = NULL;
    dcc_snowflake_t guild_id = 0;
    dcc_snowflake_t channel_id = 0;
    dcc_snowflake_t user_id = 0;
    if (!require_live_config(&token, &guild_id, &channel_id, &user_id)) {
        return 0;
    }

    uint32_t ready_timeout_ms = env_u32("DCC_VOICE_READY_TIMEOUT_MS", 30000U, 1U, 600000U);
    uint32_t descriptor_timeout_ms =
        env_u32("DCC_VOICE_DESCRIPTOR_TIMEOUT_MS", 30000U, 1U, 600000U);
    uint32_t active_timeout_ms = env_u32("DCC_VOICE_ACTIVE_TIMEOUT_MS", 30000U, 1U, 600000U);
    uint32_t reconnect_delay_ms = env_u32("DCC_VOICE_RECONNECT_DELAY_MS", 1000U, 1U, 600000U);
    uint8_t self_mute = env_bool("DCC_VOICE_SELF_MUTE", 0) ? 1U : 0U;
    uint8_t self_deaf = env_bool("DCC_VOICE_SELF_DEAF", 0) ? 1U : 0U;
    uint8_t enable_dave = env_bool("DCC_VOICE_ENABLE_DAVE", 0) ? 1U : 0U;
    int run_websocket = env_bool("DCC_VOICE_RUN_WEBSOCKET", 1);
    int wait_active = env_bool("DCC_VOICE_WAIT_ACTIVE", 1);

    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    int client_started = 0;
    int websocket_started = 0;
    int connected = 0;
    int exit_code = 1;

    dcc_client_options_t options =
        DCC_CLIENT_OPTIONS(token, DCC_INTENTS_DEFAULT | DCC_INTENT_GUILD_VOICE_STATES);
    options.gateway_max_concurrency = env_u32("DCC_VOICE_MAX_CONCURRENCY", 1U, 1U, 16U);

    dcc_status_t status = dcc_client_create(&options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    status = dcc_client_start(client);
    if (status != DCC_OK) {
        fprintf(stderr, "client start failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    client_started = 1;

    dcc_event_snapshot_t ready_snapshot = {
        .size = sizeof(ready_snapshot),
    };
    status = dcc_client_wait_for_gateway_ready_resumed_or_close(
        client,
        ready_timeout_ms,
        &ready_snapshot
    );
    if (status != DCC_OK || ready_snapshot.type == DCC_EVENT_SOCKET_CLOSE) {
        fprintf(stderr,
                "gateway ready wait failed: status=%s event=%s\n",
                dcc_status_string(status),
                dcc_event_type_name(ready_snapshot.type));
        goto cleanup;
    }
    print_event_snapshot("gateway ready", &ready_snapshot);

    dcc_voice_connect_wait_options_t join_options;
    dcc_voice_connect_wait_options_init(&join_options);
    join_options.guild_id = guild_id;
    join_options.channel_id = channel_id;
    join_options.self_mute = self_mute;
    join_options.self_deaf = self_deaf;
    join_options.enable_dave = enable_dave;
    join_options.descriptor_timeout_ms = descriptor_timeout_ms;
    join_options.ready_timeout_ms = ready_timeout_ms;

    dcc_voice_connect_wait_result_t join_result = {
        .size = sizeof(join_result),
    };
    status = dcc_voice_client_connect_and_wait_until_ready(voice_client, &join_options, &join_result);
    connected = join_result.connect_status == DCC_OK;
    if (status != DCC_OK) {
        fprintf(stderr,
                "voice join failed: status=%s connect=%s descriptor=%s ready=%s completed=%zu\n",
                dcc_status_string(status),
                dcc_status_string(join_result.connect_status),
                dcc_status_string(join_result.descriptor_status),
                dcc_status_string(join_result.ready_status),
                join_result.descriptor_completed);
        print_voice_health("voice join failed", &join_result.health);
        print_voice_health_json("voice join failed", &join_result.health);
        goto cleanup;
    }
    for (size_t i = 0; i < join_result.descriptor_completed; ++i) {
        print_event_snapshot(i == 0 ? "voice descriptor[0]" : "voice descriptor[1]",
                             &join_result.descriptor_snapshots[i]);
    }

    dcc_voice_health_snapshot_t health = {
        .size = sizeof(health),
    };
    health = join_result.health;
    print_voice_health("voice ready", &health);
    print_voice_health_json("voice ready", &health);

    if (run_websocket) {
        status = dcc_voice_client_start_websocket_loop(voice_client, user_id, reconnect_delay_ms);
        if (status != DCC_OK) {
            fprintf(stderr, "voice websocket loop start failed: %s\n", dcc_status_string(status));
            goto cleanup;
        }
        websocket_started = 1;

        if (wait_active) {
            memset(&health, 0, sizeof(health));
            health.size = sizeof(health);
            status = dcc_voice_client_wait_until_active(voice_client, active_timeout_ms, &health);
            if (status != DCC_OK) {
                fprintf(stderr, "voice active wait failed: %s\n", dcc_status_string(status));
                print_voice_health("voice active failed", &health);
                print_voice_health_json("voice active failed", &health);
                goto cleanup;
            }
            print_voice_health("voice active", &health);
            print_voice_health_json("voice active", &health);
        }
    }

    exit_code = 0;

cleanup:
    if (voice_client != NULL) {
        if (websocket_started) {
            status = dcc_voice_client_stop_websocket_loop(voice_client);
            if (status != DCC_OK) {
                fprintf(stderr, "voice websocket loop stop failed: %s\n", dcc_status_string(status));
                exit_code = 1;
            }
        }
        if (connected) {
            (void)dcc_voice_client_leave(voice_client);
        }
    }
    stop_client(client, client_started);
    dcc_voice_client_destroy(voice_client);
    dcc_client_destroy(client);
    return exit_code;
}
