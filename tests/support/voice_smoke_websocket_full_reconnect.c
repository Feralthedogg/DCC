#include "voice_smoke_support.h"

#if !defined(_WIN32)

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int voice_websocket_full_reconnect_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t gateway_thread;
    pthread_t voice_thread;
    pthread_t wait_thread;
    voice_full_reconnect_gateway_server_t gateway_server;
    voice_full_reconnect_ws_server_t voice_server;
    int gateway_started = 0;
    int voice_started = 0;
    int wait_thread_started = 0;
    int failed = 1;

    if (voice_full_reconnect_gateway_start_server(&gateway_server, &gateway_thread) != 0) {
        fprintf(stderr, "voice full reconnect gateway server start failed\n");
        return 1;
    }
    gateway_started = 1;
    if (voice_full_reconnect_ws_start_server(&voice_server, &voice_thread) != 0) {
        fprintf(stderr, "voice full reconnect websocket server start failed\n");
        goto cleanup;
    }
    voice_started = 1;

    char gateway_url[128];
    snprintf(gateway_url, sizeof(gateway_url), "ws://127.0.0.1:%u/?v=10&encoding=json", (unsigned)gateway_server.port);
    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .token = "mock-token",
        .gateway_url = gateway_url,
        .shard_id = 0,
        .shard_count = 1,
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice full reconnect client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice full reconnect voice create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }

    char endpoint[128];
    snprintf(endpoint, sizeof(endpoint), "ws://127.0.0.1:%u/?v=8", (unsigned)voice_server.port);
    dcc_voice_state_t voice_state = {
        .guild_id = 2,
        .channel_id = 222,
        .user_id = 1,
        .session_id = "s",
    };
    dcc_voice_server_update_t voice_server_update = {
        .guild_id = 2,
        .token = "t",
        .endpoint = endpoint,
    };
    if (dcc_voice_client_start_session(voice_client, 2, 222, 0, 0, 0) != DCC_OK ||
        dcc_voice_client_apply_voice_server_update(voice_client, &voice_server_update) != DCC_OK ||
        dcc_voice_client_apply_voice_state(voice_client, &voice_state, 1) != DCC_OK ||
        dcc_client_start(client) != DCC_OK) {
        fprintf(stderr, "voice full reconnect setup failed\n");
        goto cleanup;
    }

    voice_client_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    if (pthread_create(&wait_thread, NULL, voice_client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "voice full reconnect wait thread spawn failed\n");
        goto cleanup;
    }
    wait_thread_started = 1;

    for (int i = 0; i < 5000; ++i) {
        if (atomic_load_explicit(&gateway_server.saw_identify, memory_order_acquire) != 0U) {
            break;
        }
        usleep(1000U);
    }
    if (atomic_load_explicit(&gateway_server.saw_identify, memory_order_acquire) == 0U) {
        fprintf(stderr, "voice full reconnect gateway identify not observed\n");
        goto cleanup;
    }

    if (dcc_voice_client_start_websocket_loop(voice_client, 1, 1) != DCC_OK) {
        fprintf(stderr, "voice full reconnect managed loop start failed\n");
        goto cleanup;
    }

    for (int i = 0; i < 12000; ++i) {
        if (atomic_load_explicit(&gateway_server.saw_full_voice_state_update, memory_order_acquire) != 0U) {
            break;
        }
        usleep(1000U);
    }

    if (atomic_load_explicit(&gateway_server.saw_full_voice_state_update, memory_order_acquire) == 0U ||
        atomic_load_explicit(&gateway_server.saw_identify, memory_order_acquire) == 0U ||
        atomic_load_explicit(&voice_server.accepts, memory_order_acquire) != 5U ||
        atomic_load_explicit(&voice_server.saw_identify, memory_order_acquire) == 0U ||
        atomic_load_explicit(&voice_server.saw_resume, memory_order_acquire) == 0U) {
        dcc_status_t loop_status = DCC_OK;
        dcc_voice_session_info_t failed_info = {
            .size = sizeof(failed_info),
        };
        (void)dcc_voice_client_websocket_loop_status(voice_client, &loop_status);
        (void)dcc_voice_client_session_info(voice_client, &failed_info);
        fprintf(
            stderr,
            "voice full reconnect failed: gateway_identify=%u op4=%u accepts=%u identify=%u resume=%u "
            "running=%u loop=%s state=%d ready=%u attempts=%u reconnect=%u full=%u last_error=%s\n",
            atomic_load_explicit(&gateway_server.saw_identify, memory_order_acquire),
            atomic_load_explicit(&gateway_server.saw_full_voice_state_update, memory_order_acquire),
            atomic_load_explicit(&voice_server.accepts, memory_order_acquire),
            atomic_load_explicit(&voice_server.saw_identify, memory_order_acquire),
            atomic_load_explicit(&voice_server.saw_resume, memory_order_acquire),
            dcc_voice_client_websocket_loop_running(voice_client),
            dcc_status_string(loop_status),
            (int)failed_info.state,
            failed_info.ready,
            (unsigned)failed_info.reconnect_attempts,
            failed_info.reconnect_requested,
            failed_info.full_reconnect_requested,
            dcc_client_last_error(client)
        );
        goto cleanup;
    }

    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    if (dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        session_info.state != DCC_VOICE_SESSION_REQUESTED ||
        session_info.ready ||
        session_info.reconnect_attempts != 0 ||
        session_info.reconnect_requested ||
        session_info.full_reconnect_requested ||
        session_info.guild_id != 2 ||
        session_info.channel_id != 222) {
        fprintf(
            stderr,
            "voice full reconnect state failed: state=%d ready=%u attempts=%u reconnect=%u full=%u "
            "guild=%llu channel=%llu\n",
            (int)session_info.state,
            session_info.ready,
            (unsigned)session_info.reconnect_attempts,
            session_info.reconnect_requested,
            session_info.full_reconnect_requested,
            (unsigned long long)session_info.guild_id,
            (unsigned long long)session_info.channel_id
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
            fprintf(stderr, "voice full reconnect wait failed: %s\n", dcc_status_string(wait_state.status));
            failed = 1;
        }
    }
    if (voice_started) {
        close(voice_server.fd);
        pthread_join(voice_thread, NULL);
    }
    if (gateway_started) {
        close(gateway_server.fd);
        pthread_join(gateway_thread, NULL);
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
