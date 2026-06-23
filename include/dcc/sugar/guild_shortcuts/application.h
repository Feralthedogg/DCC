#ifndef DCC_SUGAR_GUILD_SHORTCUTS_APPLICATION_H
#define DCC_SUGAR_GUILD_SHORTCUTS_APPLICATION_H

#include <dcc/sugar/guild_shortcuts/base.h>

#define DCC_APP_GET_CURRENT_APPLICATION(app_) \
    dcc_app_get_current_application((app_), NULL, NULL)
#define DCC_APP_GET_GATEWAY_BOT(app_) \
    dcc_app_get_gateway_bot((app_), NULL, NULL)
#define DCC_APP_GET_SKUS(app_, application_id_) \
    dcc_app_get_skus((app_), (application_id_), NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS(app_, application_id_) \
    dcc_app_get_entitlements((app_), (application_id_), NULL, NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS_QUERY(app_, application_id_, query_) \
    dcc_app_get_entitlements((app_), (application_id_), (query_), NULL, NULL)
#define DCC_APP_GET_ENTITLEMENTS_PAGE( \
    app_, \
    application_id_, \
    user_id_, \
    sku_ids_, \
    sku_id_count_, \
    before_id_, \
    after_id_, \
    limit_, \
    guild_id_, \
    exclude_ended_ \
) \
    dcc_app_get_entitlements_page( \
        (app_), \
        (application_id_), \
        (user_id_), \
        (sku_ids_), \
        (sku_id_count_), \
        (before_id_), \
        (after_id_), \
        (limit_), \
        (guild_id_), \
        (exclude_ended_), \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_TEST_ENTITLEMENT(app_, application_id_, json_body_) \
    dcc_app_create_test_entitlement((app_), (application_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_TEST_ENTITLEMENT_PARAMS(app_, params_) \
    dcc_app_create_test_entitlement_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_TEST_ENTITLEMENT(app_, application_id_, entitlement_id_) \
    dcc_app_delete_test_entitlement((app_), (application_id_), (entitlement_id_), NULL, NULL)
#define DCC_APP_CONSUME_ENTITLEMENT(app_, application_id_, entitlement_id_) \
    dcc_app_consume_entitlement((app_), (application_id_), (entitlement_id_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER(app_) \
    dcc_app_get_current_user((app_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_USER(app_, json_body_) \
    dcc_app_modify_current_user((app_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CURRENT_USER_PARAMS(app_, params_) \
    dcc_app_modify_current_user_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_USER(app_, user_id_) \
    dcc_app_get_user((app_), (user_id_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_CONNECTIONS(app_) \
    dcc_app_get_current_user_connections((app_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_GUILDS(app_) \
    dcc_app_get_current_user_guilds((app_), NULL, NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_GUILDS_QUERY(app_, query_) \
    dcc_app_get_current_user_guilds((app_), (query_), NULL, NULL)
#define DCC_APP_GET_CURRENT_USER_DMS(app_) \
    dcc_app_get_current_user_dms((app_), NULL, NULL)
#define DCC_APP_CREATE_DM_CHANNEL(app_, json_body_) \
    dcc_app_create_dm_channel((app_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_DM_CHANNEL_PARAMS(app_, params_) \
    dcc_app_create_dm_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_ADD_GROUP_DM_RECIPIENT(app_, channel_id_, user_id_, json_body_) \
    dcc_app_add_group_dm_recipient((app_), (channel_id_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_ADD_GROUP_DM_RECIPIENT_PARAMS(app_, params_) \
    dcc_app_add_group_dm_recipient_params((app_), (params_), NULL, NULL)
#define DCC_APP_REMOVE_GROUP_DM_RECIPIENT(app_, channel_id_, user_id_) \
    dcc_app_remove_group_dm_recipient((app_), (channel_id_), (user_id_), NULL, NULL)
#define DCC_APP_CREATE_DIRECT_MESSAGE(app_, user_id_, json_body_) \
    dcc_app_create_direct_message((app_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_DIRECT_MESSAGE_BUILDER(app_, user_id_, message_) \
    dcc_app_create_direct_message_builder( \
        (app_), \
        (user_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_DIRECT_MESSAGE_TEXT(app_, user_id_, content_) \
    dcc_app_create_direct_message_text((app_), (user_id_), (content_), NULL, NULL)
#define DCC_APP_LEAVE_GUILD(app_, guild_id_) \
    dcc_app_leave_guild((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_INVITES(app_, guild_id_) \
    dcc_app_get_guild_invites((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_INVITE(app_, invite_code_) \
    dcc_app_get_invite((app_), (invite_code_), NULL, NULL, NULL)
#define DCC_APP_GET_INVITE_QUERY(app_, invite_code_, query_) \
    dcc_app_get_invite((app_), (invite_code_), (query_), NULL, NULL)
#define DCC_APP_GET_INVITE_FULL(app_, invite_code_) \
    dcc_app_get_invite_full((app_), (invite_code_), NULL, NULL)
#define DCC_APP_DELETE_INVITE(app_, invite_code_) \
    dcc_app_delete_invite((app_), (invite_code_), NULL, NULL)
#define DCC_APP_CREATE_STAGE(app_, json_body_) \
    dcc_app_create_stage_instance((app_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_STAGE_PARAMS(app_, params_) \
    dcc_app_create_stage_instance_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_STAGE(app_, channel_id_) \
    dcc_app_get_stage_instance((app_), (channel_id_), NULL, NULL)
#define DCC_APP_EDIT_STAGE(app_, channel_id_, json_body_) \
    dcc_app_modify_stage_instance((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_STAGE_PARAMS(app_, params_) \
    dcc_app_modify_stage_instance_params((app_), (params_), NULL, NULL)
#define DCC_APP_DELETE_STAGE(app_, channel_id_) \
    dcc_app_delete_stage_instance((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_TEMPLATE(app_, code_) \
    dcc_app_get_template((app_), (code_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_FROM_TEMPLATE(app_, code_, json_body_) \
    dcc_app_create_guild_from_template((app_), (code_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_FROM_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_create_guild_from_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_GUILD_TEMPLATES(app_, guild_id_) \
    dcc_app_get_guild_templates((app_), (guild_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_TEMPLATE(app_, guild_id_, json_body_) \
    dcc_app_create_guild_template((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_create_guild_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_TEMPLATE(app_, guild_id_, code_, json_body_) \
    dcc_app_modify_guild_template((app_), (guild_id_), (code_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_TEMPLATE_PARAMS(app_, params_) \
    dcc_app_modify_guild_template_params((app_), (params_), NULL, NULL)
#define DCC_APP_SYNC_GUILD_TEMPLATE(app_, guild_id_, code_) \
    dcc_app_sync_guild_template((app_), (guild_id_), (code_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_TEMPLATE(app_, guild_id_, code_) \
    dcc_app_delete_guild_template((app_), (guild_id_), (code_), NULL, NULL)

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_APPLICATION_H */
