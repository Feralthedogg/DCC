#ifndef DCC_REST_MULTIPART_INTERNAL_H
#define DCC_REST_MULTIPART_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_multipart_body(
    const dcc_rest_multipart_field_t *fields,
    size_t field_count,
    const dcc_rest_multipart_file_t *files,
    size_t file_count,
    char **out_body,
    size_t *out_body_len
);

#ifdef __cplusplus
}
#endif

#endif
