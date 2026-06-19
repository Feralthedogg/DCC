#ifndef DCC_JSON_GATEWAY_PAYLOAD_LIST_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_LIST_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_LIST_COLLECTION_FIELDS \
    dcc_emoji_t gateway_emojis[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_emoji_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_emoji_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_emoji_user_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_emoji_user_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    dcc_sticker_t gateway_stickers[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_sticker_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_sticker_descriptions[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][256]; \
    char gateway_sticker_tags[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][256]; \
    char gateway_sticker_assets[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_sticker_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_sticker_user_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_sticker_user_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    dcc_soundboard_sound_t gateway_soundboard_sounds[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_soundboard_sound_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_soundboard_sound_emoji_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_soundboard_sound_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_soundboard_sound_user_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_soundboard_sound_user_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]

#endif
