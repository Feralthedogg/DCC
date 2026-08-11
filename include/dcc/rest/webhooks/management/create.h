#ifndef DCC_REST_WEBHOOKS_MANAGEMENT_CREATE_H
#define DCC_REST_WEBHOOKS_MANAGEMENT_CREATE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `POST /channels/{channel_id}/webhooks`.
 * @details `builder` is borrowed for the call. Uses the
 * @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_create_webhook(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const dcc_rest_webhook_builder_t *builder,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
