#include "voice_smoke_support.h"

#if !defined(_WIN32)
#include <llam/runtime.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int voice_websocket_loop_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t server_thread;
    voice_ws_loop_server_t server;
    int server_started = 0;
    int failed = 1;

    if (voice_ws_loop_start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "voice websocket loop server start failed\n");
        return 1;
    }
    server_started = 1;

    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .shard_id = 7,
        .shard_count = 1,
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice websocket loop client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice websocket loop voice create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    voice_smoke_state_t event_state;
    memset(&event_state, 0, sizeof(event_state));
    event_state.client = client;
    event_state.voice_client = voice_client;
    event_state.shard_id = 7;

    dcc_voice_state_t voice_state = {
        .guild_id = 2,
        .channel_id = 222,
        .user_id = 1,
        .session_id = "s",
    };
    dcc_voice_server_update_t voice_server = {
        .guild_id = 2,
        .token = "t",
        .endpoint = "127.0.0.1",
    };
    if (dcc_client_on(client, DCC_EVENT_VOICE_READY, voice_smoke_on_event, &event_state, NULL) != DCC_OK ||
        dcc_voice_client_start_session(voice_client, 2, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &voice_state, 1) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        fprintf(stderr, "voice websocket loop setup failed\n");
        goto cleanup;
    }

    voice_ws_loop_task_t task;
    memset(&task, 0, sizeof(task));
    task.voice_client = voice_client;
    snprintf(task.url, sizeof(task.url), "ws://127.0.0.1:%u/?v=8", (unsigned)server.port);

    llam_task_t *client_task = llam_spawn(voice_ws_loop_client_task, &task, NULL);
    if (client_task == NULL || llam_detach(client_task) != 0) {
        fprintf(stderr, "voice websocket loop task spawn failed\n");
        goto cleanup;
    }

    status = dcc_client_wait(client);
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    if (status != DCC_OK ||
        task.status != DCC_OK ||
        server.accepts != 2 ||
        !server.saw_identify ||
        !server.saw_resume ||
        event_state.failed ||
        !event_state.ready_seen ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.state != DCC_VOICE_SESSION_DISCONNECTED ||
        session_info.reconnect_requested ||
        session_info.full_reconnect_requested ||
        session_info.reconnect_attempts != 0 ||
        dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client)) {
        fprintf(stderr,
                "voice websocket loop failed: wait=%s task=%s accepts=%d identify=%d resume=%d ready=%d reconnect=%d full=%d attempts=%u state=%d\n",
                dcc_status_string(status),
                dcc_status_string(task.status),
                server.accepts,
                server.saw_identify,
                server.saw_resume,
                event_state.ready_seen,
                dcc_voice_client_reconnect_requested(voice_client),
                dcc_voice_client_full_reconnect_requested(voice_client),
                (unsigned)session_info.reconnect_attempts,
                (int)session_info.state);
        goto cleanup;
    }

    failed = 0;

cleanup:
    if (client != NULL) {
        (void)dcc_client_stop(client);
    }
    if (server_started) {
        close(server.fd);
        pthread_join(server_thread, NULL);
    }
    if (voice_client != NULL) {
        dcc_voice_client_destroy(voice_client);
    }
    if (client != NULL) {
        dcc_client_destroy(client);
    }
    return failed;
}

#endif
