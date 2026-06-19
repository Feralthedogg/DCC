#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_channel_invites(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/invites", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_create_channel_invite(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/channels/%llu/invites", (unsigned long long)channel_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}
static dcc_status_t dcc_rest_build_invite_body(
    const dcc_invite_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->channel_id == 0 ||
        params->target_type > DCC_INVITE_TARGET_EMBEDDED_APPLICATION) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    uint32_t max_age = params->max_age != 0 ? params->max_age : 86400U;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "max_age", max_age);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(&body, &first, "max_uses", params->max_uses);
    }
    if (status == DCC_OK && params->target_user_id != 0) {
        status = dcc_rest_json_append_u64_string_member(&body, &first, "target_user", params->target_user_id);
    }
    if (status == DCC_OK && params->target_type != DCC_INVITE_TARGET_NONE) {
        status = dcc_rest_json_append_u64_member(&body, &first, "target_type", params->target_type);
    }
    if (status == DCC_OK && params->temporary) {
        status = dcc_rest_json_append_bool_member(&body, &first, "temporary", params->temporary);
    }
    if (status == DCC_OK && params->unique) {
        status = dcc_rest_json_append_bool_member(&body, &first, "unique", params->unique);
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
dcc_status_t dcc_rest_create_channel_invite_params(
    dcc_client_t *client,
    const dcc_invite_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_invite_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_channel_invite(client, params->channel_id, body, cb, user_data);
    }
    free(body);
    return status;
}
