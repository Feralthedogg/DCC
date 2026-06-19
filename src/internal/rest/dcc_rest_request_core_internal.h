#ifndef DCC_REST_REQUEST_CORE_INTERNAL_H
#define DCC_REST_REQUEST_CORE_INTERNAL_H

#include <dcc/rest.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_request_owned_path(
    dcc_client_t *client,
    dcc_rest_method_t method,
    char *path,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
);
dcc_status_t dcc_rest_request_with_query(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const char *query,
    const char *body,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
