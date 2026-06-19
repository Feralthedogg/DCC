#ifndef DCC_CLUSTER_REMEDIATION_LOOP_INTERNAL_H
#define DCC_CLUSTER_REMEDIATION_LOOP_INTERNAL_H

#include <dcc/cluster/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_cluster_remediation_loop_result_init(
    dcc_cluster_remediation_loop_result_t *result
);
dcc_status_t dcc_cluster_remediation_loop_finish(
    dcc_cluster_remediation_loop_result_t *out,
    const dcc_cluster_remediation_loop_result_t *result,
    dcc_status_t status
);
dcc_status_t dcc_cluster_remediation_loop_delay(dcc_cluster_t *cluster, uint32_t delay_ms);
dcc_status_t dcc_cluster_remediation_loop_options_effective(
    const dcc_cluster_remediation_loop_options_t *options,
    dcc_cluster_remediation_loop_options_t *effective
);

#ifdef __cplusplus
}
#endif

#endif
