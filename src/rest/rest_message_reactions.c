#include "internal/rest/dcc_rest_message_reactions_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

dcc_status_t dcc_rest_add_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_message_reaction_path(&path, channel_id, message_id, reaction, "/@me");
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_add_message_reaction_params(
    dcc_client_t *client,
    const dcc_message_reaction_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->channel_id == 0 ||
        params->message_id == 0 ||
        params->reaction == NULL ||
        params->reaction[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_add_message_reaction(
        client,
        params->channel_id,
        params->message_id,
        params->reaction,
        cb,
        user_data
    );
}

dcc_status_t dcc_rest_delete_own_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_message_reaction_path(&path, channel_id, message_id, reaction, "/@me");
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_user_message_reaction(
    dcc_client_t *client,
    dcc_snowflake_t channel_id,
    dcc_snowflake_t message_id,
    const char *reaction,
    dcc_snowflake_t user_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char suffix[32];
    dcc_status_t status = dcc_rest_format_path(suffix, sizeof(suffix), "/%llu", (unsigned long long)user_id);
    if (status != DCC_OK) {
        return status;
    }
    char *path = NULL;
    status = dcc_rest_alloc_message_reaction_path(&path, channel_id, message_id, reaction, suffix);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
