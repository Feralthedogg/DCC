#ifndef DCC_REST_INTERACTIONS_ORIGINAL_RESPONSE_H
#define DCC_REST_INTERACTIONS_ORIGINAL_RESPONSE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `GET /webhooks/{application_id}/{token}/messages/@original`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_original_response_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `PATCH /webhooks/{application_id}/{token}/messages/@original`.
 * @details `payload` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_original_response_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE /webhooks/{application_id}/{token}/messages/@original`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_original_response_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
