#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_rest_build_stage_instance_body(
    const dcc_stage_instance_params_t *params,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->channel_id == 0 ||
        params->topic == NULL ||
        params->topic[0] == '\0' ||
        strlen(params->topic) > 120U ||
        (params->privacy_level != DCC_STAGE_PRIVACY_PUBLIC &&
         params->privacy_level != DCC_STAGE_PRIVACY_GUILD_ONLY)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_rest_buffer_t body = {0};
    dcc_status_t status = dcc_rest_buffer_append_cstr(&body, "{\"channel_id\":\"");
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(&body, params->channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "\",\"topic\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_json_string(&body, params->topic);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, ",\"privacy_level\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_u64_text(&body, (uint64_t)params->privacy_level);
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

dcc_status_t dcc_rest_create_stage_instance_params(
    dcc_client_t *client,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_stage_instance_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_create_stage_instance(client, body, cb, user_data);
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_modify_stage_instance_params(
    dcc_client_t *client,
    const dcc_stage_instance_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_stage_instance_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_stage_instance(client, params->channel_id, body, cb, user_data);
    }
    free(body);
    return status;
}
