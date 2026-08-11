#ifndef DCC_REST_THREADS_ARCHIVES_H
#define DCC_REST_THREADS_ARCHIVES_H

#include <dcc/rest/types/threads.h>
#include <dcc/rest/request.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_active_threads(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_public_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

DCC_API dcc_status_t dcc_rest_get_joined_private_archived_threads(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_joined_thread_archive_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
