#ifndef DCC_REST_THREADS_MEMBERS_H
#define DCC_REST_THREADS_MEMBERS_H

#include <dcc/rest/types/threads.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_join_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_leave_thread(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_add_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_remove_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_thread_member(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    dcc_snowflake_t user_id,
    const dcc_rest_thread_member_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_thread_members(
    dcc_client_t *client,
    dcc_snowflake_t thread_id,
    const dcc_rest_thread_member_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
