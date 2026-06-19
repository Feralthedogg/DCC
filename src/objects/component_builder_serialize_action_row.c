#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_append_action_row_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    if (builder->children_count == 0) {
        return DCC_OK;
    }

    dcc_status_t status = dcc_component_json_member_prefix(buffer, first, "components");
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "[");
    }
    for (size_t i = 0; status == DCC_OK && i < builder->children_count; ++i) {
        if (i != 0) {
            status = dcc_component_json_append_cstr(buffer, ",");
        }
        if (status == DCC_OK) {
            status = dcc_component_builder_append_json(&builder->children[i], buffer);
        }
    }
    if (status == DCC_OK) {
        status = dcc_component_json_append_cstr(buffer, "]");
    }
    return status;
}
