#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_application_command_builders_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"

dcc_status_t dcc_rest_bulk_overwrite_global_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *json_body,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/applications/%llu/commands", (unsigned long long)application_id);
    return status == DCC_OK ? dcc_rest_request_method(client, DCC_REST_PUT, path, json_body, cb, user_data) : status;
}

dcc_status_t dcc_rest_bulk_overwrite_global_commands_builder(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char path[80];
    dcc_status_t status = dcc_rest_format_path(path, sizeof(path), "/applications/%llu/commands", (unsigned long long)application_id);
    return status == DCC_OK
        ? dcc_rest_request_application_command_builder_array_json(
              client,
              DCC_REST_PUT,
              path,
              commands,
              command_count,
              cb,
              user_data
          )
        : status;
}

dcc_status_t dcc_rest_delete_all_global_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_rest_cb cb,
    void *user_data
) {
    return dcc_rest_bulk_overwrite_global_commands(client, application_id, "[]", cb, user_data);
}
