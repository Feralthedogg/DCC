#ifndef DCC_SUGAR_ROUTE_GROUP_H
#define DCC_SUGAR_ROUTE_GROUP_H

#include <dcc/app.h>

#define DCC_GROUP(app_, custom_id_prefix_) \
    ((dcc_app_route_group_t){ \
        .size = sizeof(dcc_app_route_group_t), \
        .app = (app_), \
        .custom_id_prefix = (custom_id_prefix_) \
    })

#define DCC_GROUP_DATA(app_, custom_id_prefix_, user_data_) \
    ((dcc_app_route_group_t){ \
        .size = sizeof(dcc_app_route_group_t), \
        .app = (app_), \
        .custom_id_prefix = (custom_id_prefix_), \
        .user_data = (user_data_) \
    })

#define DCC_GROUP_INIT(group_, app_, custom_id_prefix_) \
    dcc_app_route_group_init((group_), (app_), (custom_id_prefix_), NULL)

#define DCC_GROUP_INIT_DATA(group_, app_, custom_id_prefix_, user_data_) \
    dcc_app_route_group_init((group_), (app_), (custom_id_prefix_), (user_data_))

#define DCC_GROUP_BUTTON(group_, action_, handler_) \
    dcc_app_route_group_button((group_), (action_), (handler_))

#define DCC_GROUP_BUTTON_ROUTE(group_, action_, handler_, out_route_) \
    dcc_app_route_group_button_route((group_), (action_), (handler_), (out_route_))

#define DCC_GROUP_BUTTON_PREFIX(group_, action_prefix_, handler_) \
    dcc_app_route_group_button_prefix((group_), (action_prefix_), (handler_))

#define DCC_GROUP_BUTTON_PREFIX_ROUTE(group_, action_prefix_, handler_, out_route_) \
    dcc_app_route_group_button_prefix_route((group_), (action_prefix_), (handler_), (out_route_))

#define DCC_GROUP_SELECT(group_, action_, handler_) \
    dcc_app_route_group_select((group_), (action_), (handler_))

#define DCC_GROUP_SELECT_ROUTE(group_, action_, handler_, out_route_) \
    dcc_app_route_group_select_route((group_), (action_), (handler_), (out_route_))

#define DCC_GROUP_SELECT_PREFIX(group_, action_prefix_, handler_) \
    dcc_app_route_group_select_prefix((group_), (action_prefix_), (handler_))

#define DCC_GROUP_SELECT_PREFIX_ROUTE(group_, action_prefix_, handler_, out_route_) \
    dcc_app_route_group_select_prefix_route((group_), (action_prefix_), (handler_), (out_route_))

#define DCC_GROUP_MODAL(group_, action_, handler_) \
    dcc_app_route_group_modal((group_), (action_), (handler_))

#define DCC_GROUP_MODAL_ROUTE(group_, action_, handler_, out_route_) \
    dcc_app_route_group_modal_route((group_), (action_), (handler_), (out_route_))

#define DCC_GROUP_MODAL_PREFIX(group_, action_prefix_, handler_) \
    dcc_app_route_group_modal_prefix((group_), (action_prefix_), (handler_))

#define DCC_GROUP_MODAL_PREFIX_ROUTE(group_, action_prefix_, handler_, out_route_) \
    dcc_app_route_group_modal_prefix_route((group_), (action_prefix_), (handler_), (out_route_))

#define DCC_GROUP_VIEW(group_, view_) dcc_app_route_group_view_register((group_), (view_))

#define DCC_GROUP_USE(group_, middleware_, user_data_) \
    dcc_app_route_group_use((group_), (middleware_), (user_data_))

#define DCC_GROUP_GUILD_ONLY(group_) dcc_app_route_group_use_guild_only((group_))
#define DCC_GROUP_DM_ONLY(group_) dcc_app_route_group_use_dm_only((group_))
#define DCC_GROUP_NSFW_ONLY(group_) dcc_app_route_group_use_nsfw_only((group_))

#define DCC_GROUP_CHANNEL_TYPES(group_, ...) \
    dcc_app_route_group_use_channel_types( \
        (group_), \
        (uint32_t[]){ __VA_ARGS__ }, \
        sizeof((uint32_t[]){ __VA_ARGS__ }) / sizeof(uint32_t) \
    )

#define DCC_GROUP_OWNER(group_, user_id_) \
    dcc_app_route_group_use_owner((group_), (user_id_))

#define DCC_GROUP_OWNERS(group_, ...) \
    dcc_app_route_group_use_user_allowlist( \
        (group_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )

#define DCC_GROUP_REQUIRED_PERMISSIONS(group_, permissions_) \
    dcc_app_route_group_use_required_permissions((group_), (permissions_))

#define DCC_GROUP_REQUIRE_ROLE(group_, role_id_) \
    dcc_app_route_group_use_required_roles((group_), (dcc_snowflake_t[]){ (role_id_) }, 1U)

#define DCC_GROUP_REQUIRE_ROLES(group_, ...) \
    dcc_app_route_group_use_required_roles( \
        (group_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )

#define DCC_GROUP_REQUIRE_ANY_ROLE(group_, ...) \
    dcc_app_route_group_use_any_role( \
        (group_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )

#define DCC_GROUP_CHECK(group_, check_) dcc_app_route_group_use_check((group_), (check_))
#define DCC_GROUP_COOLDOWN(group_, options_) dcc_app_route_group_use_cooldown((group_), (options_))

#define DCC_GROUP_POLICY(group_, policy_) \
    dcc_app_route_group_apply_policy( \
        (group_), \
        &((dcc_app_route_policy_t[]){ (policy_) })[0] \
    )

#endif
