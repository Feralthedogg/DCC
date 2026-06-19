#ifndef DCC_CLUSTER_SIDECAR_H
#define DCC_CLUSTER_SIDECAR_H

#include <dcc/cluster/types.h>
#include <dcc/interaction_server/base.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_add_status_route(
    dcc_interaction_server_t *server,
    const char *path,
    dcc_cluster_t *cluster
);

#ifdef __cplusplus
}
#endif

#endif
