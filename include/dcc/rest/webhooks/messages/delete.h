#ifndef DCC_REST_WEBHOOKS_MESSAGES_DELETE_H
#define DCC_REST_WEBHOOKS_MESSAGES_DELETE_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_delete_webhook_message(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_webhook_message_thread(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_snowflake_t message_id,
    dcc_snowflake_t thread_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
