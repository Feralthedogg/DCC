#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

typedef struct dcc_worker_client_wait_state {
    dcc_client_t *client;
    dcc_status_t status;
} dcc_worker_client_wait_state_t;

#if defined(_WIN32)
typedef HANDLE dcc_worker_client_wait_thread_t;

static DWORD WINAPI dcc_worker_client_wait_main(void *arg) {
    dcc_worker_client_wait_state_t *state =
        (dcc_worker_client_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return 0U;
}

static int dcc_worker_client_wait_start(
    dcc_worker_client_wait_thread_t *thread,
    dcc_worker_client_wait_state_t *state
) {
    *thread = CreateThread(
        NULL,
        0U,
        dcc_worker_client_wait_main,
        state,
        0U,
        NULL
    );
    return *thread != NULL ? 0 : -1;
}

static dcc_status_t dcc_worker_client_wait_join(
    dcc_worker_client_wait_thread_t thread,
    dcc_worker_client_wait_state_t *state
) {
    if (WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0) {
        (void)CloseHandle(thread);
        return DCC_ERR_RUNTIME;
    }
    (void)CloseHandle(thread);
    return state->status;
}
#else
typedef pthread_t dcc_worker_client_wait_thread_t;

static void *dcc_worker_client_wait_main(void *arg) {
    dcc_worker_client_wait_state_t *state =
        (dcc_worker_client_wait_state_t *)arg;
    state->status = dcc_client_wait(state->client);
    return NULL;
}

static int dcc_worker_client_wait_start(
    dcc_worker_client_wait_thread_t *thread,
    dcc_worker_client_wait_state_t *state
) {
    return pthread_create(thread, NULL, dcc_worker_client_wait_main, state);
}

static dcc_status_t dcc_worker_client_wait_join(
    dcc_worker_client_wait_thread_t thread,
    dcc_worker_client_wait_state_t *state
) {
    return pthread_join(thread, NULL) == 0 ? state->status : DCC_ERR_RUNTIME;
}
#endif

static int dcc_worker_parse_generation(const char *text, uint64_t *out) {
    if (text == NULL || text[0] == '\0' || out == NULL) {
        return -1;
    }
    errno = 0;
    char *end = NULL;
    unsigned long long value = strtoull(text, &end, 10);
    if (errno == ERANGE || end == text || end == NULL || *end != '\0' || value == 0ULL) {
        return -1;
    }
    if (value > (unsigned long long)UINT64_MAX) {
        return -1;
    }
    *out = (uint64_t)value;
    return 0;
}

static int dcc_worker_send_ready(dcc_hot_reload_t *hot_reload, dcc_status_t status) {
    dcc_hot_reload_worker_ready_t ready = {
        .generation = hot_reload != NULL ? dcc_hot_reload_generation(hot_reload) : 0U,
        .status = (uint32_t)status,
        .handler_count = hot_reload != NULL ? (uint32_t)hot_reload->module.handler_count : 0U,
    };
    dcc_hot_reload_copy_text(
        ready.last_error,
        sizeof(ready.last_error),
        hot_reload != NULL && hot_reload->last_error[0] != '\0'
            ? hot_reload->last_error
            : (status == DCC_OK ? "" : dcc_status_string(status))
    );
    if (dcc_hot_reload_worker_send_header_timeout(
            DCC_HOT_RELOAD_WORKER_FD_OUT,
            DCC_HOT_RELOAD_WORKER_MSG_READY,
            sizeof(ready),
            DCC_HOT_RELOAD_WORKER_CHILD_WRITE_TIMEOUT_MS
        ) != 0 ||
        dcc_hot_reload_worker_write_all_timeout(
            DCC_HOT_RELOAD_WORKER_FD_OUT,
            &ready,
            sizeof(ready),
            DCC_HOT_RELOAD_WORKER_CHILD_WRITE_TIMEOUT_MS
        ) != 0) {
        return -1;
    }
    for (size_t i = 0; hot_reload != NULL && i < hot_reload->module.handler_count; ++i) {
        int32_t type = (int32_t)hot_reload->module.handlers[i].type;
        if (dcc_hot_reload_worker_write_all_timeout(
                DCC_HOT_RELOAD_WORKER_FD_OUT,
                &type,
                sizeof(type),
                DCC_HOT_RELOAD_WORKER_CHILD_WRITE_TIMEOUT_MS
            ) != 0) {
            return -1;
        }
    }
    return 0;
}

int dcc_hot_reload_worker_main(int argc, char **argv) {
    if (argc < 5) {
        return 2;
    }
    uint64_t generation = 0U;
    if (dcc_worker_parse_generation(argv[4], &generation) != 0) {
        return 2;
    }
    dcc_client_options_t client_options;
    memset(&client_options, 0, sizeof(client_options));
    client_options.size = sizeof(client_options);
    client_options.token = "";

    dcc_client_t *client = NULL;
    dcc_status_t status = dcc_client_create(&client_options, &client);
    uint8_t client_started = 0U;
    uint8_t wait_thread_started = 0U;
    dcc_worker_client_wait_state_t wait_state = {
        .client = client,
        .status = DCC_ERR_RUNTIME,
    };
    dcc_worker_client_wait_thread_t wait_thread;
    if (status == DCC_OK) {
        status = dcc_client_start(client);
        if (status == DCC_OK) {
            client_started = 1U;
        }
    }
    if (status == DCC_OK) {
        if (dcc_worker_client_wait_start(&wait_thread, &wait_state) != 0) {
            status = DCC_ERR_RUNTIME;
        } else {
            wait_thread_started = 1U;
        }
    }
    dcc_hot_reload_t *hot_reload = NULL;
    if (status == DCC_OK) {
        dcc_hot_reload_options_t options;
        memset(&options, 0, sizeof(options));
        options.size = sizeof(options);
        options.init_symbol = argv[2];
        options.deinit_symbol = argv[3];
        status = dcc_hot_reload_create(client, argv[1], &options, &hot_reload);
    }
    if (status == DCC_OK) {
        hot_reload->generation = generation - 1U;
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (dcc_worker_send_ready(hot_reload, status) != 0) {
        status = DCC_ERR_RUNTIME;
    }
    int rc = status == DCC_OK ? dcc_hot_reload_worker_child_event_loop(hot_reload) : 1;
    dcc_hot_reload_destroy(hot_reload);
    if (client_started != 0U) {
        (void)dcc_client_stop(client);
        dcc_status_t wait_status = wait_thread_started != 0U
            ? dcc_worker_client_wait_join(wait_thread, &wait_state)
            : dcc_client_wait(client);
        if (wait_status != DCC_OK) {
            rc = 1;
        }
    }
    dcc_client_destroy(client);
    return rc;
}
