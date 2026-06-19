#include "internal/rest/dcc_rest_request_internal.h"
#include "internal/rest/dcc_rest_template_paths_internal.h"

dcc_status_t dcc_rest_sync_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_guild_template_path(&path, guild_id, code);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_PUT, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_delete_guild_template(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *code,
    dcc_rest_cb cb,
    void *user_data
) {
    char *path = NULL;
    dcc_status_t status = dcc_rest_alloc_guild_template_path(&path, guild_id, code);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
