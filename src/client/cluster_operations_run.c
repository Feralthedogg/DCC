#include "internal/client/dcc_cluster_internal.h"

static dcc_status_t dcc_cluster_delay_ms(dcc_cluster_t *cluster, uint32_t delay_ms) {
    uint32_t remaining = delay_ms;
    while (remaining > 0 && cluster != NULL && cluster->started) {
        uint32_t slice = remaining > 100U ? 100U : remaining;
        (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
        remaining -= slice;
    }
    return cluster != NULL && cluster->started ? DCC_OK : DCC_ERR_CANCELED;
}

dcc_status_t dcc_cluster_run_planned_operation(dcc_cluster_t *cluster) {
    if (cluster == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!cluster->started || !dcc_cluster_gateway_admission_open(cluster)) {
        return DCC_ERR_STATE;
    }

    dcc_cluster_health_lock(cluster);
    if (cluster->operation.active ||
        cluster->operation.type != DCC_CLUSTER_OPERATION_ROLLING_RECONNECT) {
        dcc_cluster_health_unlock(cluster);
        return DCC_ERR_STATE;
    }

    uint32_t first = cluster->operation.first_index;
    uint32_t count = cluster->operation.shard_count;
    uint32_t batch_size = cluster->operation.batch_size;
    uint32_t delay_ms = cluster->operation.delay_ms;
    uint8_t resume = cluster->operation.resume;
    if (first >= cluster->shard_count || count == 0 || count > cluster->shard_count - first ||
        batch_size == 0) {
        cluster->operation.last_status = DCC_ERR_INVALID_ARG;
        dcc_cluster_health_unlock(cluster);
        return DCC_ERR_INVALID_ARG;
    }

    cluster->operation.active = 1U;
    cluster->operation.completed = 0;
    cluster->operation.next_index = first;
    cluster->operation.completed_shards = 0;
    cluster->operation.failed_shards = 0;
    cluster->operation.current_batch = 0;
    cluster->operation.total_batches = dcc_cluster_rolling_total_batches(count, batch_size);
    cluster->operation.last_status = DCC_OK;
    dcc_cluster_health_unlock(cluster);

    dcc_status_t first_error = DCC_OK;
    for (uint32_t offset = 0; offset < count; ++offset) {
        uint32_t index = first + offset;
        dcc_cluster_health_lock(cluster);
        cluster->operation.current_batch = offset / batch_size + 1U;
        cluster->operation.next_index = index;
        dcc_cluster_health_unlock(cluster);

        dcc_status_t status = dcc_cluster_reconnect_shard(cluster, index, resume);
        if (first_error == DCC_OK && status != DCC_OK) {
            first_error = status;
        }

        dcc_cluster_health_lock(cluster);
        if (status == DCC_OK) {
            cluster->operation.completed_shards++;
        } else {
            cluster->operation.failed_shards++;
            if (cluster->operation.last_status == DCC_OK) {
                cluster->operation.last_status = status;
            }
        }
        cluster->operation.next_index = index + 1U;
        dcc_cluster_health_unlock(cluster);

        if ((offset + 1U) < count &&
            ((offset + 1U) % batch_size) == 0U &&
            delay_ms != 0) {
            dcc_status_t delay_status = dcc_cluster_delay_ms(cluster, delay_ms);
            if (first_error == DCC_OK && delay_status != DCC_OK) {
                first_error = delay_status;
            }
            if (delay_status != DCC_OK) {
                dcc_cluster_health_lock(cluster);
                if (cluster->operation.last_status == DCC_OK) {
                    cluster->operation.last_status = delay_status;
                }
                dcc_cluster_health_unlock(cluster);
                break;
            }
        }
    }

    dcc_cluster_health_lock(cluster);
    cluster->operation.active = 0;
    cluster->operation.completed = 1U;
    cluster->operation.last_status = first_error;
    dcc_cluster_health_unlock(cluster);
    return first_error;
}
