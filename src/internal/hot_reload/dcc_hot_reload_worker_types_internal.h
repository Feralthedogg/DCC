#ifndef DCC_HOT_RELOAD_WORKER_TYPES_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_TYPES_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_state_internal.h"
#include "internal/hot_reload/dcc_hot_reload_worker_protocol_internal.h"

#include <stddef.h>
#include <stdint.h>

#if !defined(_WIN32)
#include <pthread.h>
#include <sys/types.h>
#endif

struct dcc_hot_reload_worker_process {
#if !defined(_WIN32)
    pid_t pid;
#else
    intptr_t process;
#endif
    int in_fd;
    int out_fd;
    uint64_t process_id;
    uint64_t generation;
    char last_error[DCC_HOT_RELOAD_WORKER_MAX_ERROR_LEN];
    uint8_t loaded;
    uint8_t io_lock_initialized;
#if defined(_WIN32)
    CRITICAL_SECTION io_lock;
#else
    pthread_mutex_t io_lock;
#endif
};

typedef struct dcc_hot_reload_worker_rest_entry {
    char *method;
    char *path;
    char *content_type;
    char *body;
    size_t body_len;
} dcc_hot_reload_worker_rest_entry_t;

typedef struct dcc_hot_reload_worker_result_set {
    dcc_status_t status;
    dcc_hot_reload_worker_rest_entry_t *items;
    size_t count;
} dcc_hot_reload_worker_result_set_t;

typedef struct dcc_hot_reload_worker_capture {
    dcc_hot_reload_worker_rest_entry_t *items;
    size_t count;
    size_t cap;
} dcc_hot_reload_worker_capture_t;

#endif
