#include "internal/rest/dcc_rest_guild_prune_body_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_query_collections_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_get_guild_prune_count(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/prune", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_with_query(client, DCC_REST_GET, path, query, NULL, cb, user_data) : status;
}
dcc_status_t dcc_rest_get_guild_prune_count_options(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_rest_buffer_t query = {0};
    dcc_status_t status = dcc_rest_query_append_u64(&query, "days", days);
    if (status == DCC_OK) {
        status = dcc_rest_query_append_snowflake_csv(&query, "include_roles", include_roles, include_role_count);
    }
    if (status == DCC_OK) {
        status = dcc_rest_get_guild_prune_count(client, guild_id, query.data, cb, user_data);
    }
    dcc_rest_buffer_deinit(&query);
    return status;
}
dcc_status_t dcc_rest_begin_guild_prune(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/guilds/%llu/prune", (unsigned long long)guild_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_POST, path, json_body, cb, user_data) : status;
}
dcc_status_t dcc_rest_begin_guild_prune_options(
    dcc_client_t *client,
    dcc_snowflake_t guild_id,
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_prune_body(
        days,
        include_roles,
        include_role_count,
        compute_prune_count,
        &body
    );
    if (status == DCC_OK) {
        status = dcc_rest_begin_guild_prune(client, guild_id, body, cb, user_data);
    }
    free(body);
    return status;
}
dcc_status_t dcc_rest_begin_guild_prune_params(
    dcc_client_t *client,
    const dcc_guild_prune_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->guild_id == 0 ||
        (params->include_role_count != 0 && params->include_roles == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    return dcc_rest_begin_guild_prune_options(
        client,
        params->guild_id,
        params->days,
        params->include_roles,
        params->include_role_count,
        params->compute_prune_count,
        cb,
        user_data
    );
}
