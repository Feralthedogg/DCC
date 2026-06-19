#include "internal/client/dcc_cluster_internal.h"

uint32_t dcc_cluster_rolling_total_batches(uint32_t count, uint32_t batch_size) {
    if (count == 0 || batch_size == 0) {
        return 0;
    }
    return count / batch_size + (count % batch_size != 0 ? 1U : 0U);
}

dcc_status_t dcc_cluster_rolling_reconnect(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options
) {
    uint32_t first = 0;
    uint32_t count = 0;
    uint32_t batch_size = 0;
    dcc_status_t status =
        dcc_cluster_normalize_rolling_options(cluster, options, &first, &count, &batch_size);
    if (status != DCC_OK) {
        return status;
    }
    if (!cluster->started || !dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    status = dcc_cluster_store_rolling_plan(cluster, options, first, count, batch_size);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_cluster_run_planned_operation(cluster);
}

dcc_status_t dcc_cluster_plan_rolling_reconnect(
    dcc_cluster_t *cluster,
    const dcc_cluster_rolling_reconnect_options_t *options
) {
    uint32_t first = 0;
    uint32_t count = 0;
    uint32_t batch_size = 0;
    dcc_status_t status =
        dcc_cluster_normalize_rolling_options(cluster, options, &first, &count, &batch_size);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_cluster_store_rolling_plan(cluster, options, first, count, batch_size);
}
