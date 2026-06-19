#include "internal/rest/dcc_rest_builders_internal.h"

#include <stdlib.h>

dcc_status_t dcc_rest_edit_guild_command_permissions_params(
    dcc_client_t *client,
    const dcc_guild_command_permissions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_guild_command_permissions_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_edit_guild_command_permissions(
            client,
            params->application_id,
            params->guild_id,
            params->command_id,
            body,
            cb,
            user_data
        );
    }
    free(body);
    return status;
}

dcc_status_t dcc_rest_bulk_edit_guild_command_permissions_params(
    dcc_client_t *client,
    const dcc_bulk_guild_command_permissions_params_t *params,
    dcc_rest_cb cb,
    void *user_data
) {
    char *body = NULL;
    dcc_status_t status = dcc_rest_build_bulk_guild_command_permissions_body(params, &body);
    if (status == DCC_OK) {
        status = dcc_rest_bulk_edit_guild_command_permissions(
            client,
            params->application_id,
            params->guild_id,
            body,
            cb,
            user_data
        );
    }
    free(body);
    return status;
}
