#ifndef DCC_CLUSTER_GATEWAY_H
#define DCC_CLUSTER_GATEWAY_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_set_gateway_admission(dcc_cluster_t *cluster, uint8_t open);
DCC_API dcc_status_t dcc_cluster_gateway_admission(const dcc_cluster_t *cluster, uint8_t *out_open);
DCC_API dcc_status_t dcc_cluster_fetch_gateway_info(dcc_cluster_t *cluster);
DCC_API dcc_status_t dcc_cluster_gateway_info(const dcc_cluster_t *cluster, dcc_gateway_info_t *out);

#ifdef __cplusplus
}
#endif

#endif
