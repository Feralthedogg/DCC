#ifndef DCC_APP_ENV_H
#define DCC_APP_ENV_H

#include <dcc/error.h>
#include <dcc/export.h>
#include <dcc/intents.h>
#include <dcc/snowflake.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_app_env_requirement {
    size_t size;
    const char *name;
    const char *description;
} dcc_app_env_requirement_t;

typedef enum dcc_app_env_bind_type {
    DCC_APP_ENV_BIND_STRING = 1,
    DCC_APP_ENV_BIND_BOOL,
    DCC_APP_ENV_BIND_U64,
    DCC_APP_ENV_BIND_I64,
    DCC_APP_ENV_BIND_SNOWFLAKE,
    DCC_APP_ENV_BIND_CHANNEL,
    DCC_APP_ENV_BIND_ROLE,
    DCC_APP_ENV_BIND_USER,
    DCC_APP_ENV_BIND_GUILD,
    DCC_APP_ENV_BIND_INTENTS
} dcc_app_env_bind_type_t;

typedef struct dcc_app_env_binding {
    size_t size;
    const char *name;
    const char *description;
    dcc_app_env_bind_type_t type;
    void *out;
    uint8_t required;
    uint8_t has_fallback;
    const char *fallback_string;
    uint8_t fallback_bool;
    uint64_t fallback_u64;
    int64_t fallback_i64;
    dcc_snowflake_t fallback_snowflake;
    dcc_intents_t fallback_intents;
} dcc_app_env_binding_t;

DCC_API dcc_status_t dcc_app_load_env_file(const char *path, uint8_t overwrite);
DCC_API dcc_status_t dcc_app_load_dotenv(void);
DCC_API dcc_status_t dcc_app_env_get_token(const char *preferred_env, const char **out);
DCC_API dcc_status_t dcc_app_env_get_string(const char *name, const char **out);
DCC_API dcc_status_t dcc_app_env_get_string_or(
    const char *name,
    const char *fallback,
    const char **out
);
DCC_API dcc_status_t dcc_app_env_get_bool(const char *name, uint8_t *out);
DCC_API dcc_status_t dcc_app_env_get_bool_or(
    const char *name,
    uint8_t fallback,
    uint8_t *out
);
DCC_API dcc_status_t dcc_app_env_get_u64(const char *name, uint64_t *out);
DCC_API dcc_status_t dcc_app_env_get_u64_or(
    const char *name,
    uint64_t fallback,
    uint64_t *out
);
DCC_API dcc_status_t dcc_app_env_get_u32(const char *name, uint32_t *out);
DCC_API dcc_status_t dcc_app_env_get_u32_or(
    const char *name,
    uint32_t fallback,
    uint32_t *out
);
DCC_API dcc_status_t dcc_app_env_get_u32_range(
    const char *name,
    uint32_t min_value,
    uint32_t max_value,
    uint32_t *out
);
DCC_API dcc_status_t dcc_app_env_get_u32_range_or(
    const char *name,
    uint32_t fallback,
    uint32_t min_value,
    uint32_t max_value,
    uint32_t *out
);
DCC_API dcc_status_t dcc_app_env_get_i64(const char *name, int64_t *out);
DCC_API dcc_status_t dcc_app_env_get_i64_or(
    const char *name,
    int64_t fallback,
    int64_t *out
);
DCC_API dcc_status_t dcc_app_env_get_snowflake(const char *name, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_app_env_get_snowflake_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_app_env_get_channel(const char *name, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_app_env_get_channel_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_app_env_get_role(const char *name, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_app_env_get_role_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_app_env_get_user(const char *name, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_app_env_get_user_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_app_env_get_guild(const char *name, dcc_snowflake_t *out);
DCC_API dcc_status_t dcc_app_env_get_guild_or(
    const char *name,
    dcc_snowflake_t fallback,
    dcc_snowflake_t *out
);
DCC_API dcc_status_t dcc_app_env_get_intents(const char *name, dcc_intents_t *out);
DCC_API dcc_status_t dcc_app_env_get_intents_or(
    const char *name,
    dcc_intents_t fallback,
    dcc_intents_t *out
);
DCC_API dcc_status_t dcc_app_env_bind(const dcc_app_env_binding_t *bindings, size_t count);
DCC_API dcc_status_t dcc_app_validate_env_requirements(
    const dcc_app_env_requirement_t *requirements,
    size_t count
);

#ifdef __cplusplus
}
#endif

#endif
