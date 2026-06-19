#ifndef DCC_REST_GUILD_BODY_INTERNAL_H
#define DCC_REST_GUILD_BODY_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_guild_body(
    const dcc_guild_params_t *params,
    int with_id,
    char **out
);
int dcc_rest_guild_params_valid(const dcc_guild_params_t *params, int with_id);
dcc_status_t dcc_rest_guild_body_append_tail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_guild_params_t *params
);

#ifdef __cplusplus
}
#endif

#endif
