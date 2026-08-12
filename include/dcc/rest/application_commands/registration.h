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

#ifdef __cplusplus
}
#endif

#endif
