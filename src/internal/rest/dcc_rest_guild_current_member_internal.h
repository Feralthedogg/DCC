#ifndef DCC_REST_GUILD_CURRENT_MEMBER_INTERNAL_H
#define DCC_REST_GUILD_CURRENT_MEMBER_INTERNAL_H

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_current_guild_member_body(
    const dcc_current_guild_member_params_t *params,
    char **out
);
dcc_status_t dcc_rest_build_member_nickname_body(
    const char *nick,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
