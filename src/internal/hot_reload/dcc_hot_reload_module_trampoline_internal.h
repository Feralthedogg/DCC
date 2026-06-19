#ifndef DCC_HOT_RELOAD_MODULE_TRAMPOLINE_INTERNAL_H
#define DCC_HOT_RELOAD_MODULE_TRAMPOLINE_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_state_internal.h"

#include <dcc/hot_reload.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_install_trampoline(dcc_hot_reload_t *hot_reload, dcc_event_type_t type);

#ifdef __cplusplus
}
#endif

#endif
