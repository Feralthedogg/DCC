#ifndef DCC_REST_APPLICATION_COMMANDS_GLOBAL_COMMANDS_BULK_H
#define DCC_REST_APPLICATION_COMMANDS_GLOBAL_COMMANDS_BULK_H

#include <dcc/rest/request.h>
#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_bulk_overwrite_global_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const dcc_application_command_builder_t *commands,
    size_t command_count,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request
);

#ifdef __cplusplus
}
#endif

#endif
