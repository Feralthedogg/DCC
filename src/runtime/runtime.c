#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

#include <stdatomic.h>

static atomic_flag dcc_runtime_global_lock = ATOMIC_FLAG_INIT;
static unsigned dcc_runtime_global_refs;

static void dcc_runtime_global_lock_acquire(void) {
    while (atomic_flag_test_and_set_explicit(&dcc_runtime_global_lock, memory_order_acquire)) {
    }
}

static void dcc_runtime_global_lock_release(void) {
    atomic_flag_clear_explicit(&dcc_runtime_global_lock, memory_order_release);
}

dcc_status_t dcc_runtime_init(dcc_runtime_t *runtime) {
    if (runtime == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (runtime->initialized) {
        return DCC_OK;
    }

    dcc_runtime_global_lock_acquire();
    if (dcc_runtime_global_refs == 0) {
        llam_runtime_opts_t opts;
        if (llam_runtime_opts_init(&opts, LLAM_RUNTIME_OPTS_CURRENT_SIZE) != 0) {
            dcc_runtime_global_lock_release();
            return DCC_ERR_RUNTIME;
        }
        opts.profile = LLAM_RUNTIME_PROFILE_IO_LATENCY;

        if (llam_runtime_init_ex(&opts, LLAM_RUNTIME_OPTS_CURRENT_SIZE) != 0) {
            dcc_runtime_global_lock_release();
            return DCC_ERR_RUNTIME;
        }
    }

    dcc_runtime_global_refs++;
    runtime->initialized = 1;
    runtime->tasks_spawned = 0;
    dcc_runtime_global_lock_release();
    return DCC_OK;
}

void dcc_runtime_shutdown(dcc_runtime_t *runtime) {
    if (runtime == NULL || !runtime->initialized) {
        return;
    }
    dcc_runtime_global_lock_acquire();
    if (dcc_runtime_global_refs > 0) {
        dcc_runtime_global_refs--;
        if (dcc_runtime_global_refs == 0) {
            llam_runtime_shutdown();
        }
    }
    runtime->initialized = 0;
    dcc_runtime_global_lock_release();
}
