#ifndef DCC_COMPONENT_V2_INTERNAL_H
#define DCC_COMPONENT_V2_INTERNAL_H

#include "internal/objects/dcc_component_json_members_internal.h"

#include <dcc/component_v2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_component_v2_validation {
    const char *custom_ids[DCC_COMPONENT_V2_MAX_COMPONENTS];
    uint32_t ids[DCC_COMPONENT_V2_MAX_COMPONENTS];
    size_t custom_id_count;
    size_t id_count;
    size_t total_count;
} dcc_component_v2_validation_t;

typedef enum dcc_component_v2_context {
    DCC_COMPONENT_V2_CONTEXT_ANY = 0,
    DCC_COMPONENT_V2_CONTEXT_MESSAGE,
    DCC_COMPONENT_V2_CONTEXT_MODAL
} dcc_component_v2_context_t;

dcc_status_t dcc_component_v2_validate_array(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count
);
dcc_status_t dcc_component_v2_validate_array_context(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_v2_context_t context
);
dcc_status_t dcc_component_v2_append_json(
    const dcc_component_v2_builder_t *builder,
    dcc_component_json_buffer_t *buffer
);
dcc_status_t dcc_component_v2_append_array_json(
    const dcc_component_v2_builder_t *builders,
    size_t builder_count,
    dcc_component_json_buffer_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif
