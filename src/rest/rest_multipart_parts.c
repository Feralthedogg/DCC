#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"
#include "internal/rest/dcc_rest_multipart_build_internal.h"

static dcc_status_t dcc_rest_multipart_append_boundary(dcc_rest_buffer_t *buffer) {
    return dcc_rest_buffer_append_cstr(buffer, "--" DCC_REST_MULTIPART_BOUNDARY "\r\n");
}

dcc_status_t dcc_rest_multipart_append_field(
    dcc_rest_buffer_t *buffer,
    const dcc_rest_multipart_field_t *field
) {
    dcc_status_t status = dcc_rest_multipart_append_boundary(buffer);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "Content-Disposition: form-data; name=\"");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, field->name);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "\"\r\n\r\n");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, field->value);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_buffer_append_cstr(buffer, "\r\n");
}

dcc_status_t dcc_rest_multipart_append_file(
    dcc_rest_buffer_t *buffer,
    const dcc_rest_multipart_file_t *file
) {
    const char *content_type =
        file->content_type != NULL && file->content_type[0] != '\0'
            ? file->content_type
            : "application/octet-stream";

    dcc_status_t status = dcc_rest_multipart_append_boundary(buffer);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "Content-Disposition: form-data; name=\"");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, file->field_name);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "\"; filename=\"");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, file->filename);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "\"\r\nContent-Type: ");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, content_type);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append_cstr(buffer, "\r\n\r\n");
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_buffer_append(buffer, file->data, file->data_len);
    if (status != DCC_OK) {
        return status;
    }
    return dcc_rest_buffer_append_cstr(buffer, "\r\n");
}
