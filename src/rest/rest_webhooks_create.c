#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_webhook_body_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_create_webhook(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[88];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/webhooks", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_create_webhook_params(
    dcc_client_t *client,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_webhook_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_webhook(client, params->channel_id, body, cb, user_data);
    }
    free(body);
    return status;
}
