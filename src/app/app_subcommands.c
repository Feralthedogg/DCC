#include "internal/app/dcc_app_internal.h"

#include <stdio.h>
#include <string.h>

dcc_status_t dcc_app_subcommand_key(
    const char *command_name,
    const char *subcommand_path,
    char *buffer,
    size_t buffer_len
) {
    if (command_name == NULL || command_name[0] == '\0' ||
        subcommand_path == NULL || subcommand_path[0] == '\0' ||
        buffer == NULL || buffer_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    int written = snprintf(buffer, buffer_len, "%s/%s", command_name, subcommand_path);
    if (written < 0 || (size_t)written >= buffer_len) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

const char *dcc_ctx_subcommand(const dcc_ctx_t *ctx) {
    const dcc_interaction_option_t *option =
        ctx != NULL ? dcc_interaction_subcommand(ctx->interaction) : NULL;
    return option != NULL ? option->name : NULL;
}

const char *dcc_ctx_subcommand_group(const dcc_ctx_t *ctx) {
    const dcc_interaction_option_t *option =
        ctx != NULL ? dcc_interaction_subcommand_group(ctx->interaction) : NULL;
    return option != NULL ? option->name : NULL;
}

dcc_status_t dcc_ctx_subcommand_path(
    const dcc_ctx_t *ctx,
    char *buffer,
    size_t buffer_len
) {
    if (ctx == NULL || buffer == NULL || buffer_len == 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *subcommand = dcc_ctx_subcommand(ctx);
    if (subcommand == NULL || subcommand[0] == '\0') {
        return DCC_ERR_NOT_FOUND;
    }
    const char *group = dcc_ctx_subcommand_group(ctx);
    int written = group != NULL && group[0] != '\0'
        ? snprintf(buffer, buffer_len, "%s/%s", group, subcommand)
        : snprintf(buffer, buffer_len, "%s", subcommand);
    if (written < 0 || (size_t)written >= buffer_len) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

dcc_status_t dcc_app_subcommand_route(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_handler_fn handler,
    void *user_data,
    dcc_app_route_id_t *out_route
) {
    char key[256];
    dcc_status_t status =
        dcc_app_subcommand_key(command_name, subcommand_path, key, sizeof(key));
    if (status != DCC_OK) {
        return status;
    }
    return dcc_app_add_route(
        app,
        DCC_EVENT_SLASH_COMMAND,
        key,
        0U,
        handler,
        user_data,
        out_route
    );
}

dcc_status_t dcc_app_subcommand(
    dcc_app_t *app,
    const char *command_name,
    const char *subcommand_path,
    dcc_app_handler_fn handler,
    void *user_data
) {
    return dcc_app_subcommand_route(
        app,
        command_name,
        subcommand_path,
        handler,
        user_data,
        NULL
    );
}
