#include "internal/objects/dcc_embed_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

#include <stdlib.h>

dcc_status_t dcc_embed_builder_build_json(const dcc_embed_builder_t *builder, char **out_json) {
    if (builder == NULL || out_json == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    dcc_embed_json_buffer_t buffer = {0};
    dcc_status_t status = dcc_embed_builder_append_json(builder, &buffer);
    if (status != DCC_OK) {
        dcc_embed_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

dcc_status_t dcc_embed_builder_build_array_json(
    const dcc_embed_builder_t *builders,
    size_t builder_count,
    char **out_json
) {
    if (out_json == NULL || (builder_count != 0 && builders == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_json = NULL;

    size_t stride = 0U;
    dcc_status_t status = dcc_embed_builder_array_begin(
        builders, builder_count, &stride
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_embed_json_buffer_t buffer = {0};
    status = dcc_embed_json_append_cstr(&buffer, "[");
    for (size_t i = 0; status == DCC_OK && i < builder_count; ++i) {
        if (i != 0) {
            status = dcc_embed_json_append_cstr(&buffer, ",");
        }
        if (status == DCC_OK) {
            const dcc_embed_builder_t *builder = (const dcc_embed_builder_t *)
                dcc_builder_abi_array_at(builders, stride, i);
            dcc_builder_abi_view_t view;
            status = dcc_embed_builder_abi_validate(builder, &view);
            if (status == DCC_OK && view.size != stride) {
                status = DCC_ERR_INVALID_ARG;
            }
            if (status == DCC_OK) {
                status = dcc_embed_builder_append_json(builder, &buffer);
            }
        }
    }
    if (status == DCC_OK) {
        status = dcc_embed_json_append_cstr(&buffer, "]");
    }
    if (status != DCC_OK) {
        dcc_embed_json_buffer_deinit(&buffer);
        return status;
    }

    *out_json = buffer.data;
    return DCC_OK;
}

void dcc_embed_builder_json_free(char *json) {
    free(json);
}
