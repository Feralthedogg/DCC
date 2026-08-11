#ifndef DCC_REST_MESSAGES_FETCH_H
#define DCC_REST_MESSAGES_FETCH_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `GET /channels/{channel_id}/messages`.
 * @details `query` may be NULL. Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_get_channel_messages(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_message_list_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `GET /channels/{channel_id}/messages/{message_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_get_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
