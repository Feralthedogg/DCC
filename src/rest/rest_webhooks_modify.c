#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_webhook_body_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_modify_webhook(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[64];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/webhooks/%llu", (unsigned long long)webhook_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_webhook_params(
    dcc_client_t *client,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->webhook_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_webhook_body(params, 1, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_webhook(client, params->webhook_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_modify_webhook_with_token(
    dcc_client_t *client,
    dcc_snowflake_t webhook_id,
    const char *webhook_token,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_webhook_token_path(&path, webhook_id, webhook_token, NULL, 0);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_webhook_with_token_params(
    dcc_client_t *client,
    const dcc_webhook_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->webhook_id == 0 ||
        params->token == NULL ||
        params->token[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_webhook_body(params, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_webhook_with_token(client, params->webhook_id, params->token, body, cb, user_data);
    }
    free(body);
    return status;
}
