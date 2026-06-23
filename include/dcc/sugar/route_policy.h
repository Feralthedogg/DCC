#ifndef DCC_SUGAR_ROUTE_POLICY_H
#define DCC_SUGAR_ROUTE_POLICY_H

#include <dcc/app.h>

#define DCC_ROUTE_POLICY(...) \
    ((dcc_app_route_policy_t){ \
        .size = sizeof(dcc_app_route_policy_t), \
        __VA_ARGS__ \
    })

#define DCC_ROUTE_NO_POLICY() \
    ((dcc_app_route_policy_t){ \
        .size = sizeof(dcc_app_route_policy_t) \
    })

#define DCC_ROUTE_MIDDLEWARE(...) \
    .middlewares = (dcc_app_extension_middleware_t[]){ __VA_ARGS__ }, \
    .middleware_count = sizeof((dcc_app_extension_middleware_t[]){ __VA_ARGS__ }) / \
        sizeof(dcc_app_extension_middleware_t)

#define DCC_ROUTE_GUILD_ONLY() \
    .guild_only = 1U

#define DCC_ROUTE_DM_ONLY() \
    .dm_only = 1U

#define DCC_ROUTE_NSFW_ONLY() \
    .nsfw_only = 1U

#define DCC_ROUTE_CHANNEL_TYPES(...) \
    .channel_types = (uint32_t[]){ __VA_ARGS__ }, \
    .channel_type_count = sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t)

#define DCC_ROUTE_OWNER(user_id_) \
    .owner_user_ids = (dcc_snowflake_t[]){ (user_id_) }, \
    .owner_user_id_count = 1U

#define DCC_ROUTE_OWNERS(...) \
    .owner_user_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .owner_user_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_ROUTE_REQUIRED_PERMISSIONS(permissions_) \
    .required_permissions = (permissions_)

#define DCC_ROUTE_REQUIRE_ROLE(role_id_) \
    .required_role_ids = (dcc_snowflake_t[]){ (role_id_) }, \
    .required_role_id_count = 1U

#define DCC_ROUTE_REQUIRE_ROLES(...) \
    .required_role_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .required_role_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_ROUTE_REQUIRE_ANY_ROLE(...) \
    .any_role_ids = (dcc_snowflake_t[]){ __VA_ARGS__ }, \
    .any_role_id_count = sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t)

#define DCC_CHECK(check_, user_data_) \
    ((dcc_app_check_t){ \
        .size = sizeof(dcc_app_check_t), \
        .check = (check_), \
        .user_data = (user_data_) \
    })

#define DCC_CHECK_MESSAGE(check_, user_data_, title_, description_) \
    ((dcc_app_check_t){ \
        .size = sizeof(dcc_app_check_t), \
        .check = (check_), \
        .user_data = (user_data_), \
        .title = (title_), \
        .description = (description_) \
    })

#define DCC_ROUTE_CHECKS(...) \
    .checks = (dcc_app_check_t[]){ __VA_ARGS__ }, \
    .check_count = sizeof((dcc_app_check_t[]){ __VA_ARGS__ }) / sizeof(dcc_app_check_t)

#define DCC_ROUTE_CHECK(check_, user_data_) \
    DCC_ROUTE_CHECKS(DCC_CHECK((check_), (user_data_)))

#define DCC_ROUTE_CHECK_MESSAGE(check_, user_data_, title_, description_) \
    DCC_ROUTE_CHECKS(DCC_CHECK_MESSAGE((check_), (user_data_), (title_), (description_)))

#define DCC_COOLDOWN(bucket_, limit_, window_ms_) \
    ((dcc_app_cooldown_options_t){ \
        .size = sizeof(dcc_app_cooldown_options_t), \
        .bucket = (bucket_), \
        .limit = (limit_), \
        .window_ms = (window_ms_) \
    })

#define DCC_COOLDOWN_GLOBAL(limit_, window_ms_) \
    DCC_COOLDOWN(DCC_APP_COOLDOWN_GLOBAL, (limit_), (window_ms_))

#define DCC_COOLDOWN_USER(limit_, window_ms_) \
    DCC_COOLDOWN(DCC_APP_COOLDOWN_USER, (limit_), (window_ms_))

#define DCC_COOLDOWN_CHANNEL(limit_, window_ms_) \
    DCC_COOLDOWN(DCC_APP_COOLDOWN_CHANNEL, (limit_), (window_ms_))

#define DCC_COOLDOWN_GUILD(limit_, window_ms_) \
    DCC_COOLDOWN(DCC_APP_COOLDOWN_GUILD, (limit_), (window_ms_))

#define DCC_ROUTE_COOLDOWN(options_) \
    .cooldown = (options_)

#define DCC_ROUTE_COOLDOWN_GLOBAL(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN(DCC_COOLDOWN_GLOBAL((limit_), (window_ms_)))

#define DCC_ROUTE_COOLDOWN_USER(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN(DCC_COOLDOWN_USER((limit_), (window_ms_)))

#define DCC_ROUTE_COOLDOWN_CHANNEL(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN(DCC_COOLDOWN_CHANNEL((limit_), (window_ms_)))

#define DCC_ROUTE_COOLDOWN_GUILD(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN(DCC_COOLDOWN_GUILD((limit_), (window_ms_)))

#endif
