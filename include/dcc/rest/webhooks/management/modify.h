#ifndef DCC_REST_WEBHOOKS_MANAGEMENT_MODIFY_H
#define DCC_REST_WEBHOOKS_MANAGEMENT_MODIFY_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Submits `PATCH /webhooks/{webhook_id}[/{webhook_token}]`.
 * @details The optional token and `builder` are borrowed for the call. Uses
 * the @ref dcc_rest_endpoint_call_contract.
 */
DCC_API dcc_status_t dcc_rest_modify_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const dcc_rest_webhook_builder_t *builder,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
