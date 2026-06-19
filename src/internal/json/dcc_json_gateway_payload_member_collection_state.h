#ifndef DCC_JSON_GATEWAY_PAYLOAD_MEMBER_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_MEMBER_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_MEMBER_COLLECTION_FIELDS \
    dcc_channel_t gateway_threads[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_thread_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_thread_topics[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][1024]; \
    dcc_thread_member_t gateway_thread_members[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_thread_member_join_timestamps[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]; \
    dcc_member_t gateway_members[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    dcc_snowflake_t gateway_member_role_id_storage[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][DCC_JSON_GATEWAY_ID_LIST_CAP]; \
    char gateway_member_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_member_user_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_member_user_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_member_nicks[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_member_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_member_joined_ats[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]; \
    char gateway_member_premium_sinces[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]; \
    char gateway_member_communication_disabled_untils[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]

#endif
