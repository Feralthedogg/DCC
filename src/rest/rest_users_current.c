#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_current_application(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/oauth2/applications/@me", NULL, cb, user_data);
}

dcc_status_t dcc_rest_get_current_user(
    dcc_client_t *client,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_GET, "/users/@me", NULL, cb, user_data);
}

dcc_status_t dcc_rest_modify_current_user(
    dcc_client_t *client,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_request_method(client, DCC_REST_PATCH, "/users/@me", json_body, cb, user_data);
}

static dcc_status_t dcc_rest_build_current_user_body(
    const dcc_current_user_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL || params->size < sizeof(*params)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_nullable_string_member(&body, &first, "nickname", params->nickname);
    }
    if (status == DCC_OK && params->avatar != NULL && params->avatar[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "avatar", params->avatar);
    }
    if (status == DCC_OK && params->banner != NULL && params->banner[0] != '\0') {
        status = dcc_rest_json_append_string_member(&body, &first, "banner", params->banner);
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

dcc_status_t dcc_rest_modify_current_user_params(
    dcc_client_t *client,
    const dcc_current_user_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_current_user_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_current_user(client, body, cb, user_data);
    }
    free(body);
    return status;
}
