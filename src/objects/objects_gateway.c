#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_gateway_event_data_t *dcc_gateway_event_data_clone(const dcc_gateway_event_data_t *data) {
    if (data == NULL) {
        return NULL;
    }

    dcc_gateway_event_data_t *copy = (dcc_gateway_event_data_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *data;
    dcc_gateway_event_data_clear_owned_refs(copy);
    if (dcc_gateway_event_data_clone_strings(copy, data) != 0 ||
        dcc_gateway_event_data_clone_collections(copy, data) != 0) {
        dcc_gateway_event_data_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_gateway_event_data_free(dcc_gateway_event_data_t *data) {
    if (data == NULL) {
        return;
    }
    dcc_gateway_event_data_free_strings(data);
    dcc_gateway_event_data_free_collections(data);
    free(data);
}
