#ifndef DCC_SUGAR_GUILD_SHORTCUTS_MEDIA_H
#define DCC_SUGAR_GUILD_SHORTCUTS_MEDIA_H

#include <dcc/sugar/guild_shortcuts/base.h>

#define DCC_APP_GET_GUILD_EMOJIS(app_, guild_id_) \
    dcc_app_get_guild_emojis((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_EMOJI(app_, guild_id_, emoji_id_) \
    dcc_app_get_guild_emoji((app_), (guild_id_), (emoji_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_EMOJI(app_, guild_id_, json_body_) \
    dcc_app_create_guild_emoji((app_), (guild_id_), (json_body_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_EMOJI_PARAMS(app_, guild_id_, params_) \
    dcc_app_create_guild_emoji_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_EMOJI(app_, guild_id_, emoji_id_, json_body_) \
    dcc_app_modify_guild_emoji((app_), (guild_id_), (emoji_id_), (json_body_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_EMOJI_PARAMS(app_, guild_id_, params_) \
    dcc_app_modify_guild_emoji_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_EMOJI(app_, guild_id_, emoji_id_) \
    dcc_app_delete_guild_emoji((app_), (guild_id_), (emoji_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_STICKERS(app_, guild_id_) \
    dcc_app_get_guild_stickers((app_), (guild_id_), NULL, NULL)
#define DCC_APP_GET_GUILD_STICKER(app_, guild_id_, sticker_id_) \
    dcc_app_get_guild_sticker((app_), (guild_id_), (sticker_id_), NULL, NULL)
#define DCC_APP_CREATE_GUILD_STICKER_MULTIPART(app_, guild_id_, fields_, field_count_, file_) \
    dcc_app_create_guild_sticker_multipart( \
        (app_), \
        (guild_id_), \
        (fields_), \
        (field_count_), \
        (file_), \
        NULL, \
        NULL \
    )
#define DCC_APP_CREATE_GUILD_STICKER_PARAMS(app_, params_) \
    dcc_app_create_guild_sticker_params((app_), (params_), NULL, NULL)
#define DCC_APP_EDIT_GUILD_STICKER(app_, guild_id_, sticker_id_, json_body_) \
    dcc_app_modify_guild_sticker((app_), (guild_id_), (sticker_id_), (json_body_), NULL, NULL)
#define DCC_APP_DELETE_GUILD_STICKER(app_, guild_id_, sticker_id_) \
    dcc_app_delete_guild_sticker((app_), (guild_id_), (sticker_id_), NULL, NULL)
#define DCC_APP_GET_STICKER(app_, sticker_id_) \
    dcc_app_get_sticker((app_), (sticker_id_), NULL, NULL)
#define DCC_APP_GET_STICKER_PACKS(app_) \
    dcc_app_get_sticker_packs((app_), NULL, NULL)
#define DCC_APP_GUILD_MESSAGE_SEARCH(app_, guild_id_, query_) \
    dcc_app_guild_message_search((app_), (guild_id_), (query_), NULL, NULL)
#define DCC_APP_GUILD_MESSAGE_SEARCH_PARAMS(app_, guild_id_, params_) \
    dcc_app_guild_message_search_params((app_), (guild_id_), (params_), NULL, NULL)
#define DCC_GUILD_EMOJIS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_EMOJIS((app_), (guild_id_))
#define DCC_GUILD_EMOJI_FETCH(app_, guild_id_, emoji_id_) \
    DCC_APP_GET_GUILD_EMOJI((app_), (guild_id_), (emoji_id_))
#define DCC_GUILD_EMOJI_CREATE(app_, guild_id_, json_body_) \
    DCC_APP_CREATE_GUILD_EMOJI((app_), (guild_id_), (json_body_))
#define DCC_GUILD_EMOJI_CREATE_PARAMS(app_, guild_id_, params_) \
    DCC_APP_CREATE_GUILD_EMOJI_PARAMS((app_), (guild_id_), (params_))
#define DCC_GUILD_EMOJI_EDIT(app_, guild_id_, emoji_id_, json_body_) \
    DCC_APP_EDIT_GUILD_EMOJI((app_), (guild_id_), (emoji_id_), (json_body_))
#define DCC_GUILD_EMOJI_EDIT_PARAMS(app_, guild_id_, params_) \
    DCC_APP_EDIT_GUILD_EMOJI_PARAMS((app_), (guild_id_), (params_))
#define DCC_GUILD_EMOJI_DELETE(app_, guild_id_, emoji_id_) \
    DCC_APP_DELETE_GUILD_EMOJI((app_), (guild_id_), (emoji_id_))
#define DCC_GUILD_STICKERS_FETCH(app_, guild_id_) \
    DCC_APP_GET_GUILD_STICKERS((app_), (guild_id_))
#define DCC_GUILD_STICKER_FETCH(app_, guild_id_, sticker_id_) \
    DCC_APP_GET_GUILD_STICKER((app_), (guild_id_), (sticker_id_))
#define DCC_GUILD_STICKER_CREATE_MULTIPART(app_, guild_id_, fields_, field_count_, file_) \
    DCC_APP_CREATE_GUILD_STICKER_MULTIPART((app_), (guild_id_), (fields_), (field_count_), (file_))
#define DCC_GUILD_STICKER_CREATE_PARAMS(app_, params_) \
    DCC_APP_CREATE_GUILD_STICKER_PARAMS((app_), (params_))
#define DCC_GUILD_STICKER_EDIT(app_, guild_id_, sticker_id_, json_body_) \
    DCC_APP_EDIT_GUILD_STICKER((app_), (guild_id_), (sticker_id_), (json_body_))
#define DCC_GUILD_STICKER_DELETE(app_, guild_id_, sticker_id_) \
    DCC_APP_DELETE_GUILD_STICKER((app_), (guild_id_), (sticker_id_))
#define DCC_STICKER_FETCH(app_, sticker_id_) \
    DCC_APP_GET_STICKER((app_), (sticker_id_))
#define DCC_STICKER_PACKS_FETCH(app_) \
    DCC_APP_GET_STICKER_PACKS((app_))
#define DCC_GUILD_MESSAGES_SEARCH(app_, guild_id_, query_) \
    DCC_APP_GUILD_MESSAGE_SEARCH((app_), (guild_id_), (query_))
#define DCC_GUILD_MESSAGES_SEARCH_PARAMS(app_, guild_id_, params_) \
    DCC_APP_GUILD_MESSAGE_SEARCH_PARAMS((app_), (guild_id_), (params_))

#endif /* DCC_SUGAR_GUILD_SHORTCUTS_MEDIA_H */
