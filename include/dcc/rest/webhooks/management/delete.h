#ifndef DCC_REST_WEBHOOKS_MANAGEMENT_DELETE_H
#define DCC_REST_WEBHOOKS_MANAGEMENT_DELETE_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_delete_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_webhook_with_token(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
