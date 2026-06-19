#include "internal/rest/dcc_rest_multipart_build_internal.h"

static int dcc_rest_multipart_quoted_token_valid(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (*p == '\r' || *p == '\n' || *p == '"') {
            return 0;
        }
    }
    return 1;
}

static int dcc_rest_multipart_header_value_valid(const char *value) {
    if (value == NULL || value[0] == '\0') {
        return 0;
    }
    for (const unsigned char *p = (const unsigned char *)value; *p != '\0'; ++p) {
        if (*p == '\r' || *p == '\n') {
            return 0;
        }
    }
    return 1;
}

dcc_status_t dcc_rest_multipart_validate(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
) {
    if ((field_count != 0 && fields == NULL) ||
        (file_count != 0 && files == NULL) ||
        (field_count == 0 && file_count == 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    for (size_t i = 0; i < field_count; ++i) {
        if (!dcc_rest_multipart_quoted_token_valid(fields[i].name) || fields[i].value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    for (size_t i = 0; i < file_count; ++i) {
        const dcc_rest_multipart_file_t *file = &files[i];
        if (!dcc_rest_multipart_quoted_token_valid(file->field_name) ||
            !dcc_rest_multipart_quoted_token_valid(file->filename) ||
            (file->content_type != NULL &&
             file->content_type[0] != '\0' &&
             !dcc_rest_multipart_header_value_valid(file->content_type)) ||
            (file->data_len != 0 && file->data == NULL)) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    return DCC_OK;
}
