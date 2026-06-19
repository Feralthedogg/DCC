#ifndef DCC_JSON_GATEWAY_PAYLOAD_PRESENCE_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_PRESENCE_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_PRESENCE_COLLECTION_FIELDS \
    dcc_presence_t gateway_guild_presences[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_guild_presence_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_presence_user_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_presence_user_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_presence_statuses[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][256]; \
    char gateway_guild_presence_desktop_statuses[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][32]; \
    char gateway_guild_presence_mobile_statuses[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][32]; \
    char gateway_guild_presence_web_statuses[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][32]; \
    dcc_gateway_activity_t gateway_guild_presence_activities \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP]; \
    char gateway_guild_presence_activity_names \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP][128]; \
    char gateway_guild_presence_activity_states \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_guild_presence_activity_details \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_guild_presence_activity_urls \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP][256]; \
    char gateway_guild_presence_activity_emoji_names \
        [DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ACTIVITY_CAP][128]

#endif
