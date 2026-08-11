#ifndef DCC_REST_MESSAGES_POLLS_H
#define DCC_REST_MESSAGES_POLLS_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `GET /channels/{channel_id}/polls/{message_id}/answers/{answer_id}`.
 * @details `page` may be NULL and accepts only `after` and limit 1..100.
 * Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_get_poll_answer_voters(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    uint32_t answer_id,
    const dcc_rest_id_page_t *page,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `POST /channels/{channel_id}/polls/{message_id}/expire`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_end_poll(
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
