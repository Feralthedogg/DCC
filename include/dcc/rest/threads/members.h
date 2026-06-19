#ifndef DCC_REST_THREADS_MEMBERS_H
#define DCC_REST_THREADS_MEMBERS_H

#include <dcc/rest/types/threads.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_join_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_leave_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_add_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_add_thread_member_params(
    dcc_client_t *client,
    const dcc_thread_member_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_remove_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_thread_members(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
