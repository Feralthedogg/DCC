#ifndef DCC_REST_THREADS_CREATE_H
#define DCC_REST_THREADS_CREATE_H

#include <dcc/rest/types/threads.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_thread(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_create_forum_thread(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_create_thread_from_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_thread_params_t *params,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
