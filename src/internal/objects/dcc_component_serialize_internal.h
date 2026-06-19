#ifndef DCC_COMPONENT_SERIALIZE_INTERNAL_H
#define DCC_COMPONENT_SERIALIZE_INTERNAL_H

#include "internal/objects/dcc_component_json_members_internal.h"

#include <dcc/component.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_component_append_emoji_json(
    const dcc_component_emoji_t *emoji,
    dcc_component_json_buffer_t *buffer
);
dcc_status_t dcc_component_append_action_row_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_component_append_button_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_component_append_text_input_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_component_append_select_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
);
dcc_status_t dcc_component_builder_append_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer
);

#ifdef __cplusplus
}
#endif

#endif
