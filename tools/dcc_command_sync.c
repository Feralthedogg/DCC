#include "internal/dcc_command_sync.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_command_sync_create_client(
    const dcc_command_sync_options_t *options,
    dcc_client_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    if (options->remote_path != NULL && !options->apply) {
        return DCC_OK;
    }
    dcc_client_options_t client_options = {
        .size = sizeof(client_options),
        .token = options->token,
        .intents = DCC_INTENT_GUILDS,
    };
    return dcc_client_create(&client_options, out);
}

static dcc_status_t dcc_command_sync_run(const dcc_command_sync_options_t *options) {
    char *commands_json = NULL;
    size_t commands_len = 0;
    dcc_command_sync_local_t local;
    dcc_command_registry_remote_snapshot_t remote;
    dcc_command_registry_plan_t plan;
    dcc_client_t *client = NULL;
    char *plan_text = NULL;
    dcc_status_t status;

    memset(&local, 0, sizeof(local));
    memset(&remote, 0, sizeof(remote));
    memset(&plan, 0, sizeof(plan));

    status = dcc_command_sync_load_file(options->commands_path, &commands_json, &commands_len);
    if (status == DCC_OK) {
        status = dcc_command_sync_parse_local_commands(commands_json, commands_len, &local);
    }
    free(commands_json);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_command_sync_create_client(options, &client);
    if (status == DCC_OK) {
        status = dcc_command_sync_read_remote_snapshot(options, client, &remote);
    }
    if (status == DCC_OK) {
        dcc_command_registry_options_t registry_options;
        dcc_command_registry_options_init(&registry_options);
        if (options->guild_id != 0) {
            status = dcc_command_registry_options_set_guild(&registry_options, options->guild_id);
        } else {
            status = dcc_command_registry_options_set_global(&registry_options);
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_options_set_delete_stale(
                &registry_options,
                (uint8_t)options->delete_stale
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_options_set_dry_run(
                &registry_options,
                (uint8_t)options->dry_run
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_registry_build_plan(
                &local.registry,
                &registry_options,
                remote.entries,
                remote.entry_count,
                &plan
            );
        }
        if (status == DCC_OK) {
            status = dcc_command_sync_format_plan(&plan, &plan_text);
        }
        if (status == DCC_OK && (options->plan || options->check_only)) {
            fputs(plan_text, stdout);
        }
        if (status == DCC_OK && options->check_only) {
            fprintf(
                stdout,
                "check=ok local_commands=%zu remote_commands=%zu scope=%s dry_run=%s\n",
                dcc_command_registry_count(&local.registry),
                remote.entry_count,
                options->guild_id != 0 ? "guild" : "global",
                options->dry_run ? "true" : "false"
            );
        }
        if (status == DCC_OK && options->apply && !options->check_only) {
            dcc_command_sync_remote_body_t apply_state = {0};
            status = dcc_command_registry_apply(
                client,
                options->application_id,
                &registry_options,
                &plan,
                dcc_command_sync_apply_cb,
                &apply_state
            );
            if (status == DCC_OK && apply_state.error != DCC_OK) {
                status = apply_state.error;
            }
        }
        free(plan_text);
        dcc_command_registry_plan_deinit(&plan);
    }

    dcc_command_registry_remote_snapshot_deinit(&remote);
    dcc_client_destroy(client);
    dcc_command_sync_local_deinit(&local);
    return status;
}

int main(int argc, char **argv) {
    dcc_command_sync_options_t options;
    int parse_status = dcc_command_sync_parse_options(argc, argv, &options);
    if (parse_status > 0) {
        return 0;
    }
    if (parse_status < 0 || dcc_command_sync_validate_options(&options) != 0) {
        dcc_command_sync_usage(stderr, argv[0]);
        return 2;
    }

    dcc_status_t status = dcc_command_sync_run(&options);
    if (status != DCC_OK) {
        fprintf(stderr, "command sync failed: %s\n", dcc_status_string(status));
        return 1;
    }
    return 0;
}
