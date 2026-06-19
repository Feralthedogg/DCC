#include "internal/rest/dcc_rest_request_application_command_builders_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"

dcc_status_t dcc_rest_request_application_command_builder_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_application_command_builder_build_json(command, &json);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_method(client, method, path, json, cb, user_data);
    dcc_application_command_builder_json_free(json);
    return status;
}

dcc_status_t dcc_rest_request_application_command_builder_array_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    dcc_rest_cb cb,
    void *user_data
) {
    char *json = NULL;
    dcc_status_t status = dcc_application_command_builder_build_array_json(commands, command_count, &json);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_method(client, method, path, json, cb, user_data);
    dcc_application_command_builder_json_free(json);
    return status;
}
