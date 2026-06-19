#ifndef DCC_REST_ROLE_CONNECTION_BUILDERS_INTERNAL_H
#define DCC_REST_ROLE_CONNECTION_BUILDERS_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int dcc_rest_application_role_connection_key_valid(const char *key);

dcc_status_t dcc_rest_build_application_role_connection_metadata_body(
    const dcc_application_role_connection_metadata_params_t *metadata,
    size_t metadata_count,
    char **out
);
dcc_status_t dcc_rest_build_application_role_connection_body(
    const dcc_application_role_connection_params_t *params,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
