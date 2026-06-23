#ifndef DCC_SUGAR_COMMAND_ROUTES_CHANNEL_ALIASES_H
#define DCC_SUGAR_COMMAND_ROUTES_CHANNEL_ALIASES_H

#include <dcc/sugar/command_routes/app_messages.h>

#define DCC_CHANNEL_SEND(app_, channel_id_, message_) \
    DCC_APP_SEND((app_), (channel_id_), (message_))
#define DCC_CHANNEL_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_CB((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_CHANNEL_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_THREAD(app_, channel_id_, message_, thread_name_) \
    DCC_APP_SEND_THREAD((app_), (channel_id_), (message_), (thread_name_))
#define DCC_CHANNEL_SEND_THREAD_CB(app_, channel_id_, message_, thread_name_, cb_, user_data_) \
    DCC_APP_SEND_THREAD_CB((app_), (channel_id_), (message_), (thread_name_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_THREAD_PARAMS(app_, channel_id_, message_, thread_, cb_, user_data_) \
    DCC_APP_SEND_THREAD_PARAMS((app_), (channel_id_), (message_), (thread_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT(app_, channel_id_, content_) \
    DCC_APP_SEND_TEXT((app_), (channel_id_), (content_))
#define DCC_CHANNEL_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_CB((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_CHANNEL_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_THREAD(app_, channel_id_, content_, thread_name_) \
    DCC_APP_SEND_TEXT_THREAD((app_), (channel_id_), (content_), (thread_name_))
#define DCC_CHANNEL_SEND_TEXT_THREAD_CB(app_, channel_id_, content_, thread_name_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_THREAD_CB((app_), (channel_id_), (content_), (thread_name_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_SAFE(app_, channel_id_, content_) \
    DCC_APP_SEND_SAFE((app_), (channel_id_), (content_))
#define DCC_CHANNEL_SEND_SAFE_CB(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_SAFE_CB((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_V2(app_, channel_id_, ...) \
    DCC_APP_SEND_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_CHANNEL_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_V2_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_V2_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_UI((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_CHANNEL_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_THREAD(app_, channel_id_, thread_name_, ...) \
    DCC_APP_SEND_UI_THREAD((app_), (channel_id_), (thread_name_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_THREAD_CB(app_, channel_id_, thread_name_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_THREAD_CB((app_), (channel_id_), (thread_name_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL((app_), (channel_id_))
#define DCC_CHANNEL_FETCH_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_GET_CHANNEL_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_GET(app_, channel_id_) \
    DCC_CHANNEL_FETCH((app_), (channel_id_))
#define DCC_CHANNEL_GET_CB(app_, channel_id_, cb_, user_data_) \
    DCC_CHANNEL_FETCH_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_EDIT(app_, channel_id_, json_body_) \
    DCC_APP_EDIT_CHANNEL((app_), (channel_id_), (json_body_))
#define DCC_CHANNEL_EDIT_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_CB((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_CHANNEL_PARAMS((app_), (params_))
#define DCC_CHANNEL_EDIT_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_CHANNEL_DELETE(app_, channel_id_) \
    DCC_APP_DELETE_CHANNEL((app_), (channel_id_))
#define DCC_CHANNEL_DELETE_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_DELETE_CHANNEL_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_TYPING(app_, channel_id_) \
    DCC_APP_TRIGGER_CHANNEL_TYPING((app_), (channel_id_))
#define DCC_CHANNEL_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_TRIGGER_CHANNEL_TYPING_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_TRIGGER_TYPING(app_, channel_id_) \
    DCC_CHANNEL_TYPING((app_), (channel_id_))
#define DCC_CHANNEL_TRIGGER_TYPING_CB(app_, channel_id_, cb_, user_data_) \
    DCC_CHANNEL_TYPING_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_FOLLOW(app_, channel_id_, webhook_channel_id_) \
    DCC_APP_FOLLOW_NEWS_CHANNEL((app_), (channel_id_), (webhook_channel_id_))
#define DCC_CHANNEL_FOLLOW_CB(app_, channel_id_, webhook_channel_id_, cb_, user_data_) \
    DCC_APP_FOLLOW_NEWS_CHANNEL_CB((app_), (channel_id_), (webhook_channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_SET_VOICE_STATUS(app_, channel_id_, status_) \
    DCC_APP_SET_CHANNEL_VOICE_STATUS((app_), (channel_id_), (status_))
#define DCC_CHANNEL_SET_VOICE_STATUS_CB(app_, channel_id_, status_, cb_, user_data_) \
    DCC_APP_SET_CHANNEL_VOICE_STATUS_CB((app_), (channel_id_), (status_), (cb_), (user_data_))
#define DCC_MY_VOICE_STATE_FETCH(app_, guild_id_) \
    DCC_APP_GET_MY_VOICE_STATE((app_), (guild_id_))
#define DCC_MY_VOICE_STATE_EDIT(app_, guild_id_, json_body_) \
    DCC_APP_EDIT_MY_VOICE_STATE((app_), (guild_id_), (json_body_))
#define DCC_MY_VOICE_STATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_MY_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_USER_VOICE_STATE_FETCH(app_, guild_id_, user_id_) \
    DCC_APP_GET_USER_VOICE_STATE((app_), (guild_id_), (user_id_))
#define DCC_USER_VOICE_STATE_EDIT(app_, guild_id_, user_id_, json_body_) \
    DCC_APP_EDIT_USER_VOICE_STATE((app_), (guild_id_), (user_id_), (json_body_))
#define DCC_USER_VOICE_STATE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_USER_VOICE_STATE_PARAMS((app_), (params_))
#define DCC_VOICE_REGIONS_FETCH(app_) \
    DCC_APP_GET_VOICE_REGIONS((app_))
#define DCC_GUILD_VOICE_REGIONS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_VOICE_REGIONS((app_), (guild_id_))
#define DCC_CHANNEL_INVITES_FETCH(app_, channel_id_) \
    DCC_APP_GET_CHANNEL_INVITES((app_), (channel_id_))
#define DCC_CHANNEL_INVITES_FETCH_CB(app_, channel_id_, cb_, user_data_) \
    DCC_APP_GET_CHANNEL_INVITES_CB((app_), (channel_id_), (cb_), (user_data_))
#define DCC_CHANNEL_INVITE_CREATE(app_, channel_id_, json_body_) \
    DCC_APP_CREATE_CHANNEL_INVITE((app_), (channel_id_), (json_body_))
#define DCC_CHANNEL_INVITE_CREATE_CB(app_, channel_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_CHANNEL_INVITE_CB((app_), (channel_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_INVITE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_CHANNEL_INVITE_PARAMS((app_), (params_))
#define DCC_CHANNEL_INVITE_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_CHANNEL_INVITE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_EDIT(app_, channel_id_, overwrite_id_, json_body_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION((app_), (channel_id_), (overwrite_id_), (json_body_))
#define DCC_CHANNEL_PERMISSION_EDIT_CB(app_, channel_id_, overwrite_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_CB((app_), (channel_id_), (overwrite_id_), (json_body_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_EDIT_PARAMS(app_, channel_id_, overwrite_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS((app_), (channel_id_), (overwrite_))
#define DCC_CHANNEL_PERMISSION_EDIT_PARAMS_CB(app_, channel_id_, overwrite_, cb_, user_data_) \
    DCC_APP_EDIT_CHANNEL_PERMISSION_PARAMS_CB((app_), (channel_id_), (overwrite_), (cb_), (user_data_))
#define DCC_CHANNEL_PERMISSION_DELETE(app_, channel_id_, overwrite_id_) \
    DCC_APP_DELETE_CHANNEL_PERMISSION((app_), (channel_id_), (overwrite_id_))
#define DCC_CHANNEL_PERMISSION_DELETE_CB(app_, channel_id_, overwrite_id_, cb_, user_data_) \
    DCC_APP_DELETE_CHANNEL_PERMISSION_CB((app_), (channel_id_), (overwrite_id_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_REORDER(app_, guild_id_, json_body_) \
    DCC_APP_REORDER_GUILD_CHANNELS((app_), (guild_id_), (json_body_))
#define DCC_GUILD_CHANNELS_REORDER_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_CHANNELS_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_REORDER_PARAMS(app_, params_) \
    DCC_APP_REORDER_GUILD_CHANNELS_PARAMS((app_), (params_))
#define DCC_GUILD_CHANNELS_REORDER_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_CHANNELS_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_GUILD_CHANNELS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_CHANNELS((app_), (guild_id_))
#define DCC_GUILD_CHANNELS_FETCH_CB(app_, guild_id_, cb_, user_data_) \
    DCC_APP_GET_GUILD_CHANNELS_CB((app_), (guild_id_), (cb_), (user_data_))
#define DCC_GUILD_CHANNEL_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_CHANNEL((app_), (guild_id_), (json_body_))
#define DCC_GUILD_CHANNEL_CREATE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_CHANNEL_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_CHANNEL_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_CHANNEL_PARAMS((app_), (params_))
#define DCC_GUILD_CHANNEL_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_CHANNEL_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_GUILD_ROLES_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_ROLES((app_), (guild_id_))
#define DCC_GUILD_ROLES_FETCH_CB(app_, guild_id_, cb_, user_data_) \
    DCC_APP_GET_GUILD_ROLES_CB((app_), (guild_id_), (cb_), (user_data_))
#define DCC_GUILD_ROLES_REORDER(app_, guild_id_, positions_, position_count_) \
    DCC_APP_REORDER_GUILD_ROLES((app_), (guild_id_), (positions_), (position_count_))
#define DCC_GUILD_ROLES_REORDER_CB(app_, guild_id_, positions_, position_count_, cb_, user_data_) \
    DCC_APP_REORDER_GUILD_ROLES_CB((app_), (guild_id_), (positions_), (position_count_), (cb_), (user_data_))
#define DCC_GUILD_ROLE_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_ROLE((app_), (guild_id_), (json_body_))
#define DCC_GUILD_ROLE_CREATE_CB(app_, guild_id_, json_body_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_ROLE_CB((app_), (guild_id_), (json_body_), (cb_), (user_data_))
#define DCC_GUILD_ROLE_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_ROLE_PARAMS((app_), (params_))
#define DCC_GUILD_ROLE_CREATE_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_CREATE_GUILD_ROLE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_ROLE_EDIT(app_, guild_id_, role_id_, json_body_) \
    DCC_APP_EDIT_GUILD_ROLE((app_), (guild_id_), (role_id_), (json_body_))
#define DCC_ROLE_EDIT_CB(app_, guild_id_, role_id_, json_body_, cb_, user_data_) \
    DCC_APP_EDIT_GUILD_ROLE_CB((app_), (guild_id_), (role_id_), (json_body_), (cb_), (user_data_))
#define DCC_ROLE_EDIT_PARAMS(app_, params_) \
    DCC_APP_EDIT_GUILD_ROLE_PARAMS((app_), (params_))
#define DCC_ROLE_EDIT_PARAMS_CB(app_, params_, cb_, user_data_) \
    DCC_APP_EDIT_GUILD_ROLE_PARAMS_CB((app_), (params_), (cb_), (user_data_))
#define DCC_ROLE_DELETE(app_, guild_id_, role_id_) \
    DCC_APP_DELETE_GUILD_ROLE((app_), (guild_id_), (role_id_))
#define DCC_ROLE_DELETE_CB(app_, guild_id_, role_id_, cb_, user_data_) \
    DCC_APP_DELETE_GUILD_ROLE_CB((app_), (guild_id_), (role_id_), (cb_), (user_data_))


#endif /* DCC_SUGAR_COMMAND_ROUTES_CHANNEL_ALIASES_H */
