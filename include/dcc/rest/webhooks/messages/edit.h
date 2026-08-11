#ifndef DCC_REST_WEBHOOKS_MESSAGES_EDIT_H
#define DCC_REST_WEBHOOKS_MESSAGES_EDIT_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_webhook_message(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const dcc_rest_webhook_message_edit_t *edit,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
