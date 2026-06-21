#include "hot_reload_smoke_support.h"

#if !defined(_WIN32)

#include "hot_reload_isolated_support.h"
#include "hot_reload_test_state.h"
#include "internal/client/dcc_client_state_internal.h"
#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int dcc_hot_reload_smoke_copy_file(const char *source, const char *target) {
    return dcc_hot_reload_test_copy_file(source, target);
}

int dcc_hot_reload_smoke_dispatch_ready(dcc_client_t *client, const char *session_id) {
    dcc_event_t event;
    memset(&event, 0, sizeof(event));
    event.type = DCC_EVENT_READY;
    event.data.ready.shard_id = 0;
    event.data.ready.shard_count = 1;
    event.data.ready.session_id = session_id;
    return dcc_event_bus_dispatch(&client->events, client, &event) == DCC_OK ? 0 : -1;
}

static int dcc_hot_reload_smoke_generation_overflow_backend(dcc_hot_reload_backend_t backend) {
    char path[256];
    int n = snprintf(path, sizeof(path), "/tmp/dcc-hot-reload-overflow-%ld-%u.so", (long)getpid(), (unsigned)backend);
    if (n <= 0 || (size_t)n >= sizeof(path) ||
        dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V1, path) != 0) {
        fprintf(stderr, "failed to prepare overflow module\n");
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "overflow client create failed: %s\n", dcc_status_string(status));
        unlink(path);
        return 1;
    }

    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = backend,
        .worker_path = backend == DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER
            ? DCC_HOT_RELOAD_DEFAULT_WORKER
            : NULL,
    };
    status = dcc_hot_reload_create(client, path, &options, &hot_reload);
    if (status == DCC_OK) {
        hot_reload->generation = UINT64_MAX;
        status = dcc_hot_reload_reload(hot_reload);
    }
    uint64_t generation = hot_reload != NULL ? hot_reload->generation : 0U;
    char last_error[256];
    snprintf(last_error, sizeof(last_error), "%s", hot_reload != NULL ? dcc_hot_reload_last_error(hot_reload) : "");
    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(path);

    if (status != DCC_ERR_STATE ||
        generation != UINT64_MAX ||
        strstr(last_error, "generation counter overflow") == NULL) {
        fprintf(
            stderr,
            "generation overflow failed: backend=%u status=%s generation=%llu error=%s\n",
            (unsigned)backend,
            dcc_status_string(status),
            (unsigned long long)generation,
            last_error
        );
        return 1;
    }
    return 0;
}

int dcc_hot_reload_smoke_generation_overflow(void) {
    return dcc_hot_reload_smoke_generation_overflow_backend(DCC_HOT_RELOAD_BACKEND_IN_PROCESS) != 0 ||
           dcc_hot_reload_smoke_generation_overflow_backend(DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER) != 0
        ? 1
        : 0;
}

