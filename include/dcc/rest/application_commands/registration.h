#ifndef DCC_REST_APPLICATION_COMMANDS_REGISTRATION_H
#define DCC_REST_APPLICATION_COMMANDS_REGISTRATION_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API void dcc_application_command_registration_options_init(
    dcc_application_command_registration_options_t *options
);

DCC_API dcc_status_t dcc_application_command_registration_options_set_global(
    dcc_application_command_registration_options_t *options
);

DCC_API dcc_status_t dcc_application_command_registration_options_set_guild(
    dcc_application_command_registration_options_t *options,
    dcc_snowflake_t guild_id
);

DCC_API dcc_status_t dcc_rest_create_application_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_application_command_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_bulk_overwrite_application_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_bulk_overwrite_application_commands_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_all_application_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
