#ifndef DCC_SUGAR_CONTEXT_ACCESSORS_H
#define DCC_SUGAR_CONTEXT_ACCESSORS_H

#include <dcc/sugar/context/helpers.h>

#define DCC_CTX_APP(ctx_) dcc_ctx_app((ctx_))
#define DCC_CTX_CLIENT(ctx_) dcc_ctx_client((ctx_))
#define DCC_CTX_EVENT(ctx_) dcc_ctx_event((ctx_))
#define DCC_CTX_INTERACTION(ctx_) dcc_ctx_interaction((ctx_))
#define DCC_CTX_FLOW(ctx_) dcc_ctx_flow((ctx_))
#define DCC_CTX_EVENT_TYPE(ctx_) dcc_ctx_event_type((ctx_))
#define DCC_CTX_INTERACTION_ID(ctx_) dcc_ctx_interaction_id((ctx_))
#define DCC_CTX_APPLICATION_ID(ctx_) dcc_ctx_application_id((ctx_))
#define DCC_CTX_GUILD_ID(ctx_) dcc_ctx_guild_id((ctx_))
#define DCC_CTX_CHANNEL_ID(ctx_) dcc_ctx_channel_id((ctx_))
#define DCC_CTX_CHANNEL_TYPE(ctx_, out_type_) dcc_ctx_channel_type((ctx_), (out_type_))
#define DCC_CTX_CHANNEL_IS_TYPE(ctx_, channel_type_) \
    dcc_ctx_channel_is_type((ctx_), (channel_type_))
#define DCC_CTX_CHANNEL_IS_NSFW(ctx_) dcc_ctx_channel_is_nsfw((ctx_))
#define DCC_CTX_COMMAND_ID(ctx_) dcc_ctx_command_id((ctx_))
#define DCC_CTX_MESSAGE_ID(ctx_) dcc_ctx_message_id((ctx_))
#define DCC_CTX_TARGET_ID(ctx_) dcc_ctx_target_id((ctx_))
#define DCC_CTX_USER_ID(ctx_) dcc_ctx_user_id((ctx_))
#define DCC_CTX_AUTHOR_ID(ctx_) dcc_ctx_user_id((ctx_))
#define DCC_CTX_GUILD(ctx_) dcc_ctx_guild((ctx_))
#define DCC_CTX_CHANNEL(ctx_) dcc_ctx_channel((ctx_))
#define DCC_CTX_USER(ctx_) dcc_ctx_user((ctx_))
#define DCC_CTX_AUTHOR(ctx_) dcc_ctx_user((ctx_))
#define DCC_CTX_MEMBER(ctx_) dcc_ctx_member((ctx_))
#define DCC_CTX_RESOLVED_USER(ctx_, id_) dcc_ctx_resolved_user((ctx_), (id_))
#define DCC_CTX_RESOLVED_MEMBER(ctx_, user_id_) dcc_ctx_resolved_member((ctx_), (user_id_))
#define DCC_CTX_RESOLVED_ROLE(ctx_, id_) dcc_ctx_resolved_role((ctx_), (id_))
#define DCC_CTX_RESOLVED_CHANNEL(ctx_, id_) dcc_ctx_resolved_channel((ctx_), (id_))
#define DCC_CTX_RESOLVED_MESSAGE(ctx_, id_) dcc_ctx_resolved_message((ctx_), (id_))
#define DCC_CTX_RESOLVED_ATTACHMENT(ctx_, id_) dcc_ctx_resolved_attachment((ctx_), (id_))
#define DCC_CTX_TARGET_USER(ctx_) dcc_ctx_target_user((ctx_))
#define DCC_CTX_TARGET_MEMBER(ctx_) dcc_ctx_target_member((ctx_))
#define DCC_CTX_TARGET_ROLE(ctx_) dcc_ctx_target_role((ctx_))
#define DCC_CTX_TARGET_CHANNEL(ctx_) dcc_ctx_target_channel((ctx_))
#define DCC_CTX_TARGET_MESSAGE(ctx_) dcc_ctx_target_message((ctx_))
#define DCC_CTX_USER_DATA(ctx_) dcc_ctx_user_data((ctx_))
#define DCC_CTX_TOKEN(ctx_) dcc_ctx_token((ctx_))
#define DCC_CTX_NAME(ctx_) dcc_ctx_name((ctx_))
#define DCC_CTX_COMMAND_NAME(ctx_) dcc_ctx_name((ctx_))
#define DCC_CTX_CUSTOM_ID(ctx_) dcc_ctx_custom_id((ctx_))
#define DCC_CTX_SUBCOMMAND(ctx_) dcc_ctx_subcommand((ctx_))
#define DCC_CTX_SUBCOMMAND_GROUP(ctx_) dcc_ctx_subcommand_group((ctx_))
#define DCC_CTX_RESPONSE_STATE(ctx_) dcc_ctx_response_state((ctx_))
#define DCC_CTX_RESPONSE_STATE_NAME(ctx_) dcc_ctx_response_state_string((ctx_))
#define DCC_CTX_RESPONDED(ctx_) dcc_ctx_response_sent((ctx_))
#define DCC_CTX_RESPONSE_SENT(ctx_) dcc_ctx_response_sent((ctx_))
#define DCC_CTX_DEFERRED(ctx_) dcc_ctx_deferred((ctx_))
#define DCC_CTX_REPLIED(ctx_) dcc_ctx_replied((ctx_))
#define DCC_CTX_FOLLOWED_UP(ctx_) dcc_ctx_followed_up((ctx_))
#define DCC_CTX_RESPONSE_FAILED(ctx_) dcc_ctx_response_failed((ctx_))
#define DCC_CTX_CAN_FOLLOWUP(ctx_) dcc_ctx_can_followup((ctx_))
#define DCC_CTX_CAN_EDIT_ORIGINAL(ctx_) dcc_ctx_can_edit_original((ctx_))
#define DCC_CTX_HAS_MEMBER_PERMISSIONS(ctx_) dcc_ctx_has_member_permissions((ctx_))
#define DCC_CTX_MEMBER_PERMISSIONS(ctx_) dcc_ctx_member_permissions((ctx_))
#define DCC_CTX_MEMBER_HAS_PERMISSIONS(ctx_, permissions_) \
    dcc_ctx_member_has_permissions((ctx_), (permissions_))
