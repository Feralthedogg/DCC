#ifndef DCC_GATEWAY_EVENT_DATA_COLLECTIONS_INTERNAL_H
#define DCC_GATEWAY_EVENT_DATA_COLLECTIONS_INTERNAL_H

#include <dcc/objects.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_gateway_clone_snowflake_collection(
    const dcc_snowflake_t *src,
    size_t count,
    const dcc_snowflake_t **out
);
void dcc_gateway_activity_free_fields(dcc_gateway_activity_t *activity);
int dcc_gateway_activity_clone_fields(
    dcc_gateway_activity_t *copy,
    const dcc_gateway_activity_t *activity
);
int dcc_gateway_clone_thread_member_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
);
int dcc_gateway_clone_member_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
);
int dcc_gateway_clone_activity_collection(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
);

#ifdef __cplusplus
}
#endif

#endif
