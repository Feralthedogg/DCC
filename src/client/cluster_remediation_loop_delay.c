#include "internal/client/dcc_cluster_internal.h"

dcc_status_t dcc_cluster_remediation_loop_delay(dcc_cluster_t *cluster, uint32_t delay_ms) {
    uint32_t remaining = delay_ms;
    while (remaining > 0) {
        if (cluster != NULL && !cluster->started) {
            return DCC_ERR_CANCELED;
        }
        uint32_t slice = remaining > 100U ? 100U : remaining;
        (void)llam_sleep_ns((uint64_t)slice * 1000000ULL);
        remaining -= slice;
    }
    return DCC_OK;
}