#define DCC_CTX_MEMBER_HAS_ROLE(ctx_, role_id_) dcc_ctx_member_has_role((ctx_), (role_id_))
#define DCC_CTX_MEMBER_HAS_ANY_ROLE(ctx_, role_ids_, role_id_count_) \
    dcc_ctx_member_has_any_role((ctx_), (role_ids_), (role_id_count_))
#define DCC_CTX_MEMBER_HAS_ALL_ROLES(ctx_, role_ids_, role_id_count_) \
    dcc_ctx_member_has_all_roles((ctx_), (role_ids_), (role_id_count_))
#define DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS(ctx_, ...) \
    dcc_ctx_member_has_any_role( \
        (ctx_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )
#define DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS(ctx_, ...) \
    dcc_ctx_member_has_all_roles( \
        (ctx_), \
        (dcc_snowflake_t[]){ __VA_ARGS__ }, \
        sizeof((dcc_snowflake_t[]){ __VA_ARGS__ }) / sizeof(dcc_snowflake_t) \
    )
#define DCC_CTX_HAS_ROLE(ctx_, role_id_) DCC_CTX_MEMBER_HAS_ROLE((ctx_), (role_id_))
#define DCC_CTX_HAS_ANY_ROLE_IDS(ctx_, ...) DCC_CTX_MEMBER_HAS_ANY_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_CTX_HAS_ALL_ROLE_IDS(ctx_, ...) DCC_CTX_MEMBER_HAS_ALL_ROLE_IDS((ctx_), __VA_ARGS__)
#define DCC_CTX_HAS_PERMISSIONS(ctx_, permissions_) \
    dcc_ctx_member_has_permissions((ctx_), (permissions_))

#define DCC_USER(ctx_) DCC_CTX_USER((ctx_))
#define DCC_AUTHOR(ctx_) DCC_CTX_AUTHOR((ctx_))
#define DCC_MEMBER(ctx_) DCC_CTX_MEMBER((ctx_))
#define DCC_GUILD(ctx_) DCC_CTX_GUILD((ctx_))
#define DCC_CHANNEL(ctx_) DCC_CTX_CHANNEL((ctx_))
#define DCC_USER_ID(ctx_) DCC_CTX_USER_ID((ctx_))
#define DCC_AUTHOR_ID(ctx_) DCC_CTX_AUTHOR_ID((ctx_))
#define DCC_GUILD_ID(ctx_) DCC_CTX_GUILD_ID((ctx_))
#define DCC_CHANNEL_ID(ctx_) DCC_CTX_CHANNEL_ID((ctx_))
#define DCC_MESSAGE_ID(ctx_) DCC_CTX_MESSAGE_ID((ctx_))
#define DCC_TARGET_ID(ctx_) DCC_CTX_TARGET_ID((ctx_))
#define DCC_COMMAND_ID(ctx_) DCC_CTX_COMMAND_ID((ctx_))
#define DCC_COMMAND_NAME(ctx_) DCC_CTX_COMMAND_NAME((ctx_))
#define DCC_CUSTOM_ID(ctx_) DCC_CTX_CUSTOM_ID((ctx_))
#define DCC_CUSTOM_ID_OR(ctx_, fallback_) DCC_CTX_CUSTOM_ID_OR((ctx_), (fallback_))
#define DCC_ACTION(ctx_, prefix_) DCC_CTX_ACTION((ctx_), (prefix_))
#define DCC_USER_DATA(ctx_) DCC_CTX_USER_DATA((ctx_))

#define DCC_CTX_IS_GUILD(ctx_) dcc_sugar_ctx_is_guild((ctx_))
#define DCC_CTX_IS_DM(ctx_) dcc_sugar_ctx_is_dm((ctx_))
#define DCC_CTX_IS_AUTHOR(ctx_, user_id_) dcc_sugar_ctx_is_author((ctx_), (user_id_))
#define DCC_CTX_NAME_OR(ctx_, fallback_) dcc_sugar_ctx_name_or((ctx_), (fallback_))
#define DCC_CTX_TOKEN_OR(ctx_, fallback_) dcc_sugar_ctx_token_or((ctx_), (fallback_))
#define DCC_CTX_CUSTOM_ID_OR(ctx_, fallback_) dcc_sugar_ctx_custom_id_or((ctx_), (fallback_))
#define DCC_CTX_HAS_CUSTOM_ID(ctx_, custom_id_) dcc_sugar_ctx_has_custom_id((ctx_), (custom_id_))
#define DCC_CTX_CUSTOM_ID_STARTS_WITH(ctx_, prefix_) \
    dcc_sugar_ctx_custom_id_starts_with((ctx_), (prefix_))
#define DCC_CTX_CUSTOM_ID_SUFFIX(ctx_, prefix_) dcc_sugar_ctx_custom_id_suffix((ctx_), (prefix_))
#define DCC_CTX_ACTION(ctx_, prefix_) dcc_sugar_ctx_custom_id_suffix((ctx_), (prefix_))
#define DCC_CTX_COMPONENT_ACTION_OR(ctx_, namespace_, fallback_) \
    dcc_sugar_ctx_component_action_or((ctx_), (namespace_), (fallback_))
#define DCC_CTX_COMPONENT_ACTION(ctx_, namespace_) \
    dcc_sugar_ctx_component_action((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_IN_NS(ctx_, namespace_) \
    dcc_sugar_ctx_component_in_namespace((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_IN_NAMESPACE(ctx_, namespace_) \
    DCC_CTX_COMPONENT_IN_NS((ctx_), (namespace_))
#define DCC_CTX_COMPONENT_HAS_ACTION(ctx_, namespace_, action_) \
    dcc_sugar_ctx_component_has_action((ctx_), (namespace_), (action_))
#define DCC_CTX_COMPONENT_IS(ctx_, namespace_, action_) \
    DCC_CTX_COMPONENT_HAS_ACTION((ctx_), (namespace_), (action_))
#define DCC_CTX_ACTION_IN_NS(ctx_, namespace_) \
    DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_))
#define DCC_CTX_ACTION_IN_NAMESPACE(ctx_, namespace_) \
    DCC_CTX_COMPONENT_ACTION((ctx_), (namespace_))


#endif /* DCC_SUGAR_CONTEXT_ACCESSORS_H */
