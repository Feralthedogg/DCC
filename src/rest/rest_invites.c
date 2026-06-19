#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_guild_invites(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/invites", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_GET, path, NULL, cb, user_data) : status;
}

dcc_status_t dcc_rest_get_invite(
    dcc_client_t *client,
    const char *invite_code,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char *code = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(invite_code, &code);
    if (status != DCC_OK) {
        return status;
    }

    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, "/invites/%s", code);
    free(code);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data);
    free(path);
    return status;
}

dcc_status_t dcc_rest_get_invite_full(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_get_invite(client, invite_code, "with_counts=true&with_expiration=true", cb, user_data);
}

dcc_status_t dcc_rest_delete_invite(
    dcc_client_t *client,
    const char *invite_code,
    dcc_rest_cb cb,
    void *user_data
) {
    char *code = NULL;
    dcc_status_t status = dcc_rest_escape_path_segment(invite_code, &code);
    if (status != DCC_OK) {
        return status;
    }

    char *path = NULL;
    status = dcc_rest_alloc_formatted_path(&path, "/invites/%s", code);
    free(code);
    return status == DCC_OK ? dcc_rest_request_owned_path(client, DCC_REST_DELETE, path, NULL, cb, user_data) : status;
}
