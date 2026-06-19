#ifndef DCC_INTERACTION_OBJECT_FIELDS_INTERNAL_H
#define DCC_INTERACTION_OBJECT_FIELDS_INTERNAL_H

#include <dcc/objects.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_clone_interaction_option_array(
    const dcc_interaction_option_t *src,
    size_t count,
    dcc_interaction_option_t **out,
    const dcc_interaction_option_t *src_focused,
    const dcc_interaction_option_t **copy_focused
);
void dcc_free_interaction_option_array(dcc_interaction_option_t *options, size_t count);
int dcc_clone_interaction_resolved_fields(
    dcc_interaction_resolved_t *copy,
    const dcc_interaction_resolved_t *resolved
);
void dcc_free_interaction_resolved_fields(dcc_interaction_resolved_t *resolved);

#ifdef __cplusplus
}
#endif

#endif
