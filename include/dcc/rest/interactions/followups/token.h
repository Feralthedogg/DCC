#ifndef DCC_REST_INTERACTIONS_FOLLOWUPS_TOKEN_H
#define DCC_REST_INTERACTIONS_FOLLOWUPS_TOKEN_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `POST /webhooks/{application_id}/{token}`.
 * @details `payload` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_followup_create(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `GET /webhooks/{application_id}/{token}/messages/{message_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_followup_get(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `PATCH /webhooks/{application_id}/{token}/messages/{message_id}`.
 * @details `payload` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_followup_edit(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const dcc_rest_message_payload_t *payload,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

/**
 * @brief Submits `DELETE /webhooks/{application_id}/{token}/messages/{message_id}`.
 * @details Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_interaction_followup_delete(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *interaction_token,
    dcc_snowflake_t message_id,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
