#ifndef DCC_CLUSTER_REMEDIATION_INTERNAL_H
#define DCC_CLUSTER_REMEDIATION_INTERNAL_H

#include <dcc/cluster/health.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_cluster_remediation_has(
    const dcc_cluster_remediation_options_t *options,
    size_t offset,
    size_t size
);
void dcc_cluster_remediation_result_init(dcc_cluster_remediation_result_t *result);
dcc_status_t dcc_cluster_remediation_finish(
    dcc_cluster_remediation_result_t *out,
    const dcc_cluster_remediation_result_t *result,
    dcc_status_t status
);

#ifdef __cplusplus
}
#endif

#endif
