#include "internal/command_registry/dcc_command_registry_internal.h"

#include <stddef.h>
#include <string.h>

void dcc_command_registry_options_init(dcc_command_registry_options_t *options) {
    if (options == NULL) {
        return;
    }
    memset(options, 0, sizeof(*options));
    options->size = sizeof(*options);
    options->version = DCC_COMMAND_REGISTRY_OPTIONS_VERSION;
}

dcc_status_t dcc_command_registry_options_set_global(dcc_command_registry_options_t *options) {
    if (options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->size == 0) {
        dcc_command_registry_options_init(options);
    }
    options->guild_id = 0;
    options->present &= ~DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_GUILD_ID;
    return DCC_OK;
}

dcc_status_t dcc_command_registry_options_set_guild(
    dcc_command_registry_options_t *options,
    dcc_snowflake_t guild_id
) {
    if (options == NULL || guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->size == 0) {
        dcc_command_registry_options_init(options);
    }
    options->guild_id = guild_id;
    options->present |= DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_GUILD_ID;
    return DCC_OK;
}

dcc_status_t dcc_command_registry_options_set_delete_stale(
    dcc_command_registry_options_t *options,
    uint8_t delete_stale
) {
    if (options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->size == 0) {
        dcc_command_registry_options_init(options);
    }
    if (delete_stale > 1U) return DCC_ERR_INVALID_ARG;
    options->delete_stale = delete_stale;
    options->present |= DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DELETE_STALE;
    return DCC_OK;
}

dcc_status_t dcc_command_registry_options_set_dry_run(
    dcc_command_registry_options_t *options,
    uint8_t dry_run
) {
    if (options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (options->size == 0) {
        dcc_command_registry_options_init(options);
    }
    if (dry_run > 1U) return DCC_ERR_INVALID_ARG;
    options->dry_run = dry_run;
    options->present |= DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DRY_RUN;
    return DCC_OK;
}

dcc_snowflake_t dcc_command_registry_options_guild_id(
    const dcc_command_registry_options_t *options
) {
    if (options == NULL ||
        options->version != DCC_COMMAND_REGISTRY_OPTIONS_VERSION ||
        (options->present & DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_GUILD_ID) == 0U ||
        options->size < offsetof(dcc_command_registry_options_t, guild_id) + sizeof(options->guild_id)) {
        return 0;
    }
    return options->guild_id;
}

uint8_t dcc_command_registry_options_delete_stale(
    const dcc_command_registry_options_t *options
) {
    if (options == NULL ||
        options->version != DCC_COMMAND_REGISTRY_OPTIONS_VERSION ||
        (options->present & DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DELETE_STALE) == 0U ||
        options->size < offsetof(dcc_command_registry_options_t, delete_stale) + sizeof(options->delete_stale)) {
        return 0;
    }
    return options->delete_stale != 0;
}

uint8_t dcc_command_registry_options_dry_run(
    const dcc_command_registry_options_t *options
) {
    if (options == NULL ||
        options->version != DCC_COMMAND_REGISTRY_OPTIONS_VERSION ||
        (options->present & DCC_COMMAND_REGISTRY_OPTIONS_PRESENT_DRY_RUN) == 0U ||
        options->size < offsetof(dcc_command_registry_options_t, dry_run) + sizeof(options->dry_run)) {
        return 0;
    }
    return options->dry_run != 0;
}