int dcc_hot_reload_smoke_watcher(void) {
    char path[256];
    int n = snprintf(path, sizeof(path), "/tmp/dcc-hot-reload-watch-%ld.so", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(path) ||
        dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V1, path) != 0) {
        fprintf(stderr, "failed to prepare watcher module\n");
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "watcher client create failed: %s\n", dcc_status_string(status));
        unlink(path);
        return 1;
    }

    hot_reload_test_state_t state;
    memset(&state, 0, sizeof(state));
    dcc_hot_reload_t *hot_reload = NULL;
    pthread_t wait_thread;
    int wait_thread_started = 0;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .poll_interval_ms = 10,
        .user_data = &state,
    };
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    dcc_hot_reload_test_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    if (status == DCC_OK && pthread_create(&wait_thread, NULL, dcc_hot_reload_test_wait_main, &wait_state) == 0) {
        wait_thread_started = 1;
    } else if (status == DCC_OK) {
        status = DCC_ERR_RUNTIME;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_attach(client, path, &options, &hot_reload);
    }
    if (status != DCC_OK) {
        fprintf(stderr,
                "watcher start failed: %s %s\n",
                dcc_status_string(status),
                hot_reload != NULL ? dcc_hot_reload_last_error(hot_reload) : "");
        dcc_hot_reload_destroy(hot_reload);
        (void)dcc_client_stop(client);
        if (wait_thread_started) {
            pthread_join(wait_thread, NULL);
        }
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_hot_reload_health_snapshot_t wait_health = {
        .size = sizeof(wait_health),
    };
    dcc_hot_reload_health_snapshot_t bad_wait_health = {0};
    if (dcc_hot_reload_wait_for_generation(NULL, 1, 1) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_wait_for_generation(hot_reload, 0, 1) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_wait_for_generation(hot_reload, 1, 1000) != DCC_OK ||
        dcc_hot_reload_wait_until_loaded(NULL, 1, &wait_health) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_wait_until_loaded(hot_reload, 1, &bad_wait_health) != DCC_ERR_INVALID_ARG ||
        dcc_hot_reload_wait_until_loaded(hot_reload, 1000, &wait_health) != DCC_OK ||
        wait_health.loaded != 1U ||
        wait_health.generation != 1U ||
        atomic_load_explicit(&state.init_v1, memory_order_acquire) != 1U) {
        fprintf(stderr, "watcher did not load v1\n");
        dcc_hot_reload_destroy(hot_reload);
        (void)dcc_client_stop(client);
        if (wait_thread_started) {
            pthread_join(wait_thread, NULL);
        }
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    dcc_hot_reload_health_snapshot_t watch_health = {
        .size = sizeof(watch_health),
    };
    char watch_health_json[1536];
    if (dcc_hot_reload_health_snapshot(hot_reload, &watch_health) != DCC_OK ||
        watch_health.health != DCC_HOT_RELOAD_HEALTH_WATCHING ||
        watch_health.backend != DCC_HOT_RELOAD_BACKEND_IN_PROCESS ||
        watch_health.ok != 1U ||
        watch_health.running != 1U ||
        watch_health.loaded != 1U ||
        watch_health.worker_supervisor_running != 0U ||
        watch_health.active_worker_loaded != 0U ||
        watch_health.last_good_worker_loaded != 0U ||
        watch_health.generation != 1U ||
        dcc_hot_reload_health_snapshot_json(&watch_health, watch_health_json, sizeof(watch_health_json), NULL) !=
            DCC_OK ||
        strstr(watch_health_json, "\"backend\":\"in_process\"") == NULL ||
        strstr(watch_health_json, "\"health\":\"watching\"") == NULL ||
        strstr(watch_health_json, "\"ok\":true") == NULL ||
        watch_health.active_worker_pid != 0U ||
        watch_health.last_good_worker_pid != 0U ||
        strstr(watch_health_json, "\"active_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL ||
        strstr(watch_health_json, "\"last_good_worker\":{\"loaded\":false,\"generation\":0,\"pid\":0}") == NULL) {
        fprintf(stderr, "watcher health snapshot failed: %s\n", watch_health_json);
        dcc_hot_reload_destroy(hot_reload);
        (void)dcc_client_stop(client);
        if (wait_thread_started) {
            pthread_join(wait_thread, NULL);
        }
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    usleep(20000U);
    if (dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V2, path) != 0 ||
        dcc_hot_reload_wait_for_generation(hot_reload, 2, 1000) != DCC_OK ||
        atomic_load_explicit(&state.init_v2, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 1U) {
        fprintf(stderr,
                "watcher did not swap to v2: gen=%llu init2=%u deinit1=%u err=%s\n",
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.init_v2, memory_order_acquire),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        (void)dcc_client_stop(client);
        if (wait_thread_started) {
            pthread_join(wait_thread, NULL);
        }
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }
    if (dcc_hot_reload_wait_for_generation(hot_reload, 3, 1) != DCC_ERR_TIMEOUT) {
        fprintf(stderr, "watcher generation wait timeout contract failed\n");
        dcc_hot_reload_destroy(hot_reload);
        (void)dcc_client_stop(client);
        if (wait_thread_started) {
            pthread_join(wait_thread, NULL);
        }
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_hot_reload_destroy(hot_reload);
    (void)dcc_client_stop(client);
    if (wait_thread_started) {
        pthread_join(wait_thread, NULL);
        if (wait_state.status != DCC_OK && wait_state.status != DCC_ERR_CANCELED) {
            fprintf(stderr, "watcher wait failed: %s\n", dcc_status_string(wait_state.status));
            dcc_client_destroy(client);
            unlink(path);
            return 1;
        }
    }
    dcc_client_destroy(client);
    unlink(path);
    return atomic_load_explicit(&state.deinit_v2, memory_order_acquire) == 1U ? 0 : 1;
}

int dcc_hot_reload_smoke_settle_poll(void) {
    char path[256];
    int n = snprintf(path, sizeof(path), "/tmp/dcc-hot-reload-settle-%ld.so", (long)getpid());
    if (n <= 0 || (size_t)n >= sizeof(path) ||
        dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V1, path) != 0) {
        fprintf(stderr, "failed to prepare settle module\n");
        return 1;
    }

    dcc_client_t *client = NULL;
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = "",
        .intents = DCC_INTENT_GUILDS,
    };
    dcc_status_t status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        fprintf(stderr, "settle client create failed: %s\n", dcc_status_string(status));
        unlink(path);
        return 1;
    }

    hot_reload_test_state_t state;
    memset(&state, 0, sizeof(state));
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .poll_interval_ms = 10,
        .settle_interval_ms = 80,
        .user_data = &state,
    };
    status = dcc_hot_reload_create(client, path, &options, &hot_reload);
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status != DCC_OK || dcc_hot_reload_generation(hot_reload) != 1) {
        fprintf(stderr, "settle initial load failed: %s\n", dcc_status_string(status));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    usleep(20000U);
    if (dcc_hot_reload_smoke_copy_file(DCC_HOT_RELOAD_MODULE_V2, path) != 0) {
        fprintf(stderr, "failed to copy settle v2 module\n");
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    status = dcc_hot_reload_poll(hot_reload);
    if (status != DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 1 ||
        atomic_load_explicit(&state.init_v2, memory_order_acquire) != 0U) {
        fprintf(stderr,
                "settle first poll reloaded too early: status=%s gen=%llu init2=%u\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.init_v2, memory_order_acquire));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    usleep(100000U);
    status = dcc_hot_reload_poll(hot_reload);
    if (status != DCC_OK ||
        dcc_hot_reload_generation(hot_reload) != 2 ||
        atomic_load_explicit(&state.init_v2, memory_order_acquire) != 1U ||
        atomic_load_explicit(&state.deinit_v1, memory_order_acquire) != 1U) {
        fprintf(stderr,
                "settle second poll did not reload: status=%s gen=%llu init2=%u deinit1=%u err=%s\n",
                dcc_status_string(status),
                (unsigned long long)dcc_hot_reload_generation(hot_reload),
                atomic_load_explicit(&state.init_v2, memory_order_acquire),
                atomic_load_explicit(&state.deinit_v1, memory_order_acquire),
                dcc_hot_reload_last_error(hot_reload));
        dcc_hot_reload_destroy(hot_reload);
        dcc_client_destroy(client);
        unlink(path);
        return 1;
    }

    dcc_hot_reload_destroy(hot_reload);
    dcc_client_destroy(client);
    unlink(path);
    return atomic_load_explicit(&state.deinit_v2, memory_order_acquire) == 1U ? 0 : 1;
}

#endif
