#ifndef DCC_TASK_GROUP_CANCEL_INTERNAL_H
#define DCC_TASK_GROUP_CANCEL_INTERNAL_H

#include "internal/runtime/dcc_task_group_state_internal.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t dcc_task_group_cancel_requested(dcc_task_group_t *group);

#ifdef __cplusplus
}
#endif

#endif
