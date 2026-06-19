#ifndef DCC_CLUSTER_WAIT_HEALTH_INTERNAL_H
#define DCC_CLUSTER_WAIT_HEALTH_INTERNAL_H

#include <dcc/cluster.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t dcc_cluster_health_wait_now_ms(void);
void dcc_cluster_health_wait_sleep_ms(uint32_t ms);
int dcc_cluster_health_wait_valid_target(dcc_cluster_health_state_t state);
dcc_status_t dcc_cluster_health_wait_effective_options(
    const dcc_cluster_health_wait_options_t *options,
    dcc_cluster_health_wait_options_t *effective
);

#ifdef __cplusplus
}
#endif

#endif
