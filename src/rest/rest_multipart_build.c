#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_multipart_internal.h"

dcc_status_t dcc_rest_build_multipart_body(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    char **out_body,
    size_t *out_body_len
) {
    if (out_body == NULL || out_body_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body = NULL;
    *out_body_len = 0;

    dcc_status_t status = dcc_rest_multipart_validate(fields, field_count, files, file_count);
    if (status != DCC_OK) {
        return status;
    }

    dcc_rest_buffer_t buffer = {0};
    for (size_t i = 0; i < field_count; ++i) {
        status = dcc_rest_multipart_append_field(&buffer, &fields[i]);
        if (status != DCC_OK) {
            dcc_rest_buffer_deinit(&buffer);
            return status;
        }
    }
    for (size_t i = 0; i < file_count; ++i) {
        status = dcc_rest_multipart_append_file(&buffer, &files[i]);
        if (status != DCC_OK) {
            dcc_rest_buffer_deinit(&buffer);
            return status;
        }
    }
    status = dcc_rest_buffer_append_cstr(&buffer, "--" DCC_REST_MULTIPART_BOUNDARY "--\r\n");
    if (status != DCC_OK) {
        dcc_rest_buffer_deinit(&buffer);
        return status;
    }

    *out_body = buffer.data;
    *out_body_len = buffer.len;
    return DCC_OK;
}
