#ifndef DCC_CLUSTER_START_INTERNAL_H
#define DCC_CLUSTER_START_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_cluster_check_initial_identify_budget(dcc_cluster_t *cluster);
dcc_status_t dcc_cluster_init_shard_runtimes(dcc_cluster_t *cluster);
void dcc_cluster_shutdown_shard_runtimes(dcc_cluster_t *cluster);

#ifdef __cplusplus
}
#endif

#endif
