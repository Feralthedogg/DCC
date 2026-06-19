#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_voice_state_body_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_user_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/voice-states/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_user_voice_state(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t user_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[112];
    dcc_status_t status = dcc_rest_format_path(
        path,
        sizeof(path),
        "/guilds/%llu/voice-states/%llu",
        (unsigned long long)guild_id,
        (unsigned long long)user_id
    );
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PATCH, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_modify_user_voice_state_params(
    dcc_client_t *client,
    const dcc_voice_state_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL || params->size < sizeof(*params) || params->user_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    char *body = NULL;
    dcc_status_t status = dcc_rest_build_voice_state_body(params, 0, &body);
    if (status == DCC_OK) {
        status = dcc_rest_modify_user_voice_state(client, params->guild_id, params->user_id, body, cb, user_data);
    }
    free(body);
    return status;
}
