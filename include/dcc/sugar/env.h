#ifndef DCC_SUGAR_ENV_H
#define DCC_SUGAR_ENV_H

#include <dcc/app.h>

#define DCC_ENV_TOKEN(out_) \
    dcc_app_env_get_token(NULL, (out_))

#define DCC_ENV_TOKEN_NAMED(name_, out_) \
    dcc_app_env_get_token((name_), (out_))

#define DCC_ENV_STRING(name_, out_) \
    dcc_app_env_get_string((name_), (out_))

#define DCC_ENV_STRING_OR(name_, fallback_, out_) \
    dcc_app_env_get_string_or((name_), (fallback_), (out_))

#define DCC_ENV_BOOL(name_, out_) \
    dcc_app_env_get_bool((name_), (out_))

#define DCC_ENV_BOOL_OR(name_, fallback_, out_) \
    dcc_app_env_get_bool_or((name_), (fallback_), (out_))

#define DCC_ENV_U64(name_, out_) \
    dcc_app_env_get_u64((name_), (out_))

#define DCC_ENV_U64_OR(name_, fallback_, out_) \
    dcc_app_env_get_u64_or((name_), (fallback_), (out_))

#define DCC_ENV_I64(name_, out_) \
    dcc_app_env_get_i64((name_), (out_))

#define DCC_ENV_I64_OR(name_, fallback_, out_) \
    dcc_app_env_get_i64_or((name_), (fallback_), (out_))

#define DCC_ENV_SNOWFLAKE(name_, out_) \
    dcc_app_env_get_snowflake((name_), (out_))

#define DCC_ENV_SNOWFLAKE_OR(name_, fallback_, out_) \
    dcc_app_env_get_snowflake_or((name_), (fallback_), (out_))

#define DCC_ENV_CHANNEL(name_, out_) \
    dcc_app_env_get_channel((name_), (out_))

#define DCC_ENV_CHANNEL_OR(name_, fallback_, out_) \
    dcc_app_env_get_channel_or((name_), (fallback_), (out_))

#define DCC_ENV_ROLE(name_, out_) \
    dcc_app_env_get_role((name_), (out_))

#define DCC_ENV_ROLE_OR(name_, fallback_, out_) \
    dcc_app_env_get_role_or((name_), (fallback_), (out_))

#define DCC_ENV_USER(name_, out_) \
    dcc_app_env_get_user((name_), (out_))

#define DCC_ENV_USER_OR(name_, fallback_, out_) \
    dcc_app_env_get_user_or((name_), (fallback_), (out_))

#define DCC_ENV_GUILD(name_, out_) \
    dcc_app_env_get_guild((name_), (out_))

#define DCC_ENV_GUILD_OR(name_, fallback_, out_) \
    dcc_app_env_get_guild_or((name_), (fallback_), (out_))

#define DCC_CONFIG(...) \
    dcc_app_env_bind( \
        (dcc_app_env_binding_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_app_env_binding_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_env_binding_t) \
    )

#define DCC_CONFIG_STRING(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_STRING, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_STRING_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_STRING, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_STRING_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_STRING, \
        .out = (out_) \
    })

#define DCC_CONFIG_STRING_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_STRING, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_string = (fallback_) \
    })

#define DCC_CONFIG_BOOL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_BOOL, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_BOOL_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_BOOL, \
        .out = (out_) \
    })

#define DCC_CONFIG_BOOL_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_BOOL, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_bool = (fallback_) ? 1U : 0U \
    })

#define DCC_CONFIG_U64(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_U64, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_U64_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_U64, \
        .out = (out_) \
    })

#define DCC_CONFIG_U64_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_U64, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_u64 = (fallback_) \
    })

#define DCC_CONFIG_I64(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_I64, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_I64_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_I64, \
        .out = (out_) \
    })

#define DCC_CONFIG_I64_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_I64, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_i64 = (fallback_) \
    })

#define DCC_CONFIG_SNOWFLAKE(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_SNOWFLAKE, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_SNOWFLAKE_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_SNOWFLAKE, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_SNOWFLAKE_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_SNOWFLAKE, \
        .out = (out_) \
    })

#define DCC_CONFIG_SNOWFLAKE_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_SNOWFLAKE, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_snowflake = (fallback_) \
    })

#define DCC_CONFIG_CHANNEL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_CHANNEL, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_CHANNEL_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_CHANNEL, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_CHANNEL_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_CHANNEL, \
        .out = (out_) \
    })

#define DCC_CONFIG_CHANNEL_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_CHANNEL, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_snowflake = (fallback_) \
    })

#define DCC_CONFIG_ROLE(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_ROLE, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_ROLE_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_ROLE, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_ROLE_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_ROLE, \
        .out = (out_) \
    })

#define DCC_CONFIG_ROLE_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_ROLE, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_snowflake = (fallback_) \
    })

#define DCC_CONFIG_USER(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_USER, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_USER_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_USER, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_USER_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_USER, \
        .out = (out_) \
    })

#define DCC_CONFIG_USER_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_USER, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_snowflake = (fallback_) \
    })

#define DCC_CONFIG_GUILD(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_GUILD, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_GUILD_DESCRIBED(name_, description_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .description = (description_), \
        .type = DCC_APP_ENV_BIND_GUILD, \
        .out = (out_), \
        .required = 1U \
    })

#define DCC_CONFIG_GUILD_OPTIONAL(name_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_GUILD, \
        .out = (out_) \
    })

#define DCC_CONFIG_GUILD_DEFAULT(name_, fallback_, out_) \
    ((dcc_app_env_binding_t){ \
        .size = sizeof(dcc_app_env_binding_t), \
        .name = (name_), \
        .type = DCC_APP_ENV_BIND_GUILD, \
        .out = (out_), \
        .has_fallback = 1U, \
        .fallback_snowflake = (fallback_) \
    })

#endif
