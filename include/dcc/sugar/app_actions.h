#ifndef DCC_SUGAR_APP_ACTIONS_H
#define DCC_SUGAR_APP_ACTIONS_H

#include <dcc/app.h>
#include <dcc/sugar/messages.h>
#include <dcc/sugar/status.h>

#define DCC_APP_SEND(app_, channel_id_, message_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], NULL, NULL)
#define DCC_APP_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    dcc_app_send_with_id((app_), (channel_id_), &((dcc_message_builder_t[]){ (message_) })[0], (cb_), (user_data_))
#define DCC_APP_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT(app_, channel_id_, content_) \
    dcc_app_send_text((app_), (channel_id_), (content_), NULL, NULL)
#define DCC_APP_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    dcc_app_send_text_with_id((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_APP_SEND_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_V2_CB(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_CB((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), (cb_), (user_data_))
#define DCC_APP_SEND_V2_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_EPHEMERAL_V2(app_, channel_id_, ...) \
    DCC_APP_SEND((app_), (channel_id_), DCC_MESSAGE_EPHEMERAL_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_SEND_UI(app_, channel_id_, ...) DCC_APP_SEND_V2((app_), (channel_id_), __VA_ARGS__)
#define DCC_APP_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_V2_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_APP_SEND_EPHEMERAL_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_EPHEMERAL_V2((app_), (channel_id_), __VA_ARGS__)

#define DCC_APP_GET_CHANNEL(app_, channel_id_) \
    dcc_app_get_channel((app_), (channel_id_), NULL, NULL)
#define DCC_APP_GET_CHANNEL_CB(app_, channel_id_, cb_, user_data_) \
    dcc_app_get_channel((app_), (channel_id_), (cb_), (user_data_))
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

#define DCC_APP_EDIT_MESSAGE(app_, channel_id_, message_id_, message_) \
    dcc_app_edit_message( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        NULL, \
        NULL \
    )
#define DCC_APP_EDIT_MESSAGE_CB(app_, channel_id_, message_id_, message_, cb_, user_data_) \
    dcc_app_edit_message( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        &((dcc_message_builder_t[]){ (message_) })[0], \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_EDIT_V2(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_MESSAGE((app_), (channel_id_), (message_id_), DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__))
#define DCC_APP_EDIT_UI(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_V2((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_APP_EDIT_V2_CB(app_, channel_id_, message_id_, cb_, user_data_, ...) \
    DCC_APP_EDIT_MESSAGE_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        DCC_MESSAGE_COMPONENTS_V2(__VA_ARGS__), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_EDIT_TEXT(app_, channel_id_, message_id_, content_) \
    dcc_app_edit_message_text((app_), (channel_id_), (message_id_), (content_), NULL, NULL)
#define DCC_APP_EDIT_TEXT_CB(app_, channel_id_, message_id_, content_, cb_, user_data_) \
    dcc_app_edit_message_text((app_), (channel_id_), (message_id_), (content_), (cb_), (user_data_))
#define DCC_APP_DELETE_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_delete_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_DELETE_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_delete_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_CROSSPOST_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_crosspost_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_CROSSPOST_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_crosspost_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_PIN_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_pin_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_PIN_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_pin_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_UNPIN_MESSAGE(app_, channel_id_, message_id_) \
    dcc_app_unpin_message((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_UNPIN_MESSAGE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_unpin_message((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_ADD_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_) \
    dcc_app_add_message_reaction((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_ADD_MESSAGE_REACTION_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_add_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_APP_DELETE_OWN_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_) \
    dcc_app_delete_own_message_reaction((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_DELETE_OWN_MESSAGE_REACTION_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_delete_own_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_APP_DELETE_USER_MESSAGE_REACTION(app_, channel_id_, message_id_, reaction_, user_id_) \
    dcc_app_delete_user_message_reaction((app_), (channel_id_), (message_id_), (reaction_), (user_id_), NULL, NULL)
#define DCC_APP_DELETE_USER_MESSAGE_REACTION_CB( \
    app_, \
    channel_id_, \
    message_id_, \
    reaction_, \
    user_id_, \
    cb_, \
    user_data_ \
) \
    dcc_app_delete_user_message_reaction( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (user_id_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_CLEAR_MESSAGE_REACTIONS(app_, channel_id_, message_id_) \
    dcc_app_delete_all_message_reactions((app_), (channel_id_), (message_id_), NULL, NULL)
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    dcc_app_delete_all_message_reactions((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI(app_, channel_id_, message_id_, reaction_) \
    dcc_app_delete_all_message_reactions_for_emoji((app_), (channel_id_), (message_id_), (reaction_), NULL, NULL)
#define DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    dcc_app_delete_all_message_reactions_for_emoji( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (cb_), \
        (user_data_) \
    )

#define DCC_APP_THREAD_FROM_MESSAGE(app_, channel_id_, message_id_, name_) \
    dcc_app_create_thread_from_message_name((app_), (channel_id_), (message_id_), (name_), NULL, NULL)
#define DCC_APP_THREAD_FROM_MESSAGE_CB(app_, channel_id_, message_id_, name_, cb_, user_data_) \
    dcc_app_create_thread_from_message_name( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (name_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_APP_THREAD_FROM_MESSAGE_PARAMS(app_, channel_id_, message_id_, params_) \
    dcc_app_create_thread_from_message((app_), (channel_id_), (message_id_), (params_), NULL, NULL)
#define DCC_APP_THREAD_FROM_MESSAGE_PARAMS_CB(app_, channel_id_, message_id_, params_, cb_, user_data_) \
    dcc_app_create_thread_from_message((app_), (channel_id_), (message_id_), (params_), (cb_), (user_data_))
#define DCC_APP_ARCHIVE_THREAD(app_, thread_id_) \
    dcc_app_archive_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_ARCHIVE_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_archive_thread((app_), (thread_id_), (cb_), (user_data_))
#define DCC_APP_LOCK_THREAD(app_, thread_id_) \
    dcc_app_lock_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_LOCK_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_lock_thread((app_), (thread_id_), (cb_), (user_data_))
#define DCC_APP_UNLOCK_THREAD(app_, thread_id_) \
    dcc_app_unlock_thread((app_), (thread_id_), NULL, NULL)
#define DCC_APP_UNLOCK_THREAD_CB(app_, thread_id_, cb_, user_data_) \
    dcc_app_unlock_thread((app_), (thread_id_), (cb_), (user_data_))

#define DCC_CHANNEL_SEND(app_, channel_id_, message_) \
    DCC_APP_SEND((app_), (channel_id_), (message_))
#define DCC_CHANNEL_SEND_CB(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_CB((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_WITH_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_APP_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_ID(app_, channel_id_, message_, cb_, user_data_) \
    DCC_CHANNEL_SEND_WITH_ID((app_), (channel_id_), (message_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT(app_, channel_id_, content_) \
    DCC_APP_SEND_TEXT((app_), (channel_id_), (content_))
#define DCC_CHANNEL_SEND_TEXT_CB(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_CB((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_WITH_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_APP_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
#define DCC_CHANNEL_SEND_TEXT_ID(app_, channel_id_, content_, cb_, user_data_) \
    DCC_CHANNEL_SEND_TEXT_WITH_ID((app_), (channel_id_), (content_), (cb_), (user_data_))
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
#define DCC_CHANNEL_SEND_UI(app_, channel_id_, ...) \
    DCC_APP_SEND_UI((app_), (channel_id_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_WITH_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_APP_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
#define DCC_CHANNEL_SEND_UI_ID(app_, channel_id_, cb_, user_data_, ...) \
    DCC_CHANNEL_SEND_UI_WITH_ID((app_), (channel_id_), (cb_), (user_data_), __VA_ARGS__)
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

#define DCC_MESSAGE_EDIT(app_, channel_id_, message_id_, message_) \
    DCC_APP_EDIT_MESSAGE((app_), (channel_id_), (message_id_), (message_))
#define DCC_MESSAGE_EDIT_CB(app_, channel_id_, message_id_, message_, cb_, user_data_) \
    DCC_APP_EDIT_MESSAGE_CB((app_), (channel_id_), (message_id_), (message_), (cb_), (user_data_))
#define DCC_MESSAGE_EDIT_TEXT(app_, channel_id_, message_id_, content_) \
    DCC_APP_EDIT_TEXT((app_), (channel_id_), (message_id_), (content_))
#define DCC_MESSAGE_EDIT_TEXT_CB(app_, channel_id_, message_id_, content_, cb_, user_data_) \
    DCC_APP_EDIT_TEXT_CB((app_), (channel_id_), (message_id_), (content_), (cb_), (user_data_))
#define DCC_MESSAGE_EDIT_V2(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_V2((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_EDIT_UI(app_, channel_id_, message_id_, ...) \
    DCC_APP_EDIT_UI((app_), (channel_id_), (message_id_), __VA_ARGS__)
#define DCC_MESSAGE_DELETE(app_, channel_id_, message_id_) \
    DCC_APP_DELETE_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_DELETE_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_DELETE_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_CROSSPOST(app_, channel_id_, message_id_) \
    DCC_APP_CROSSPOST_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_CROSSPOST_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_CROSSPOST_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_PIN(app_, channel_id_, message_id_) \
    DCC_APP_PIN_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_PIN_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_PIN_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_UNPIN(app_, channel_id_, message_id_) \
    DCC_APP_UNPIN_MESSAGE((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_UNPIN_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_UNPIN_MESSAGE_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_REACT(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_ADD_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_REACT_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_ADD_MESSAGE_REACTION_CB((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_MESSAGE_UNREACT(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_DELETE_OWN_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_UNREACT_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_DELETE_OWN_MESSAGE_REACTION_CB((app_), (channel_id_), (message_id_), (reaction_), (cb_), (user_data_))
#define DCC_MESSAGE_REMOVE_REACTION(app_, channel_id_, message_id_, reaction_, user_id_) \
    DCC_APP_DELETE_USER_MESSAGE_REACTION((app_), (channel_id_), (message_id_), (reaction_), (user_id_))
#define DCC_MESSAGE_REMOVE_REACTION_CB(app_, channel_id_, message_id_, reaction_, user_id_, cb_, user_data_) \
    DCC_APP_DELETE_USER_MESSAGE_REACTION_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (user_id_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_MESSAGE_CLEAR_REACTIONS(app_, channel_id_, message_id_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS((app_), (channel_id_), (message_id_))
#define DCC_MESSAGE_CLEAR_REACTIONS_CB(app_, channel_id_, message_id_, cb_, user_data_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_CB((app_), (channel_id_), (message_id_), (cb_), (user_data_))
#define DCC_MESSAGE_CLEAR_REACTION_EMOJI(app_, channel_id_, message_id_, reaction_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI((app_), (channel_id_), (message_id_), (reaction_))
#define DCC_MESSAGE_CLEAR_REACTION_EMOJI_CB(app_, channel_id_, message_id_, reaction_, cb_, user_data_) \
    DCC_APP_CLEAR_MESSAGE_REACTIONS_FOR_EMOJI_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (reaction_), \
        (cb_), \
        (user_data_) \
    )

#define DCC_MESSAGE_THREAD(app_, channel_id_, message_id_, name_) \
    DCC_APP_THREAD_FROM_MESSAGE((app_), (channel_id_), (message_id_), (name_))
#define DCC_MESSAGE_THREAD_CB(app_, channel_id_, message_id_, name_, cb_, user_data_) \
    DCC_APP_THREAD_FROM_MESSAGE_CB((app_), (channel_id_), (message_id_), (name_), (cb_), (user_data_))
#define DCC_MESSAGE_THREAD_PARAMS(app_, channel_id_, message_id_, params_) \
    DCC_APP_THREAD_FROM_MESSAGE_PARAMS((app_), (channel_id_), (message_id_), (params_))
#define DCC_MESSAGE_THREAD_PARAMS_CB(app_, channel_id_, message_id_, params_, cb_, user_data_) \
    DCC_APP_THREAD_FROM_MESSAGE_PARAMS_CB( \
        (app_), \
        (channel_id_), \
        (message_id_), \
        (params_), \
        (cb_), \
        (user_data_) \
    )
#define DCC_THREAD_FROM_MESSAGE(app_, channel_id_, message_id_, name_) \
    DCC_MESSAGE_THREAD((app_), (channel_id_), (message_id_), (name_))
#define DCC_THREAD_FROM_MESSAGE_PARAMS(app_, channel_id_, message_id_, params_) \
    DCC_MESSAGE_THREAD_PARAMS((app_), (channel_id_), (message_id_), (params_))
#define DCC_THREAD_ARCHIVE(app_, thread_id_) \
    DCC_APP_ARCHIVE_THREAD((app_), (thread_id_))
#define DCC_THREAD_ARCHIVE_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_ARCHIVE_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))
#define DCC_THREAD_LOCK(app_, thread_id_) \
    DCC_APP_LOCK_THREAD((app_), (thread_id_))
#define DCC_THREAD_LOCK_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_LOCK_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))
#define DCC_THREAD_UNLOCK(app_, thread_id_) \
    DCC_APP_UNLOCK_THREAD((app_), (thread_id_))
#define DCC_THREAD_UNLOCK_CB(app_, thread_id_, cb_, user_data_) \
    DCC_APP_UNLOCK_THREAD_CB((app_), (thread_id_), (cb_), (user_data_))

#define DCC_APP_ADD_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_ADD_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_add_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_APP_REMOVE_MEMBER_ROLE(app_, guild_id_, user_id_, role_id_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), NULL, NULL)
#define DCC_APP_REMOVE_MEMBER_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    dcc_app_remove_member_role((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_ADD_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_ADD_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_APP_REMOVE_MEMBER_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_APP_REMOVE_MEMBER_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_ADD_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_ADD_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_ADD_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_ADD_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE(app_, guild_id_, user_id_, role_id_) \
    DCC_MEMBER_REMOVE_ROLE((app_), (guild_id_), (user_id_), (role_id_))
#define DCC_GUILD_MEMBER_REMOVE_ROLE_CB(app_, guild_id_, user_id_, role_id_, cb_, user_data_) \
    DCC_MEMBER_REMOVE_ROLE_CB((app_), (guild_id_), (user_id_), (role_id_), (cb_), (user_data_))
#define DCC_MEMBER_FETCH(app_, guild_id_, user_id_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_get_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), NULL, NULL)
#define DCC_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    dcc_app_set_member_timeout((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_clear_member_timeout((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), NULL, NULL)
#define DCC_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    dcc_app_move_member((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_MEMBER_KICK(app_, guild_id_, user_id_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), NULL, NULL)
#define DCC_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    dcc_app_remove_member((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_FETCH(app_, guild_id_, user_id_) \
    DCC_MEMBER_FETCH((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_FETCH_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_FETCH_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_TIMEOUT(app_, guild_id_, user_id_, until_) \
    DCC_MEMBER_TIMEOUT((app_), (guild_id_), (user_id_), (until_))
#define DCC_GUILD_MEMBER_TIMEOUT_CB(app_, guild_id_, user_id_, until_, cb_, user_data_) \
    DCC_MEMBER_TIMEOUT_CB((app_), (guild_id_), (user_id_), (until_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT(app_, guild_id_, user_id_) \
    DCC_MEMBER_CLEAR_TIMEOUT((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_CLEAR_TIMEOUT_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_CLEAR_TIMEOUT_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_MOVE(app_, guild_id_, user_id_, channel_id_) \
    DCC_MEMBER_MOVE((app_), (guild_id_), (user_id_), (channel_id_))
#define DCC_GUILD_MEMBER_MOVE_CB(app_, guild_id_, user_id_, channel_id_, cb_, user_data_) \
    DCC_MEMBER_MOVE_CB((app_), (guild_id_), (user_id_), (channel_id_), (cb_), (user_data_))
#define DCC_GUILD_MEMBER_KICK(app_, guild_id_, user_id_) \
    DCC_MEMBER_KICK((app_), (guild_id_), (user_id_))
#define DCC_GUILD_MEMBER_KICK_CB(app_, guild_id_, user_id_, cb_, user_data_) \
    DCC_MEMBER_KICK_CB((app_), (guild_id_), (user_id_), (cb_), (user_data_))

#endif
