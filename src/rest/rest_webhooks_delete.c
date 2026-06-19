#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_delete_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/webhooks/%llu", (unsigned long long)webhook_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_webhook_with_token(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(&path, webhook_id, webhook_token, NULL, 0);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
