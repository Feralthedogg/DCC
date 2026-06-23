#ifndef DCC_SUGAR_GUARDS_H
#define DCC_SUGAR_GUARDS_H

#include <dcc/sugar/app_extension.h>

#define DCC_GUARDS(...) DCC_ROUTE_POLICY(__VA_ARGS__)
#define DCC_GUARD_POLICY(...) DCC_ROUTE_POLICY(__VA_ARGS__)
#define DCC_POLICY(...) DCC_ROUTE_POLICY(__VA_ARGS__)

#define DCC_GUARD_MIDDLEWARE(...) DCC_ROUTE_MIDDLEWARE(__VA_ARGS__)
#define DCC_GUARD_GUILD_ONLY() DCC_ROUTE_GUILD_ONLY()
#define DCC_GUARD_DM_ONLY() DCC_ROUTE_DM_ONLY()
#define DCC_GUARD_NSFW_ONLY() DCC_ROUTE_NSFW_ONLY()
#define DCC_GUARD_CHANNEL_TYPES(...) DCC_ROUTE_CHANNEL_TYPES(__VA_ARGS__)

#define DCC_GUARD_OWNER(user_id_) DCC_ROUTE_OWNER((user_id_))
#define DCC_GUARD_OWNERS(...) DCC_ROUTE_OWNERS(__VA_ARGS__)
#define DCC_GUARD_PERMISSIONS(permissions_) DCC_ROUTE_REQUIRED_PERMISSIONS((permissions_))
#define DCC_GUARD_HAS_PERMISSION(permissions_) DCC_ROUTE_REQUIRED_PERMISSIONS((permissions_))
#define DCC_GUARD_HAS_PERMISSIONS(permissions_) DCC_ROUTE_REQUIRED_PERMISSIONS((permissions_))

#define DCC_GUARD_HAS_ROLE(role_id_) DCC_ROUTE_REQUIRE_ROLE((role_id_))
#define DCC_GUARD_HAS_ROLES(...) DCC_ROUTE_REQUIRE_ROLES(__VA_ARGS__)
#define DCC_GUARD_HAS_ANY_ROLE(...) DCC_ROUTE_REQUIRE_ANY_ROLE(__VA_ARGS__)
#define DCC_GUARD_REQUIRE_ROLE(role_id_) DCC_ROUTE_REQUIRE_ROLE((role_id_))
#define DCC_GUARD_REQUIRE_ROLES(...) DCC_ROUTE_REQUIRE_ROLES(__VA_ARGS__)
#define DCC_GUARD_REQUIRE_ANY_ROLE(...) DCC_ROUTE_REQUIRE_ANY_ROLE(__VA_ARGS__)

#define DCC_GUARD_CHECK(check_, user_data_) DCC_ROUTE_CHECK((check_), (user_data_))
#define DCC_GUARD_CHECK_MESSAGE(check_, user_data_, title_, description_) \
    DCC_ROUTE_CHECK_MESSAGE((check_), (user_data_), (title_), (description_))

#define DCC_GUARD_COOLDOWN(options_) DCC_ROUTE_COOLDOWN((options_))
#define DCC_GUARD_COOLDOWN_GLOBAL(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN_GLOBAL((limit_), (window_ms_))
#define DCC_GUARD_COOLDOWN_USER(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN_USER((limit_), (window_ms_))
#define DCC_GUARD_COOLDOWN_CHANNEL(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN_CHANNEL((limit_), (window_ms_))
#define DCC_GUARD_COOLDOWN_GUILD(limit_, window_ms_) \
    DCC_ROUTE_COOLDOWN_GUILD((limit_), (window_ms_))

#define DCC_MIDDLEWARE(middleware_, user_data_) \
    DCC_EXTENSION_MIDDLEWARE((middleware_), (user_data_))
#define DCC_MIDDLEWARE_DATA(middleware_, user_data_) \
    DCC_EXTENSION_MIDDLEWARE((middleware_), (user_data_))

#define DCC_USE(...) DCC_GUARD_MIDDLEWARE(__VA_ARGS__)
#define DCC_ONLY_GUILD() DCC_GUARD_GUILD_ONLY()
#define DCC_ONLY_DM() DCC_GUARD_DM_ONLY()
#define DCC_ONLY_NSFW() DCC_GUARD_NSFW_ONLY()
#define DCC_IN_CHANNEL_TYPES(...) DCC_GUARD_CHANNEL_TYPES(__VA_ARGS__)

#define DCC_OWNER_ONLY(user_id_) DCC_GUARD_OWNER((user_id_))
#define DCC_OWNERS_ONLY(...) DCC_GUARD_OWNERS(__VA_ARGS__)
#define DCC_CAN(permissions_) DCC_GUARD_HAS_PERMISSIONS((permissions_))
#define DCC_CAN_ALL(permissions_) DCC_GUARD_HAS_PERMISSIONS((permissions_))
#define DCC_HAS_PERMISSION(permissions_) DCC_GUARD_HAS_PERMISSION((permissions_))
#define DCC_HAS_PERMISSIONS(permissions_) DCC_GUARD_HAS_PERMISSIONS((permissions_))

#define DCC_HAS_ROLE(role_id_) DCC_GUARD_HAS_ROLE((role_id_))
#define DCC_HAS_ROLES(...) DCC_GUARD_HAS_ROLES(__VA_ARGS__)
#define DCC_HAS_ANY_ROLE(...) DCC_GUARD_HAS_ANY_ROLE(__VA_ARGS__)

#define DCC_WITH_CHECK(check_, user_data_) DCC_GUARD_CHECK((check_), (user_data_))
#define DCC_WITH_CHECK_MESSAGE(check_, user_data_, title_, description_) \
    DCC_GUARD_CHECK_MESSAGE((check_), (user_data_), (title_), (description_))

#define DCC_RATE_LIMIT(options_) DCC_GUARD_COOLDOWN((options_))
#define DCC_RATE_LIMIT_GLOBAL(limit_, window_ms_) DCC_GUARD_COOLDOWN_GLOBAL((limit_), (window_ms_))
#define DCC_RATE_LIMIT_USER(limit_, window_ms_) DCC_GUARD_COOLDOWN_USER((limit_), (window_ms_))
#define DCC_RATE_LIMIT_CHANNEL(limit_, window_ms_) DCC_GUARD_COOLDOWN_CHANNEL((limit_), (window_ms_))
#define DCC_RATE_LIMIT_GUILD(limit_, window_ms_) DCC_GUARD_COOLDOWN_GUILD((limit_), (window_ms_))

#endif
