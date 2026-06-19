#include "voice_smoke_support.h"

#if !defined(_WIN32)
#include <llam/runtime.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int voice_websocket_smoke(void) {
    dcc_client_t *client = NULL;
    dcc_voice_client_t *voice_client = NULL;
    pthread_t server_thread;
    pthread_t wait_thread;
    voice_ws_server_t server;
    int server_started = 0;
    int wait_thread_started = 0;
    int failed = 1;

    if (voice_ws_start_server(&server, &server_thread) != 0) {
        fprintf(stderr, "voice websocket server start failed\n");
        return 1;
    }
    server_started = 1;
    server.idle_max_us = 750000U;

    dcc_client_options_t opts = {
        .size = sizeof(opts),
        .shard_id = 7,
        .shard_count = 1,
    };
    dcc_status_t status = dcc_client_create(&opts, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice websocket client create failed: %s\n", dcc_status_string(status));
        goto cleanup;
    }
    status = dcc_voice_client_create(client, &voice_client);
    if (status != DCC_OK) {
        fprintf(stderr, "voice websocket voice create failed: %s\n", dcc_status_string(status));
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
        fprintf(stderr, "voice websocket setup failed\n");
        goto cleanup;
    }

    voice_ws_task_t task;
    memset(&task, 0, sizeof(task));
    task.voice_client = voice_client;
    snprintf(task.url, sizeof(task.url), "ws://127.0.0.1:%u/?v=8", (unsigned)server.port);

    llam_spawn_opts_t client_opts;
    if (llam_spawn_opts_init(&client_opts, LLAM_SPAWN_OPTS_CURRENT_SIZE) != 0) {
        fprintf(stderr, "voice websocket task options failed\n");
        goto cleanup;
    }
    client_opts.stack_class = LLAM_STACK_CLASS_LARGE;

    llam_task_t *client_task =
        llam_spawn_ex(voice_ws_client_task, &task, &client_opts, LLAM_SPAWN_OPTS_CURRENT_SIZE);
    if (client_task == NULL || llam_detach(client_task) != 0) {
        fprintf(stderr, "voice websocket task spawn failed\n");
        goto cleanup;
    }

    voice_client_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    if (pthread_create(&wait_thread, NULL, voice_client_wait_thread_main, &wait_state) != 0) {
        fprintf(stderr, "voice websocket wait thread spawn failed\n");
        goto cleanup;
    }
    wait_thread_started = 1;

    int idle_started = 0;
    for (int i = 0; i < 1000; ++i) {
        if (atomic_load_explicit(&server.idle_started, memory_order_acquire) != 0U) {
            idle_started = 1;
            break;
        }
        usleep(1000U);
    }
    if (!idle_started) {
        fprintf(stderr, "voice websocket idle hold was not reached\n");
        goto cleanup;
    }

    llam_runtime_stats_t idle_before;
    llam_runtime_stats_t idle_after;
    memset(&idle_before, 0, sizeof(idle_before));
    memset(&idle_after, 0, sizeof(idle_after));
    if (llam_runtime_collect_stats(&idle_before) != 0) {
        fprintf(stderr, "voice websocket idle stats before failed\n");
        goto cleanup;
    }
    uint64_t cpu_before_us = voice_rusage_cpu_us();
    usleep(250000U);
    uint64_t cpu_after_us = voice_rusage_cpu_us();
    if (llam_runtime_collect_stats(&idle_after) != 0) {
        fprintf(stderr, "voice websocket idle stats after failed\n");
        goto cleanup;
    }

    uint64_t idle_cpu_us = voice_u64_delta(cpu_after_us, cpu_before_us);
    uint64_t idle_ctx_switches = voice_u64_delta(idle_after.ctx_switches, idle_before.ctx_switches);
    uint64_t idle_yields = voice_u64_delta(idle_after.yields, idle_before.yields);
    uint64_t idle_preempt_yields = voice_u64_delta(idle_after.preempt_yields, idle_before.preempt_yields);
    if (idle_cpu_us > 150000U ||
        idle_ctx_switches > 4000U ||
        idle_yields > 4000U ||
        idle_preempt_yields > 4000U) {
        fprintf(
            stderr,
            "voice websocket idle spin regression: cpu_us=%llu ctx=%llu yields=%llu preempt_yields=%llu\n",
            (unsigned long long)idle_cpu_us,
            (unsigned long long)idle_ctx_switches,
            (unsigned long long)idle_yields,
            (unsigned long long)idle_preempt_yields
        );
        goto cleanup;
    }

    atomic_store_explicit(&server.idle_release, 1U, memory_order_release);
    (void)pthread_join(wait_thread, NULL);
    wait_thread_started = 0;
    status = wait_state.status;
    dcc_voice_session_info_t session_info = {
        .size = sizeof(session_info),
    };
    if (status != DCC_OK ||
        task.status != DCC_ERR_CANCELED ||
        !server.saw_identify ||
        !server.saw_discovery ||
        !server.saw_select_protocol ||
        !server.saw_heartbeat ||
        event_state.failed ||
        !event_state.ready_seen ||
        dcc_voice_client_ssrc(voice_client) != 123456 ||
        !dcc_voice_client_has_secret_key(voice_client) ||
        dcc_voice_client_session_info(voice_client, &session_info) != DCC_OK ||
        strcmp(session_info.udp_host, "127.0.0.1") != 0 ||
        session_info.udp_port != server.udp_port ||
        strcmp(session_info.discovered_address, "127.0.0.1") != 0 ||
        session_info.discovered_port == 0 ||
        !session_info.select_protocol_sent ||
        !dcc_voice_client_reconnect_requested(voice_client) ||
        dcc_voice_client_full_reconnect_requested(voice_client)) {
        fprintf(stderr,
                "voice websocket run failed: wait=%s task=%s identify=%d discovery=%d select=%d heartbeat=%d ready=%d reconnect=%d\n",
                dcc_status_string(status),
                dcc_status_string(task.status),
                server.saw_identify,
                server.saw_discovery,
                server.saw_select_protocol,
                server.saw_heartbeat,
                event_state.ready_seen,
                dcc_voice_client_reconnect_requested(voice_client));
        goto cleanup;
    }

    failed = 0;

cleanup:
    if (client != NULL) {
        (void)dcc_client_stop(client);
    }
    atomic_store_explicit(&server.idle_release, 1U, memory_order_release);
    if (wait_thread_started) {
        (void)pthread_join(wait_thread, NULL);
    }
    if (server_started) {
        close(server.fd);
        close(server.udp_fd);
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
