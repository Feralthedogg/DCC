#ifndef DCC_SUGAR_MESSAGE_COMPONENT_UI_APP_SHORTCUTS_H
#define DCC_SUGAR_MESSAGE_COMPONENT_UI_APP_SHORTCUTS_H

#include <dcc/sugar/message_component_ui/base.h>

/* Included by <dcc/sugar.h>. */
#define DCC_ME_EDIT_USER_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_USER_PARAMS((app_), (params_))
#define DCC_ME_CONNECTIONS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_CONNECTIONS((app_))
#define DCC_ME_GUILDS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_GUILDS((app_))
#define DCC_ME_GUILDS_FETCH_QUERY(app_, query_) \
    DCC_APP_GET_CURRENT_USER_GUILDS_QUERY((app_), (query_))
#define DCC_ME_DMS_FETCH(app_) \
    DCC_APP_GET_CURRENT_USER_DMS((app_))
#define DCC_USER_FETCH(app_, user_id_) \
    DCC_APP_GET_USER((app_), (user_id_))
#define DCC_USER_DM_CHANNEL_CREATE(app_, user_id_) \
    DCC_APP_CREATE_DM_CHANNEL_PARAMS( \
        (app_), \
        &((dcc_dm_channel_params_t[]){ { \
            .size = sizeof(dcc_dm_channel_params_t), \
            .user_id = (user_id_) \
        } })[0] \
    )
#define DCC_GROUP_DM_RECIPIENT_ADD(app_, channel_id_, user_id_, json_body_) \
    DCC_APP_ADD_GROUP_DM_RECIPIENT((app_), (channel_id_), (user_id_), (json_body_))
#define DCC_GROUP_DM_RECIPIENT_ADD_PARAMS(app_, params_) \
    DCC_APP_ADD_GROUP_DM_RECIPIENT_PARAMS((app_), (params_))
#define DCC_GROUP_DM_RECIPIENT_REMOVE(app_, channel_id_, user_id_) \
    DCC_APP_REMOVE_GROUP_DM_RECIPIENT((app_), (channel_id_), (user_id_))
#define DCC_USER_DM_SEND_JSON(app_, user_id_, json_body_) \
    DCC_APP_CREATE_DIRECT_MESSAGE((app_), (user_id_), (json_body_))
#define DCC_USER_DM_SEND(app_, user_id_, message_) \
    DCC_APP_CREATE_DIRECT_MESSAGE_BUILDER((app_), (user_id_), (message_))
#define DCC_USER_DM_SEND_TEXT(app_, user_id_, content_) \
    DCC_APP_CREATE_DIRECT_MESSAGE_TEXT((app_), (user_id_), (content_))
#define DCC_USER_DM_SEND_UI(app_, user_id_, ...) \
    DCC_USER_DM_SEND((app_), (user_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_DM_SEND(app_, user_id_, message_) \
    DCC_USER_DM_SEND((app_), (user_id_), (message_))
#define DCC_DM_SEND_TEXT(app_, user_id_, content_) \
    DCC_USER_DM_SEND_TEXT((app_), (user_id_), (content_))
#define DCC_DM_SEND_UI(app_, user_id_, ...) \
    DCC_USER_DM_SEND_UI((app_), (user_id_), __VA_ARGS__)
#define DCC_GUILD_LEAVE(app_, guild_id_) \
    DCC_APP_LEAVE_GUILD((app_), (guild_id_))
#define DCC_GUILD_INVITES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_INVITES((app_), (guild_id_))
#define DCC_INVITE_FETCH(app_, invite_code_) \
    DCC_APP_GET_INVITE((app_), (invite_code_))
#define DCC_INVITE_FETCH_QUERY(app_, invite_code_, query_) \
    DCC_APP_GET_INVITE_QUERY((app_), (invite_code_), (query_))
#define DCC_INVITE_FETCH_FULL(app_, invite_code_) \
    DCC_APP_GET_INVITE_FULL((app_), (invite_code_))
#define DCC_INVITE_DELETE(app_, invite_code_) \
    DCC_APP_DELETE_INVITE((app_), (invite_code_))
#define DCC_STAGE_CREATE(app_, json_body_) \
    DCC_APP_CREATE_STAGE((app_), (json_body_))
#define DCC_STAGE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_STAGE_PARAMS((app_), (params_))
#define DCC_STAGE_FETCH(app_, channel_id_) \
    DCC_APP_GET_STAGE((app_), (channel_id_))
#define DCC_STAGE_EDIT(app_, channel_id_, json_body_) \
    DCC_APP_EDIT_STAGE((app_), (channel_id_), (json_body_))
#define DCC_STAGE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_STAGE_PARAMS((app_), (params_))
#define DCC_STAGE_DELETE(app_, channel_id_) \
    DCC_APP_DELETE_STAGE((app_), (channel_id_))
#define DCC_TEMPLATE_FETCH(app_, code_) \
    DCC_APP_GET_TEMPLATE((app_), (code_))
#define DCC_TEMPLATE_CREATE_GUILD(app_, code_, json_body_) \
    DCC_APP_CREATE_GUILD_FROM_TEMPLATE((app_), (code_), (json_body_))
#define DCC_TEMPLATE_CREATE_GUILD_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_FROM_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_TEMPLATES((app_), (guild_id_))
#define DCC_GUILD_TEMPLATE_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_TEMPLATE((app_), (guild_id_), (json_body_))
#define DCC_GUILD_TEMPLATE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATE_EDIT(app_, guild_id_, code_, json_body_) \
    DCC_APP_EDIT_GUILD_TEMPLATE((app_), (guild_id_), (code_), (json_body_))
#define DCC_GUILD_TEMPLATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_TEMPLATE_PARAMS((app_), (params_))
#define DCC_GUILD_TEMPLATE_SYNC(app_, guild_id_, code_) \
    DCC_APP_SYNC_GUILD_TEMPLATE((app_), (guild_id_), (code_))
#define DCC_GUILD_TEMPLATE_DELETE(app_, guild_id_, code_) \
    DCC_APP_DELETE_GUILD_TEMPLATE((app_), (guild_id_), (code_))

#endif /* DCC_SUGAR_MESSAGE_COMPONENT_UI_APP_SHORTCUTS_H */
