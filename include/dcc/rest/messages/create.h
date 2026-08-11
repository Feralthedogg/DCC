#ifndef DCC_REST_MESSAGES_CREATE_H
#define DCC_REST_MESSAGES_CREATE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `POST /channels/{channel_id}/messages`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_create_message(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
