#ifndef DCC_AUTOCOMPLETE_BUILDER_INTERNAL_H
#define DCC_AUTOCOMPLETE_BUILDER_INTERNAL_H

#include "internal/objects/dcc_autocomplete_json_members_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <dcc/autocomplete.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_autocomplete_append_choice(
    const dcc_autocomplete_choice_t *choice,
    dcc_autocomplete_json_buffer_t *buffer
);
dcc_status_t dcc_autocomplete_choice_semantic_validate(
    const dcc_autocomplete_choice_t *choice,
    dcc_builder_abi_view_t *out_view
);

#ifdef __cplusplus
}
#endif

#endif
