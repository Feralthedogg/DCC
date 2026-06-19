#ifndef DCC_REST_MULTIPART_BUILD_INTERNAL_H
#define DCC_REST_MULTIPART_BUILD_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/rest.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_multipart_validate(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count
);
dcc_status_t dcc_rest_multipart_append_field(dcc_rest_buffer_t *buffer, const dcc_rest_multipart_field_t *field);
dcc_status_t dcc_rest_multipart_append_file(dcc_rest_buffer_t *buffer, const dcc_rest_multipart_file_t *file);

#ifdef __cplusplus
}
#endif

#endif
