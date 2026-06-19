#ifndef DCC_CLUSTER_CONFIG_INTERNAL_H
#define DCC_CLUSTER_CONFIG_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"

#include <dcc/cluster.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t dcc_cluster_effective_concurrency(const dcc_cluster_options_t *options);
dcc_status_t dcc_cluster_identify_delay(
    uint32_t index,
    uint32_t max_concurrency,
    uint32_t *out_delay_ms
);
dcc_status_t dcc_cluster_apply_identify_delays(dcc_cluster_t *cluster);

#ifdef __cplusplus
}
#endif

#endif
