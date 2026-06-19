#ifndef DCC_REST_INTERCEPT_INTERNAL_H
#define DCC_REST_INTERCEPT_INTERNAL_H

#include <dcc/rest.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef dcc_status_t (*dcc_rest_intercept_fn)(
    dcc_client_t *client,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type,
    dcc_rest_cb cb,
    void *user_data,
    void *intercept_user_data
);

void dcc_rest_set_interceptor(
    dcc_client_t *client,
    dcc_rest_intercept_fn intercept,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
