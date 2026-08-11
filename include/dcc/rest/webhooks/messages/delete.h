#ifndef DCC_REST_WEBHOOKS_MESSAGES_DELETE_H
#define DCC_REST_WEBHOOKS_MESSAGES_DELETE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `DELETE /webhooks/{id}/{token}/messages/{message_id}`.
 * @details `query` may be NULL. Uses the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_delete_webhook_message(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const dcc_rest_webhook_message_query_t *query,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
