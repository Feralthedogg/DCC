#include "voice_smoke_support.h"

#if !defined(_WIN32)
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int voice_websocket_managed_loop_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t server_thread;
    pthread_t wait_thread;
    voice_ws_loop_server_t server;
    int server_started = 0;
    int wait_thread_started = 0;
    int failed = 1;

    if (voice_ws_loop_start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "voice managed websocket loop server start failed\n");
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
        fprintf(stderr, "voice managed websocket client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice managed websocket voice create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    voice_smoke_state_t event_state;
    memset(&event_state, 0, sizeof(event_state));
    event_state.client = client;
    event_state.voice_client = voice_client;
    event_state.shard_id = 7;

    char endpoint[128];
    snprintf(endpoint, sizeof(endpoint), "ws://127.0.0.1:%u/?v=8", (unsigned)server.port);
    dcc_voice_state_t voice_state = {
        .guild_id = 2,
        .channel_id = 222,
        .user_id = 1,
        .session_id = "s",
    };
    dcc_voice_server_update_t voice_server = {
        .guild_id = 2,
        .token = "t",
        .endpoint = endpoint,
    };
    if (dcc_client_on(client, DCC_EVENT_VOICE_READY, voice_smoke_on_event, &event_state, NULL) != DCC_OK ||
        dcc_voice_client_start_session(voice_client, 2, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &voice_state, 1) != DCC_OK ||
        dcc_client_start(client) != DCC_OK ||
        dcc_voice_client_start_websocket_loop(voice_client, 1, 1) != DCC_OK ||
        !dcc_voice_client_websocket_loop_running(voice_client)) {
        fprintf(stderr, "voice managed websocket loop setup failed\n");
        goto cleanup;
    }

    voice_client_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    if (pthread_create(&wait_thread, NULL, voice_client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "voice managed websocket wait thread spawn failed\n");
        goto cleanup;
    }
    wait_thread_started = 1;

    for (int i = 0; i < 2000; ++i) {
        if (!dcc_voice_client_websocket_loop_running(voice_client)) {
            break;
        }
        usleep(1000U);
    }

    dcc_status_t loop_status = DCC_ERR_RUNTIME;
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    if (dcc_voice_client_websocket_loop_running(voice_client) ||
        dcc_voice_client_websocket_loop_status(voice_client, &loop_status) != DCC_OK ||
        loop_status != DCC_OK ||
        server.accepts != 2 ||
        !server.saw_identify ||
        !server.saw_resume ||
        event_state.failed ||
        !event_state.ready_seen ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.state != DCC_VOICE_SESSION_DISCONNECTED ||
        session_info.reconnect_requested ||
        session_info.full_reconnect_requested ||
        session_info.reconnect_attempts != 0) {
        fprintf(stderr,
                "voice managed websocket loop failed: loop=%s accepts=%d identify=%d resume=%d ready=%d state=%d attempts=%u\n",
                dcc_status_string(loop_status),
                server.accepts,
                server.saw_identify,
                server.saw_resume,
                event_state.ready_seen,
                (int)session_info.state,
                (unsigned)session_info.reconnect_attempts);
        goto cleanup;
    }

    failed = 0;

cleanup:
    if (client != NULL) {
        (void)dcc_client_stop(client);
    }
    if (wait_thread_started) {
        pthread_join(wait_thread, NULL);
        if (!failed && wait_state.status != DCC_OK) {
            fprintf(stderr, "voice managed websocket wait failed: %s\n", dcc_status_string(wait_state.status));
            failed = 1;
        }
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

int voice_websocket_managed_close_chaos_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t server_thread;
    pthread_t wait_thread;
    voice_ws_chaos_server_t server;
    int server_started = 0;
    int wait_thread_started = 0;
    int failed = 1;

    if (voice_ws_chaos_start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "voice managed websocket chaos server start failed\n");
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
        fprintf(stderr, "voice managed websocket chaos client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice managed websocket chaos voice create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    voice_smoke_state_t event_state;
    memset(&event_state, 0, sizeof(event_state));
    event_state.client = client;
    event_state.voice_client = voice_client;
    event_state.shard_id = 7;

    char endpoint[128];
    snprintf(endpoint, sizeof(endpoint), "ws://127.0.0.1:%u/?v=8", (unsigned)server.port);
    dcc_voice_state_t voice_state = {
        .guild_id = 2,
        .channel_id = 222,
        .user_id = 1,
        .session_id = "s",
    };
    dcc_voice_server_update_t voice_server = {
        .guild_id = 2,
        .token = "t",
        .endpoint = endpoint,
    };
    if (dcc_client_on(client, DCC_EVENT_VOICE_READY, voice_smoke_on_event, &event_state, NULL) != DCC_OK ||
        dcc_voice_client_start_session(voice_client, 2, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &voice_server) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &voice_state, 1) != DCC_OK ||
        dcc_client_start(client) != DCC_OK ||
        dcc_voice_client_start_websocket_loop(voice_client, 1, 1) != DCC_OK ||
        !dcc_voice_client_websocket_loop_running(voice_client)) {
        fprintf(stderr, "voice managed websocket chaos setup failed\n");
        goto cleanup;
    }

    voice_client_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    if (pthread_create(&wait_thread, NULL, voice_client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "voice managed websocket chaos wait thread spawn failed\n");
        goto cleanup;
    }
    wait_thread_started = 1;

    for (int i = 0; i < 5000; ++i) {
        if (!dcc_voice_client_websocket_loop_running(voice_client)) {
            break;
        }
        usleep(1000U);
    }

    dcc_status_t loop_status = DCC_ERR_RUNTIME;
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    uint32_t accepts = atomic_load_explicit(&server.accepts, memory_order_acquire);
    uint32_t bad = atomic_load_explicit(&server.bad, memory_order_acquire);
    uint32_t saw_identify = atomic_load_explicit(&server.saw_identify, memory_order_acquire);
    uint32_t saw_resume = atomic_load_explicit(&server.saw_resume, memory_order_acquire);
    uint32_t sent_4015 = atomic_load_explicit(&server.sent_4015, memory_order_acquire);
    uint32_t sent_4014 = atomic_load_explicit(&server.sent_4014, memory_order_acquire);
    if (dcc_voice_client_websocket_loop_running(voice_client) ||
        dcc_voice_client_websocket_loop_status(voice_client, &loop_status) != DCC_OK ||
        loop_status != DCC_ERR_CANCELED ||
        bad != 0U ||
        accepts != 2U ||
        saw_identify == 0U ||
        saw_resume == 0U ||
        sent_4015 == 0U ||
        sent_4014 == 0U ||
        event_state.failed ||
        !event_state.ready_seen ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.state != DCC_VOICE_SESSION_TERMINATED ||
        session_info.reconnect_requested ||
        session_info.full_reconnect_requested ||
        session_info.reconnect_attempts != 0 ||
        session_info.last_close_code != 4014U ||
        strcmp(session_info.last_close_reason, "disconnected") != 0) {
        fprintf(
            stderr,
            "voice managed websocket chaos failed: loop=%s accepts=%u bad=%u identify=%u resume=%u "
            "4015=%u 4014=%u ready=%d state=%d attempts=%u reconnect=%u full=%u close=%u reason=%s\n",
            dcc_status_string(loop_status),
            accepts,
            bad,
            saw_identify,
            saw_resume,
            sent_4015,
            sent_4014,
            event_state.ready_seen,
            (int)session_info.state,
            (unsigned)session_info.reconnect_attempts,
            session_info.reconnect_requested,
            session_info.full_reconnect_requested,
            (unsigned)session_info.last_close_code,
            session_info.last_close_reason != NULL ? session_info.last_close_reason : ""
        );
        goto cleanup;
    }

    failed = 0;

cleanup:
    if (voice_client != NULL) {
        (void)dcc_voice_client_stop_websocket_loop(voice_client);
    }
    if (client != NULL) {
        (void)dcc_client_stop(client);
    }
    if (wait_thread_started) {
        pthread_join(wait_thread, NULL);
        if (!failed && wait_state.status != DCC_OK) {
            fprintf(stderr, "voice managed websocket chaos wait failed: %s\n", dcc_status_string(wait_state.status));
            failed = 1;
        }
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
