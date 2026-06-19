#ifndef DCC_CLUSTER_RECOVERY_INTERNAL_H
#define DCC_CLUSTER_RECOVERY_INTERNAL_H

#include <dcc/cluster.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned dcc_cluster_recovery_action_priority(dcc_cluster_recovery_action_t action);
const char *dcc_cluster_recovery_default_reason(dcc_cluster_recovery_action_t action);

#ifdef __cplusplus
}
#endif

#endif
