#include <dcc/rest/application_commands/global_commands/bulk.h>
#include <dcc/rest/application_commands/global_commands/write.h>
#include <dcc/rest/application_commands/guild_commands/bulk.h>
#include <dcc/rest/application_commands/guild_commands/write.h>
#include <dcc/rest/application_commands/registration.h>

#include <stddef.h>

static dcc_snowflake_t dcc_application_command_registration_guild_id(
    const dcc_application_command_registration_options_t *options
) {
    if (options == NULL ||
        options->size < offsetof(dcc_application_command_registration_options_t, guild_id) +
                sizeof(options->guild_id)) {
        return 0;
    }
    return options->guild_id;
}

dcc_status_t dcc_rest_create_application_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t guild_id = dcc_application_command_registration_guild_id(options);
    return guild_id != 0
        ? dcc_rest_create_guild_command(client, application_id, guild_id, json_body, cb, user_data)
        : dcc_rest_create_global_command(client, application_id, json_body, cb, user_data);
}

dcc_status_t dcc_rest_create_application_command_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t guild_id = dcc_application_command_registration_guild_id(options);
    return guild_id != 0
        ? dcc_rest_create_guild_command_builder(client, application_id, guild_id, command, cb, user_data)
        : dcc_rest_create_global_command_builder(client, application_id, command, cb, user_data);
}

dcc_status_t dcc_rest_bulk_overwrite_application_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t guild_id = dcc_application_command_registration_guild_id(options);
    return guild_id != 0
        ? dcc_rest_bulk_overwrite_guild_commands(client, application_id, guild_id, json_body, cb, user_data)
        : dcc_rest_bulk_overwrite_global_commands(client, application_id, json_body, cb, user_data);
}

dcc_status_t dcc_rest_bulk_overwrite_application_commands_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    dcc_rest_cb cb,
    void *user_data
) {
    dcc_snowflake_t guild_id = dcc_application_command_registration_guild_id(options);
    return guild_id != 0
        ? dcc_rest_bulk_overwrite_guild_commands_builder(
              client,
              application_id,
              guild_id,
              commands,
              command_count,
              cb,
              user_data
          )
        : dcc_rest_bulk_overwrite_global_commands_builder(
              client,
              application_id,
              commands,
              command_count,
              cb,
              user_data
          );
}

dcc_status_t dcc_rest_delete_all_application_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_registration_options_t *options,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_bulk_overwrite_application_commands(client, application_id, options, "[]", cb, user_data);
}
