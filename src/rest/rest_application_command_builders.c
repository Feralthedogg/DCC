#include "internal/rest/dcc_rest_request_application_command_builders_internal.h"
#include "internal/rest/dcc_rest_request_core_internal.h"
#include "internal/objects/dcc_application_command_builder_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

static dcc_status_t dcc_rest_validate_application_command_create_array(
    const dcc_application_command_builder_t *commands,
    size_t command_count
) {
    size_t stride = 0U;
    dcc_status_t status = dcc_application_command_builder_array_begin(
        commands, command_count, &stride
    );
    for (size_t i = 0U; status == DCC_OK && i < command_count; ++i) {
        const dcc_application_command_builder_t *command =
            (const dcc_application_command_builder_t *)dcc_builder_abi_array_at(
                commands, stride, i
            );
        dcc_builder_abi_view_t view;
        status = dcc_application_command_builder_abi_validate(command, &view);
        if (status == DCC_OK && view.size != stride) {
            status = DCC_ERR_INVALID_ARG;
        }
        if (status == DCC_OK) {
            status = dcc_application_command_builder_validate_create(command);
        }
    }
    return status;
}

dcc_status_t dcc_rest_request_application_command_builder_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
) {
    if (method == DCC_REST_POST &&
        dcc_application_command_builder_validate_create(command) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
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
    if (method == DCC_REST_PUT &&
        dcc_rest_validate_application_command_create_array(commands, command_count) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    char *json = NULL;
    dcc_status_t status = dcc_application_command_builder_build_array_json(commands, command_count, &json);
    if (status != DCC_OK) {
        return status;
    }
    status = dcc_rest_request_method(client, method, path, json, cb, user_data);
    dcc_application_command_builder_json_free(json);
    return status;
}
