#ifndef DCC_REST_APPLICATION_COMMANDS_GUILD_COMMANDS_WRITE_H
#define DCC_REST_APPLICATION_COMMANDS_GUILD_COMMANDS_WRITE_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_create_guild_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_create_guild_command_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_edit_guild_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_edit_guild_command_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_delete_guild_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t guild_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
