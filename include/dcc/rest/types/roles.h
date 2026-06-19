#ifndef DCC_REST_TYPES_ROLES_H
#define DCC_REST_TYPES_ROLES_H

#include <dcc/rest/base.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_role_params {
    size_t size;
    dcc_snowflake_t guild_id;
    dcc_snowflake_t role_id;
    const char *name;
    uint32_t color;
    uint8_t position;
    uint64_t permissions;
    uint8_t hoist;
    uint8_t mentionable;
    const char *icon;
    const char *unicode_emoji;
} dcc_role_params_t;

typedef struct dcc_role_position {
    dcc_snowflake_t role_id;
    uint8_t position;
} dcc_role_position_t;

#ifdef __cplusplus
}
#endif

#endif
