#ifndef DCC_REST_THREADS_CREATE_H
#define DCC_REST_THREADS_CREATE_H

#include <dcc/rest/types/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_thread(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_forum_thread_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_thread_from_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_thread_from_message_params(
    dcc_client_t *client,
    const dcc_thread_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
