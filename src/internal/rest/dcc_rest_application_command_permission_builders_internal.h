#ifndef DCC_REST_APPLICATION_COMMAND_PERMISSION_BUILDERS_INTERNAL_H
#define DCC_REST_APPLICATION_COMMAND_PERMISSION_BUILDERS_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_guild_command_permissions_body(
    const dcc_guild_command_permissions_params_t *params,
    char **out
);
dcc_status_t dcc_rest_build_bulk_guild_command_permissions_body(
    const dcc_bulk_guild_command_permissions_params_t *params,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
