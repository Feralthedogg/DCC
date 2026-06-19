#include "internal/command_registry/dcc_command_registry_internal.h"

dcc_status_t dcc_command_registry_apply(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_command_registry_options_t *options,
    const dcc_command_registry_plan_t *plan,
    dcc_rest_cb cb,
    void *user_data
) {
    if (client == NULL || application_id == 0 || plan == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (dcc_command_registry_options_dry_run(options) || plan->dry_run) {
        return DCC_OK;
    }

    dcc_snowflake_t guild_id = dcc_command_registry_plan_guild_id(options, plan);
    dcc_status_t status = DCC_OK;
    for (size_t i = 0; i < plan->diff_count; ++i) {
        const dcc_command_registry_diff_t *diff = &plan->diffs[i];
        switch (diff->action) {
            case DCC_COMMAND_REGISTRY_CREATE:
                if (diff->local_json == NULL) {
                    return DCC_ERR_INVALID_ARG;
                }
                status = dcc_command_registry_apply_create(
                    client,
                    application_id,
                    guild_id,
                    diff->local_json,
                    cb,
                    user_data
                );
                break;
            case DCC_COMMAND_REGISTRY_UPDATE:
                if (diff->local_json == NULL) {
                    return DCC_ERR_INVALID_ARG;
                }
                status = dcc_command_registry_apply_update(
                    client,
                    application_id,
                    guild_id,
                    diff->remote_id,
                    diff->local_json,
                    cb,
                    user_data
                );
                break;
            case DCC_COMMAND_REGISTRY_DELETE_STALE:
                status = dcc_command_registry_apply_delete(
                    client,
                    application_id,
                    guild_id,
                    diff->remote_id,
                    cb,
                    user_data
                );
                break;
            case DCC_COMMAND_REGISTRY_NOOP:
            default:
                status = DCC_OK;
                break;
        }
        if (status != DCC_OK) {
            return status;
        }
    }
    return DCC_OK;
}
