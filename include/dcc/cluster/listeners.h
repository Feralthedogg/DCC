#ifndef DCC_CLUSTER_LISTENERS_H
#define DCC_CLUSTER_LISTENERS_H

#include <dcc/cluster/base.h>
#include <dcc/events/listeners.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_cluster_on(
    dcc_cluster_t *cluster,
    dcc_event_type_t type,
    dcc_event_cb cb,
    void *user_data,
    dcc_listener_id_t *out_ids,
    size_t out_ids_count
);

#ifdef __cplusplus
}
#endif

#endif
