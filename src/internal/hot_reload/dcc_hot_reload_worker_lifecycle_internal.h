#ifndef DCC_HOT_RELOAD_WORKER_LIFECYCLE_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_LIFECYCLE_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_hot_reload_worker_reload(dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_worker_destroy_all(dcc_hot_reload_t *hot_reload);
uint8_t dcc_hot_reload_worker_loaded(const dcc_hot_reload_t *hot_reload);

#ifdef __cplusplus
}
#endif

#endif
