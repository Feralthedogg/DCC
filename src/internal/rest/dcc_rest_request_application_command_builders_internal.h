#ifndef DCC_REST_REQUEST_APPLICATION_COMMAND_BUILDERS_INTERNAL_H
#define DCC_REST_REQUEST_APPLICATION_COMMAND_BUILDERS_INTERNAL_H

#include <dcc/rest.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_request_application_command_builder_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_application_command_builder_t *command,
    dcc_rest_cb cb,
    void *user_data
);
dcc_status_t dcc_rest_request_application_command_builder_array_json(
    dcc_client_t *client,
    dcc_rest_method_t method,
    const char *path,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
