#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_current_user_dms(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/users/@me/channels", NULL, cb, user_data);
}

dcc_status_t dcc_rest_create_dm_channel(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_POST, "/users/@me/channels", json_body, cb, user_data);
}

static dcc_status_t dcc_rest_build_dm_channel_body(
    const dcc_dm_channel_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL || params->size < sizeof(*params) || params->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "recipient_id", params->user_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}

dcc_status_t dcc_rest_create_dm_channel_params(
    dcc_client_t *client,
    const dcc_dm_channel_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_dm_channel_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_dm_channel(client, body, cb, user_data);
    }
    free(body);
    return status;
}
