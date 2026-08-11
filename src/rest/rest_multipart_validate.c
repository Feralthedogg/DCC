#include "internal/rest/dcc_rest_multipart_build_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"

#include <stdint.h>
#include <string.h>

static int dcc_rest_multipart_array_span_valid(
    const void *values,
    size_t count,
    size_t item_size
) {
    if (count == 0U) {
        return 1;
    }
    if (values == NULL || item_size == 0U || count > SIZE_MAX / item_size) {
        return 0;
    }
    size_t span = count * item_size;
    uintptr_t address = (uintptr_t)values;
    return address <= UINTPTR_MAX - (span - 1U);
}

static int dcc_rest_multipart_size_add(size_t *total, size_t amount) {
    if (total == NULL || *total > SIZE_MAX - 1U ||
        amount > SIZE_MAX - 1U - *total) {
        return 0;
    }
    *total += amount;
    return 1;
}

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

dcc_status_t dcc_rest_multipart_measure(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    size_t *out_body_len
) {
    if (out_body_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body_len = 0U;
    if (!dcc_rest_multipart_array_span_valid(
            fields,
            field_count,
            sizeof(*fields)
        ) ||
        !dcc_rest_multipart_array_span_valid(
            files,
            file_count,
            sizeof(*files)
        ) ||
        (field_count == 0 && file_count == 0)) {
        return DCC_ERR_INVALID_ARG;
    }

    size_t body_len = 0U;
    for (size_t i = 0; i < field_count; ++i) {
        if (!dcc_rest_multipart_quoted_token_valid(fields[i].name) || fields[i].value == NULL) {
            return DCC_ERR_INVALID_ARG;
        }
        if (!dcc_rest_multipart_size_add(
                &body_len,
                sizeof("--" DCC_REST_MULTIPART_BOUNDARY "\r\n") - 1U
            ) ||
            !dcc_rest_multipart_size_add(
                &body_len,
                sizeof("Content-Disposition: form-data; name=\"") - 1U
            ) ||
            !dcc_rest_multipart_size_add(&body_len, strlen(fields[i].name)) ||
            !dcc_rest_multipart_size_add(&body_len, sizeof("\"\r\n\r\n") - 1U) ||
            !dcc_rest_multipart_size_add(&body_len, strlen(fields[i].value)) ||
            !dcc_rest_multipart_size_add(&body_len, sizeof("\r\n") - 1U)) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    for (size_t i = 0; i < file_count; ++i) {
        const dcc_rest_multipart_file_t *file = &files[i];
        const char *content_type =
            file->content_type != NULL && file->content_type[0] != '\0'
                ? file->content_type
                : "application/octet-stream";
        if (!dcc_rest_multipart_quoted_token_valid(file->field_name) ||
            !dcc_rest_multipart_quoted_token_valid(file->filename) ||
            (file->content_type != NULL &&
             file->content_type[0] != '\0' &&
             !dcc_rest_multipart_header_value_valid(file->content_type)) ||
            (file->data_len != 0 && file->data == NULL)) {
            return DCC_ERR_INVALID_ARG;
        }
        if (!dcc_rest_multipart_size_add(
                &body_len,
                sizeof("--" DCC_REST_MULTIPART_BOUNDARY "\r\n") - 1U
            ) ||
            !dcc_rest_multipart_size_add(
                &body_len,
                sizeof("Content-Disposition: form-data; name=\"") - 1U
            ) ||
            !dcc_rest_multipart_size_add(&body_len, strlen(file->field_name)) ||
            !dcc_rest_multipart_size_add(&body_len, sizeof("\"; filename=\"") - 1U) ||
            !dcc_rest_multipart_size_add(&body_len, strlen(file->filename)) ||
            !dcc_rest_multipart_size_add(
                &body_len,
                sizeof("\"\r\nContent-Type: ") - 1U
            ) ||
            !dcc_rest_multipart_size_add(&body_len, strlen(content_type)) ||
            !dcc_rest_multipart_size_add(&body_len, sizeof("\r\n\r\n") - 1U) ||
            !dcc_rest_multipart_size_add(&body_len, file->data_len) ||
            !dcc_rest_multipart_size_add(&body_len, sizeof("\r\n") - 1U)) {
            return DCC_ERR_INVALID_ARG;
        }
    }

    if (!dcc_rest_multipart_size_add(
            &body_len,
            sizeof("--" DCC_REST_MULTIPART_BOUNDARY "--\r\n") - 1U
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_body_len = body_len;
    return DCC_OK;
}

dcc_status_t dcc_rest_multipart_validate(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
) {
    size_t body_len = 0U;
    return dcc_rest_multipart_measure(
        fields,
        field_count,
        files,
        file_count,
        &body_len
    );
}
