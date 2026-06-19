#ifndef DCC_REST_APPLICATION_COMMAND_PERMISSION_JSON_INTERNAL_H
#define DCC_REST_APPLICATION_COMMAND_PERMISSION_JSON_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_append_command_permission_object(
    dcc_rest_buffer_t *body,
    const dcc_application_command_permission_t *permission
);
dcc_status_t dcc_rest_append_command_permissions_array(
    dcc_rest_buffer_t *body,
    const dcc_application_command_permission_t *permissions,
    size_t permission_count
);

#ifdef __cplusplus
}
#endif

#endif
