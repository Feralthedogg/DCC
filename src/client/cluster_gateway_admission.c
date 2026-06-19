#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_set_gateway_admission(dcc_cluster_t *cluster, uint8_t open) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    atomic_store_explicit(&cluster->gateway_admission_open, open != 0, memory_order_release);
    return DCC_OK;
}

dcc_status_t dcc_cluster_gateway_admission(const dcc_cluster_t *cluster, uint8_t *out_open) {
    if (cluster == NULL || out_open == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_open = dcc_cluster_gateway_admission_open(cluster) ? 1U : 0U;
    return DCC_OK;
}
