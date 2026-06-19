#ifndef DCC_HOT_RELOAD_SIDECAR_H
#define DCC_HOT_RELOAD_SIDECAR_H

#include <dcc/hot_reload/base.h>
#include <dcc/interaction_server/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_hot_reload_add_health_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_hot_reload_t *hot_reload
);

#ifdef __cplusplus
}
#endif

#endif
