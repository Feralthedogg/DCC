#ifndef DCC_TASK_GROUP_OPTIONS_INTERNAL_H
#define DCC_TASK_GROUP_OPTIONS_INTERNAL_H

#include "internal/runtime/dcc_task_group_state_internal.h"

#include <dcc/error.h>
#include <dcc/tasks.h>

#include <llam/runtime.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_task_group_status_from_errno(int err);
int dcc_task_option_has(size_t size, size_t offset, size_t field_size);
dcc_status_t dcc_task_group_build_opts(const dcc_task_options_t *options, llam_spawn_opts_t *out);
uint64_t dcc_task_group_deadline_from_timeout(uint32_t timeout_ms);
int dcc_task_group_client_accepts_work(const dcc_client_t *client);
dcc_status_t dcc_task_group_wait_status_from_errno(int err);

#ifdef __cplusplus
}
#endif

#endif
