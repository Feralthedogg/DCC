#ifndef DCC_SUGAR_COMMAND_ROUTES_CHANNELS_H
#define DCC_SUGAR_COMMAND_ROUTES_CHANNELS_H

#include <dcc/sugar/command_routes/send.h>

#define DCC_APP_GET_CHANNEL(app_, channel_id_) \
    dcc_app_get_channel((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_CHANNEL_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_get_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_GET_GUILD_CHANNELS(app_, guild_id_) \
    dcc_app_get_guild_channels((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_CHANNELS_CB(app_, guild_id_, cb_, user_data_) \
    dcc_app_get_guild_channels((app_), (guild_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_CHANNEL(app_, guild_id_, json_body_) \
    dcc_app_create_guild_channel((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_CHANNEL_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_create_guild_channel((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_GUILD_CHANNEL_PARAMS(app_, params_) \
    dcc_app_create_guild_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_CHANNEL_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_guild_channel_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL(app_, channel_id_, json_body_) \
    dcc_app_modify_channel((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_channel((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PARAMS(app_, params_) \
    dcc_app_modify_channel_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_channel_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_DELETE_CHANNEL(app_, channel_id_) \
    dcc_app_delete_channel((app_), (channel_id_), NULL, NULL)
#define DCC_APP_DELETE_CHANNEL_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_delete_channel((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_TRIGGER_CHANNEL_TYPING(app_, channel_id_) \
    dcc_app_trigger_channel_typing((app_), (channel_id_), NULL, NULL)
#define DCC_APP_TRIGGER_CHANNEL_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_trigger_channel_typing((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_FOLLOW_NEWS_CHANNEL(app_, channel_id_, webhook_channel_id_) \
    dcc_app_follow_news_channel((app_), (channel_id_), (webhook_channel_id_), NULL, NULL)
#define DCC_APP_FOLLOW_NEWS_CHANNEL_CB(app_, channel_id_, webhook_channel_id_, cb_, user_data_) \
    dcc_app_follow_news_channel((app_), (channel_id_), (webhook_channel_id_), (cb_), (user_data_))
#define DCC_APP_SET_CHANNEL_VOICE_STATUS(app_, channel_id_, status_) \
    dcc_app_set_channel_voice_status((app_), (channel_id_), (status_), NULL, NULL)
#define DCC_APP_SET_CHANNEL_VOICE_STATUS_CB(app_, channel_id_, status_, cb_, user_data_) \
    dcc_app_set_channel_voice_status((app_), (channel_id_), (status_), (cb_), (user_data_))
#define DCC_APP_GET_CURRENT_USER_VOICE_STATE(app_, guild_id_) \
    dcc_app_get_current_user_voice_state((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_MY_VOICE_STATE(app_, guild_id_) \
    DCC_APP_GET_CURRENT_USER_VOICE_STATE((app_), (guild_id_))
#define DCC_APP_EDIT_CURRENT_USER_VOICE_STATE(app_, guild_id_, json_body_) \
    dcc_app_modify_current_user_voice_state((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_MY_VOICE_STATE(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_CURRENT_USER_VOICE_STATE((app_), (guild_id_), (json_body_))
#define DCC_APP_EDIT_CURRENT_USER_VOICE_STATE_PARAMS(app_, params_) \
    dcc_app_modify_current_user_voice_state_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_MY_VOICE_STATE_PARAMS(app_, params_) \
    DCC_APP_EDIT_CURRENT_USER_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_APP_GET_USER_VOICE_STATE(app_, guild_id_, user_id_) \
    dcc_app_get_user_voice_state((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_APP_EDIT_USER_VOICE_STATE(app_, guild_id_, user_id_, json_body_) \
    dcc_app_modify_user_voice_state((app_), (guild_id_), (user_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_USER_VOICE_STATE_PARAMS(app_, params_) \
    dcc_app_modify_user_voice_state_params((app_), (params_), NULL, NULL)
#define DCC_APP_GET_VOICE_REGIONS(app_) \
    dcc_app_get_voice_regions((app_), NULL, NULL)
#define DCC_APP_GET_GUILD_VOICE_REGIONS(app_, guild_id_) \
    dcc_app_get_guild_voice_regions((app_), (guild_id_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS(app_, guild_id_, json_body_) \
    dcc_app_modify_guild_channel_positions((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_guild_channel_positions((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_REORDER_GUILD_CHANNELS_PARAMS(app_, params_) \
    dcc_app_modify_guild_channel_positions_params((app_), (params_), NULL, NULL)
#define DCC_APP_REORDER_GUILD_CHANNELS_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_modify_guild_channel_positions_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_GET_CHANNEL_INVITES(app_, channel_id_) \
    dcc_app_get_channel_invites((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_CHANNEL_INVITES_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_get_channel_invites((app_), (channel_id_), (cb_), (user_data_))
#define DCC_APP_CREATE_CHANNEL_INVITE(app_, channel_id_, json_body_) \
    dcc_app_create_channel_invite((app_), (channel_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_CHANNEL_INVITE_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    dcc_app_create_channel_invite((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_CREATE_CHANNEL_INVITE_PARAMS(app_, params_) \
    dcc_app_create_channel_invite_params((app_), (params_), NULL, NULL)
#define DCC_APP_CREATE_CHANNEL_INVITE_PARAMS_CB(app_, params_, cb_, user_data_) \
    dcc_app_create_channel_invite_params((app_), (params_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PERMISSION(app_, channel_id_, overwrite_id_, json_body_) \
    dcc_app_modify_channel_permission((app_), (channel_id_), (overwrite_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PERMISSION_CB(app_, channel_id_, overwrite_id_, json_body_, cb_, user_data_) \
    dcc_app_modify_channel_permission((app_), (channel_id_), (overwrite_id_), (json_body_), (cb_), (user_data_))
#define DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS(app_, channel_id_, overwrite_) \
    dcc_app_modify_channel_permission_params((app_), (channel_id_), (overwrite_), NULL, NULL)
#define DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS_CB(app_, channel_id_, overwrite_, cb_, user_data_) \
    dcc_app_modify_channel_permission_params((app_), (channel_id_), (overwrite_), (cb_), (user_data_))
#define DCC_APP_DELETE_CHANNEL_PERMISSION(app_, channel_id_, overwrite_id_) \
    dcc_app_delete_channel_permission((app_), (channel_id_), (overwrite_id_), NULL, NULL)
#define DCC_APP_DELETE_CHANNEL_PERMISSION_CB(app_, channel_id_, overwrite_id_, cb_, user_data_) \
    dcc_app_delete_channel_permission((app_), (channel_id_), (overwrite_id_), (cb_), (user_data_))

#endif /* DCC_SUGAR_COMMAND_ROUTES_CHANNELS_H */
