#ifndef DCC_HOT_RELOAD_WORKER_CHILD_INTERNAL_H
#define DCC_HOT_RELOAD_WORKER_CHILD_INTERNAL_H

#include "internal/hot_reload/dcc_hot_reload_worker_types_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int dcc_hot_reload_worker_main(int argc, char **argv);
int dcc_hot_reload_worker_child_event_loop(dcc_hot_reload_t *hot_reload);

#ifdef __cplusplus
}
#endif

#endif
