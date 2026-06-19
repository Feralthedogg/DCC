#ifndef DCC_INTERACTION_RESOLVED_CLONE_INTERNAL_H
#define DCC_INTERACTION_RESOLVED_CLONE_INTERNAL_H

#include "internal/objects/dcc_interaction_object_fields_internal.h"
#include "internal/objects/dcc_object_core_fields_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int dcc_clone_interaction_resolved_people(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
);

int dcc_clone_interaction_resolved_resources(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
);

int dcc_clone_interaction_resolved_files(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
);

#ifdef __cplusplus
}
#endif

#endif
