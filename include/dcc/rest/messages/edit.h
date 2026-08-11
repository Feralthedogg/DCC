#ifndef DCC_REST_MESSAGES_EDIT_H
#define DCC_REST_MESSAGES_EDIT_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `PATCH /channels/{channel_id}/messages/{message_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_edit_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE /channels/{channel_id}/messages/{message_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `POST /channels/{channel_id}/messages/{message_id}/crosspost`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_crosspost_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `POST /channels/{channel_id}/messages/bulk-delete`.
 * @details `message_ids` is borrowed for the call and must contain 2..100
 * nonzero IDs. Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_bulk_delete_messages(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_snowflake_t *message_ids,
    size_t message_count,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
