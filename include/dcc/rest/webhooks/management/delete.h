#ifndef DCC_REST_WEBHOOKS_MANAGEMENT_DELETE_H
#define DCC_REST_WEBHOOKS_MANAGEMENT_DELETE_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_delete_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
