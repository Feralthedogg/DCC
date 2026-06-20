#ifndef DCC_INTERACTION_CLONE_INTERNAL_H
#define DCC_INTERACTION_CLONE_INTERNAL_H

#include "internal/objects/dcc_interaction_object_fields_internal.h"
#include "internal/objects/dcc_object_clone_helpers_internal.h"
#include "internal/objects/dcc_object_resource_fields_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

int dcc_clone_interaction_string_fields(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
);

int dcc_clone_interaction_values(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
);

int dcc_clone_interaction_form_fields(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
);

int dcc_clone_interaction_options(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
);

int dcc_clone_interaction_collections(
    dcc_interaction_t *copy,
    const dcc_interaction_t *interaction
);

#ifdef __cplusplus
}
#endif

#endif
