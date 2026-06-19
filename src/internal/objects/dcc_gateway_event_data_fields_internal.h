#ifndef DCC_GATEWAY_EVENT_DATA_FIELDS_INTERNAL_H
#define DCC_GATEWAY_EVENT_DATA_FIELDS_INTERNAL_H

#include <dcc/objects.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_gateway_event_data_clear_owned_refs(dcc_gateway_event_data_t *data);
int dcc_gateway_event_data_clone_strings(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
);
void dcc_gateway_event_data_free_strings(dcc_gateway_event_data_t *data);
int dcc_gateway_event_data_clone_collections(
    dcc_gateway_event_data_t *copy,
    const dcc_gateway_event_data_t *data
);
void dcc_gateway_event_data_free_collections(dcc_gateway_event_data_t *data);

#ifdef __cplusplus
}
#endif

#endif
