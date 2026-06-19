#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

int dcc_task_option_has(size_t size, size_t offset, size_t field_size) {
    return offset <= size && field_size <= size - offset;
}

static uint32_t dcc_task_group_map_class(dcc_task_class_t task_class) {
    switch (task_class) {
        case DCC_TASK_CLASS_LATENCY:
            return LLAM_TASK_CLASS_LATENCY;
        case DCC_TASK_CLASS_BATCH:
            return LLAM_TASK_CLASS_BATCH;
        case DCC_TASK_CLASS_DEFAULT:
        default:
            return LLAM_TASK_CLASS_DEFAULT;
    }
}

static uint32_t dcc_task_group_map_flags(uint32_t flags) {
    uint32_t mapped = 0;
    if ((flags & (uint32_t)DCC_TASK_F_PINNED) != 0U) {
        mapped |= LLAM_SPAWN_F_PINNED;
    }
    if ((flags & (uint32_t)DCC_TASK_F_NO_PREEMPT) != 0U) {
        mapped |= LLAM_SPAWN_F_NO_PREEMPT;
    }
    if ((flags & (uint32_t)DCC_TASK_F_LATENCY_CRITICAL) != 0U) {
        mapped |= LLAM_SPAWN_F_LATENCY_CRITICAL;
    }
    return mapped;
}

dcc_status_t dcc_task_group_build_opts(const dcc_task_options_t *options, llam_spawn_opts_t *out) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (llam_spawn_opts_init(out, LLAM_SPAWN_OPTS_CURRENT_SIZE) != 0) {
        return dcc_task_group_status_from_errno(errno);
    }

    out->task_class = LLAM_TASK_CLASS_DEFAULT;
#if defined(DCC_ENABLE_SANITIZERS)
    out->stack_class = LLAM_STACK_CLASS_HUGE;
#else
    out->stack_class = LLAM_STACK_CLASS_LARGE;
#endif

    if (options == NULL) {
        return DCC_OK;
    }

    if (dcc_task_option_has(options->size, offsetof(dcc_task_options_t, task_class), sizeof(options->task_class))) {
        out->task_class = dcc_task_group_map_class(options->task_class);
    }
    if (dcc_task_option_has(options->size, offsetof(dcc_task_options_t, flags), sizeof(options->flags))) {
        out->flags = dcc_task_group_map_flags(options->flags);
    }
    if (dcc_task_option_has(options->size, offsetof(dcc_task_options_t, deadline_ns), sizeof(options->deadline_ns))) {
        out->deadline_ns = options->deadline_ns;
    }

    return DCC_OK;
}
