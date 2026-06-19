#ifndef DCC_REST_WEBHOOKS_MANAGEMENT_MODIFY_H
#define DCC_REST_WEBHOOKS_MANAGEMENT_MODIFY_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_modify_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_webhook_params(
    dcc_client_t *client,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_webhook_with_token(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_modify_webhook_with_token_params(
    dcc_client_t *client,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
