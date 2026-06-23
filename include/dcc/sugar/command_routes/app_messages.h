#ifndef DCC_SUGAR_COMMAND_ROUTES_APP_MESSAGES_H
#define DCC_SUGAR_COMMAND_ROUTES_APP_MESSAGES_H

#include <dcc/sugar/command_routes/guild_roles.h>

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


#endif /* DCC_SUGAR_COMMAND_ROUTES_APP_MESSAGES_H */
