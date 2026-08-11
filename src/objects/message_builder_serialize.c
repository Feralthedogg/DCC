#include "internal/objects/dcc_message_builder_serialize_internal.h"

dcc_status_t dcc_message_builder_append_members_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
) {
    if (buffer == NULL || first == NULL || builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_message_builder_validate_for_json(builder);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_message_builder_append_basic_json(buffer, first, builder);
    if (status == DCC_OK) {
        status = dcc_message_builder_append_rich_json(buffer, first, builder);
    }
    return status;
}

dcc_status_t dcc_message_builder_append_json(
    dcc_message_json_buffer_t *buffer,
    const dcc_message_builder_t *builder
) {
    if (buffer == NULL || builder == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_message_builder_validate_for_json(builder);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_message_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_message_builder_append_basic_json(buffer, &first, builder);
    }
    if (status == DCC_OK) {
        status = dcc_message_builder_append_rich_json(buffer, &first, builder);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(buffer, "}");
    }
    return status;
}

dcc_status_t dcc_message_builder_build_json(const dcc_message_builder_t *builder, char **out_json) {
    if (builder == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_message_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_message_builder_append_json(&buffer, builder);
    if (status != DCC_OK) {
        dcc_message_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

dcc_status_t dcc_message_builder_measure_json(
    const dcc_message_builder_t *builder,
    size_t *out_json_len
) {
    if (builder == NULL || out_json_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json_len = 0U;

    dcc_message_json_buffer_t buffer;
    dcc_message_json_buffer_init_count(&buffer);
    dcc_status_t status = dcc_message_builder_append_json(&buffer, builder);
    if (status == DCC_OK) {
        *out_json_len = buffer.len;
    }
    dcc_message_json_buffer_deinit(&buffer);
    return status;
}
