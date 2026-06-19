#ifndef DCC_CLUSTER_EVENTS_INTERNAL_H
#define DCC_CLUSTER_EVENTS_INTERNAL_H

#include "internal/client/dcc_cluster_state_internal.h"

#include <dcc/cluster.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_cluster_register_health_events(dcc_cluster_t *cluster, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
