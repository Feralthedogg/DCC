#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>

dcc_integration_t *dcc_integration_clone(const dcc_integration_t *integration) {
    if (integration == NULL) {
        return NULL;
    }

    dcc_integration_t *copy = (dcc_integration_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *integration;
    copy->name = dcc_clone_string_or_null(integration->name);
    copy->type_text = dcc_clone_string_or_null(integration->type_text);
    if ((integration->name != NULL && copy->name == NULL) ||
        (integration->type_text != NULL && copy->type_text == NULL)) {
        dcc_integration_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_integration_free(dcc_integration_t *integration) {
    if (integration == NULL) {
        return;
    }
    free((void *)integration->name);
    free((void *)integration->type_text);
    free(integration);
}
