#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_builder_append_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_validate(builder);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_component_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    status = dcc_component_json_append_u32_member(buffer, &first, "type", (uint32_t)builder->type);
    if (status != DCC_OK) {
        return status;
    }

    switch (builder->type) {
    case DCC_COMPONENT_ACTION_ROW:
        status = dcc_component_append_action_row_json(builder, buffer, &first);
        break;
    case DCC_COMPONENT_BUTTON:
        status = dcc_component_append_button_json(builder, buffer, &first);
        break;
    case DCC_COMPONENT_TEXT_INPUT:
        status = dcc_component_append_text_input_json(builder, buffer, &first);
        break;
    default:
        status = dcc_component_append_select_json(builder, buffer, &first);
        break;
    }

    if (status != DCC_OK) {
        return status;
    }

    return dcc_component_json_append_cstr(buffer, "}");
}
