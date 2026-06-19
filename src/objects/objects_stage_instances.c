#include "internal/objects/dcc_objects_internal.h"

#include <stdlib.h>
#include <string.h>

void dcc_free_stage_instance_fields(dcc_stage_instance_t *stage_instance) {
    if (stage_instance == NULL) {
        return;
    }
    free((void *)stage_instance->topic);
    stage_instance->topic = NULL;
}

int dcc_clone_stage_instance_fields(dcc_stage_instance_t *copy, const dcc_stage_instance_t *stage_instance) {
    if (copy == NULL || stage_instance == NULL) {
        return -1;
    }

    *copy = *stage_instance;
    copy->topic = dcc_clone_string_or_null(stage_instance->topic);
    if (stage_instance->topic != NULL && copy->topic == NULL) {
        dcc_free_stage_instance_fields(copy);
        memset(copy, 0, sizeof(*copy));
        return -1;
    }

    return 0;
}

dcc_stage_instance_t *dcc_stage_instance_clone(const dcc_stage_instance_t *stage_instance) {
    if (stage_instance == NULL) {
        return NULL;
    }

    dcc_stage_instance_t *copy = (dcc_stage_instance_t *)calloc(1, sizeof(*copy));
    if (copy == NULL) {
        return NULL;
    }

    if (dcc_clone_stage_instance_fields(copy, stage_instance) != 0) {
        dcc_stage_instance_free(copy);
        return NULL;
    }

    return copy;
}

void dcc_stage_instance_free(dcc_stage_instance_t *stage_instance) {
    if (stage_instance == NULL) {
        return;
    }
    dcc_free_stage_instance_fields(stage_instance);
    free(stage_instance);
}
