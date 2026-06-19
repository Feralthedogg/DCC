#ifndef DCC_REST_GUILD_PRUNE_BODY_INTERNAL_H
#define DCC_REST_GUILD_PRUNE_BODY_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_guild_prune_body(
    uint32_t days,
    const dcc_snowflake_t *include_roles,
    size_t include_role_count,
    uint8_t compute_prune_count,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
