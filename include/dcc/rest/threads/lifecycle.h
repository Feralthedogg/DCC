#ifndef DCC_REST_THREADS_LIFECYCLE_H
#define DCC_REST_THREADS_LIFECYCLE_H

#include <dcc/rest/types/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
