#ifndef DCC_REST_CHANNEL_PERMISSION_BUILDERS_INTERNAL_H
#define DCC_REST_CHANNEL_PERMISSION_BUILDERS_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_channel_permission_overwrite_body(
    const dcc_channel_permission_overwrite_t *overwrite,
    uint8_t include_id,
    dcc_rest_buffer_t *body
);
dcc_status_t dcc_rest_build_channel_permission_overwrites_array(
    const dcc_channel_permission_overwrite_t *overwrites,
    size_t overwrite_count,
    dcc_rest_buffer_t *body
);

#ifdef __cplusplus
}
#endif

#endif
