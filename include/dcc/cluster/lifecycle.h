#ifndef DCC_CLUSTER_LIFECYCLE_H
#define DCC_CLUSTER_LIFECYCLE_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_create(const dcc_cluster_options_t *options, dcc_cluster_t **out);
DCC_API void dcc_cluster_destroy(dcc_cluster_t *cluster);

DCC_API dcc_status_t dcc_cluster_start(dcc_cluster_t *cluster);
DCC_API dcc_status_t dcc_cluster_stop(dcc_cluster_t *cluster);
DCC_API dcc_status_t dcc_cluster_wait(dcc_cluster_t *cluster);

#ifdef __cplusplus
}
#endif

#endif
