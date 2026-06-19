#include "internal/objects/dcc_component_builder_internal.h"

dcc_status_t dcc_component_append_emoji_json(
    const dcc_component_emoji_t *emoji,
    dcc_component_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_component_validate_emoji(emoji);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_component_json_append_cstr(buffer, "{");
    if (status != DCC_OK) {
        return status;
    }

    int first = 1;
    if (emoji->has_id) {
        status = dcc_component_json_append_snowflake_string_member(buffer, &first, "id", emoji->id);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (emoji->name != NULL) {
        status = dcc_component_json_append_string_member(buffer, &first, "name", emoji->name);
        if (status != DCC_OK) {
            return status;
        }
    }
    if (emoji->has_animated) {
        status = dcc_component_json_append_bool_member(buffer, &first, "animated", emoji->animated);
        if (status != DCC_OK) {
            return status;
        }
    }

    return dcc_component_json_append_cstr(buffer, "}");
}
