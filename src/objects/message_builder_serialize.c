#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"

dcc_status_t dcc_message_builder_build_json(const dcc_message_builder_t *builder, char **out_json) {
    if (builder == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;
    dcc_status_t status = dcc_message_builder_validate_for_json(builder);
    if (status != DCC_OK) {
        return status;
    }

    dcc_message_json_buffer_t buffer = {0};
    status = dcc_message_json_append_cstr(&buffer, "{");
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&buffer);
        return status;
    }

    int first = 1;
    status = dcc_message_builder_append_basic_json(&buffer, &first, builder);
    if (status == DCC_OK) {
        status = dcc_message_builder_append_rich_json(&buffer, &first, builder);
    }
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&buffer);
        return status;
    }

    status = dcc_message_json_append_cstr(&buffer, "}");
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}
