#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_entitlement_fields(dcc_entitlement_t *entitlement) {
    if (entitlement == NULL) {
        return;
    }
    free((void *)entitlement->start_time);
    free((void *)entitlement->end_time);
    entitlement->start_time = NULL;
    entitlement->end_time = NULL;
}

int dcc_clone_entitlement_fields(dcc_entitlement_t *copy, const dcc_entitlement_t *entitlement) {
    if (copy == NULL || entitlement == NULL) {
        return -1;
    }

    *copy = *entitlement;
    copy->start_time = dcc_clone_string_or_null(entitlement->start_time);
    copy->end_time = dcc_clone_string_or_null(entitlement->end_time);
    if ((entitlement->start_time != NULL && copy->start_time == NULL) ||
        (entitlement->end_time != NULL && copy->end_time == NULL)) {
        dcc_free_entitlement_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

dcc_entitlement_t *dcc_entitlement_clone(const dcc_entitlement_t *entitlement) {
    if (entitlement == NULL) {
        return NULL;
    }

    dcc_entitlement_t *copy = (dcc_entitlement_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_entitlement_fields(copy, entitlement) != 0) {
        dcc_entitlement_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_entitlement_free(dcc_entitlement_t *entitlement) {
    if (entitlement == NULL) {
        return;
    }
    dcc_free_entitlement_fields(entitlement);
    free(entitlement);
}
