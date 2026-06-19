#ifndef DCC_REST_GUILD_MEMBER_BUILDERS_INTERNAL_H
#define DCC_REST_GUILD_MEMBER_BUILDERS_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_guild_member_body(
    const dcc_guild_member_params_t *params,
    int include_access_token,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
