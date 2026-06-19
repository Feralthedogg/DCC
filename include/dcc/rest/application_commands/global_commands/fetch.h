#ifndef DCC_REST_APPLICATION_COMMANDS_GLOBAL_COMMANDS_FETCH_H
#define DCC_REST_APPLICATION_COMMANDS_GLOBAL_COMMANDS_FETCH_H

#include <dcc/rest/types.h>

#ifdef __cplusplus
extern "C" {
#endif

DCC_API dcc_status_t dcc_rest_get_global_commands(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    const char *query,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_global_commands_with_localizations(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    uint8_t with_localizations,
    dcc_rest_cb cb,
    void *user_data
);

DCC_API dcc_status_t dcc_rest_get_global_command(
    dcc_client_t *client,
    dcc_snowflake_t application_id,
    dcc_snowflake_t command_id,
    dcc_rest_cb cb,
    void *user_data
);

#ifdef __cplusplus
}
#endif

#endif
