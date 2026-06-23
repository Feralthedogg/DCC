#ifndef DCC_SUGAR_COMMAND_ROUTES_MESSAGE_ALIASES_H
#define DCC_SUGAR_COMMAND_ROUTES_MESSAGE_ALIASES_H

#include <dcc/sugar/command_routes/app_messages.h>
#include <dcc/sugar/command_routes/channel_aliases.h>

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

#endif /* DCC_SUGAR_COMMAND_ROUTES_MESSAGE_ALIASES_H */
