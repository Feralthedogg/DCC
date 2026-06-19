#include <dcc/dcc.h>

#include <stdio.h>

static int voice_wait_fail(
    dcc_client_t *client,
    dcc_voice_client_t *voice_client,
    const char *message
) {
    fprintf(stderr, "%s\n", message);
    if (voice_client != NULL) {
        dcc_voice_client_destroy(voice_client);
    }
    if (client != NULL) {
        dcc_client_destroy(client);
    }
    return 1;
}

int main(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_voice_health_snapshot_t snapshot = {
        .size = sizeof(snapshot),
    };
    dcc_voice_health_snapshot_t bad_snapshot = {0};
    dcc_voice_connect_wait_options_t join_options;
    dcc_voice_connect_wait_result_t join_result = {
        .size = sizeof(join_result),
    };
    dcc_voice_connect_wait_result_t bad_join_result = {0};

    dcc_voice_connect_wait_options_init(&join_options);
    if (join_options.size != sizeof(join_options) ||
        join_options.descriptor_timeout_ms != 30000U ||
        join_options.ready_timeout_ms != 30000U) {
        return voice_wait_fail(client, voice_client, "voice connect wait options init failed");
    }

    if (dcc_client_create(&opts, &client) != DCC_OK ||
        dcc_voice_client_create(client, &voice_client) != DCC_OK) {
        return voice_wait_fail(client, voice_client, "voice wait setup failed");
    }

    join_options.guild_id = 333;
    join_options.channel_id = 222;
    if (dcc_voice_client_wait_until_ready(NULL, 1U, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_wait_until_ready(voice_client, 1U, &bad_snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_wait_until_active(NULL, 1U, &snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_wait_until_active(voice_client, 1U, &bad_snapshot) != DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_and_wait_until_ready(NULL, &join_options, &join_result) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_and_wait_until_ready(voice_client, NULL, &join_result) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_and_wait_until_ready(voice_client, &join_options, &bad_join_result) !=
            DCC_ERR_INVALID_ARG ||
        dcc_voice_client_connect_and_wait_until_ready(voice_client, &join_options, &join_result) !=
            DCC_ERR_STATE ||
        join_result.connect_status != DCC_ERR_STATE ||
        join_result.descriptor_status != DCC_ERR_STATE ||
        join_result.ready_status != DCC_ERR_STATE ||
        join_result.descriptor_snapshots[0].size != sizeof(join_result.descriptor_snapshots[0]) ||
        join_result.descriptor_snapshots[1].size != sizeof(join_result.descriptor_snapshots[1]) ||
        join_result.health.size != sizeof(join_result.health) ||
        dcc_voice_client_wait_until_ready(voice_client, 1U, &snapshot) != DCC_ERR_STATE ||
        snapshot.health != DCC_VOICE_HEALTH_DISCONNECTED ||
        dcc_voice_client_wait_until_active(voice_client, 1U, &snapshot) != DCC_ERR_STATE ||
        snapshot.health != DCC_VOICE_HEALTH_DISCONNECTED) {
        return voice_wait_fail(client, voice_client, "voice wait disconnected contracts failed");
    }

    dcc_voice_state_t voice_state = {
        .guild_id = 333,
        .channel_id = 222,
        .user_id = 999,
        .session_id = "voice-session",
    };
    dcc_voice_server_update_t voice_server = {
        .guild_id = 333,
        .token = "voice-token",
        .endpoint = "voice.example.com",
    };
    if (dcc_voice_client_start_session(voice_client, 333, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &voice_state, 999) != DCC_OK) {
        return voice_wait_fail(client, voice_client, "voice wait ready setup failed");
    }

    snapshot.size = sizeof(snapshot);
    if (dcc_voice_client_wait_until_ready(voice_client, 1U, &snapshot) != DCC_OK ||
        snapshot.health != DCC_VOICE_HEALTH_READY ||
        snapshot.ok != 1U ||
        snapshot.session.ready == 0U ||
        dcc_voice_client_wait_until_active(voice_client, 1U, &snapshot) != DCC_ERR_TIMEOUT ||
        snapshot.health != DCC_VOICE_HEALTH_READY) {
        return voice_wait_fail(client, voice_client, "voice wait ready contracts failed");
    }

    if (dcc_voice_client_mark_session_active(voice_client) != DCC_OK ||
        dcc_voice_client_wait_until_active(voice_client, 1U, &snapshot) != DCC_OK ||
        snapshot.health != DCC_VOICE_HEALTH_ACTIVE ||
        snapshot.ok != 1U ||
        snapshot.session.active == 0U) {
        return voice_wait_fail(client, voice_client, "voice wait active contracts failed");
    }

    if (dcc_voice_client_handle_disconnect(voice_client, 4014, "channel gone") != DCC_OK ||
        dcc_voice_client_wait_until_ready(voice_client, 1U, &snapshot) != DCC_ERR_STATE ||
        snapshot.health != DCC_VOICE_HEALTH_TERMINATED ||
        dcc_voice_client_wait_until_active(voice_client, 1U, &snapshot) != DCC_ERR_STATE ||
        snapshot.health != DCC_VOICE_HEALTH_TERMINATED) {
        return voice_wait_fail(client, voice_client, "voice wait terminal contracts failed");
    }

    dcc_voice_client_destroy(voice_client);
    dcc_client_destroy(client);
    return 0;
}
