#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

dcc_status_t dcc_runtime_spawn(dcc_runtime_t *runtime, void (*fn)(void *), void *arg) {
    if (runtime == NULL || fn == NULL || !runtime->initialized) {
        return DCC_ERR_INVALID_ARG;
    }

    llam_spawn_opts_t opts;
    if (llam_spawn_opts_init(&opts, LLAM_SPAWN_OPTS_CURRENT_SIZE) != 0) {
        return DCC_ERR_RUNTIME;
    }
    opts.task_class = LLAM_TASK_CLASS_DEFAULT;
#if defined(DCC_ENABLE_SANITIZERS)
    opts.stack_class = LLAM_STACK_CLASS_HUGE;
#else
    opts.stack_class = LLAM_STACK_CLASS_LARGE;
#endif

    llam_task_t *task = llam_spawn_ex(fn, arg, &opts, LLAM_SPAWN_OPTS_CURRENT_SIZE);
    if (task == NULL) {
        return DCC_ERR_RUNTIME;
    }
    if (llam_detach(task) != 0) {
        return DCC_ERR_RUNTIME;
    }

    runtime->tasks_spawned++;
    return DCC_OK;
}
