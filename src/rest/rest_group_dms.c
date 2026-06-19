#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_add_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/recipients/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

static dcc_status_t dcc_rest_build_group_dm_recipient_body(
    const dcc_group_dm_recipient_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->channel_id == 0 ||
        params->user_id == 0 ||
        params->access_token == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "access_token", params->access_token);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(
            &body,
            &first,
            "nick",
            params->nick != NULL ? params->nick : ""
        );
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

dcc_status_t dcc_rest_add_group_dm_recipient_params(
    dcc_client_t *client,
    const dcc_group_dm_recipient_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_group_dm_recipient_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_add_group_dm_recipient(client, params->channel_id, params->user_id, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_remove_group_dm_recipient(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/channels/%llu/recipients/%llu",
        (unsigned long long)channel_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
