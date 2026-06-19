#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_append_button_json(
    const dcc_component_builder_t *builder,
    dcc_component_json_buffer_t *buffer,
    int *first
) {
    dcc_status_t status = DCC_OK;
    if (builder->has_style) {
        status = dcc_component_json_append_u32_member(buffer, first, "style", (uint32_t)builder->style);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->label != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "label", builder->label);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_emoji) {
        status = dcc_component_json_member_prefix(buffer, first, "emoji");
        if (status == DCC_OK) {
            status = dcc_component_append_emoji_json(&builder->emoji, buffer);
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->custom_id != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "custom_id", builder->custom_id);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->url != NULL) {
        status = dcc_component_json_append_string_member(buffer, first, "url", builder->url);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_sku_id) {
        status = dcc_component_json_append_snowflake_string_member(buffer, first, "sku_id", builder->sku_id);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (builder->has_disabled) {
        status = dcc_component_json_append_bool_member(buffer, first, "disabled", builder->disabled);
        if (status != DCC_OK) {
            return status;
        }
    }

    return DCC_OK;
}
