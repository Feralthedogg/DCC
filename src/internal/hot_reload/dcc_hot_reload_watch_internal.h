#ifndef DCC_HOT_RELOAD_WATCH_INTERNAL_H
#define DCC_HOT_RELOAD_WATCH_INTERNAL_H

#include <dcc/hot_reload.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_hot_reload_watch_task(void *arg);
dcc_status_t dcc_hot_reload_start(dcc_hot_reload_t *hot_reload);
void dcc_hot_reload_request_stop(dcc_hot_reload_t *hot_reload);
dcc_status_t dcc_hot_reload_stop(dcc_hot_reload_t *hot_reload);

#ifdef __cplusplus
}
#endif

#endif
