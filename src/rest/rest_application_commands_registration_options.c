#include <dcc/rest/application_commands/registration.h>

void dcc_application_command_registration_options_init(
    dcc_application_command_registration_options_t *options
) {
    if (options == NULL) {
        return;
    }

    options->size = sizeof(*options);
    options->guild_id = 0;
}

dcc_status_t dcc_application_command_registration_options_set_global(
    dcc_application_command_registration_options_t *options
) {
    if (options == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    options->size = sizeof(*options);
    options->guild_id = 0;
    return DCC_OK;
}

dcc_status_t dcc_application_command_registration_options_set_guild(
    dcc_application_command_registration_options_t *options,
    dcc_snowflake_t guild_id
) {
    if (options == NULL || guild_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }

    options->size = sizeof(*options);
    options->guild_id = guild_id;
    return DCC_OK;
}
