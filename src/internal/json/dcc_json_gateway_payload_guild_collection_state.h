#ifndef DCC_JSON_GATEWAY_PAYLOAD_GUILD_COLLECTION_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_COLLECTION_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_COLLECTION_FIELDS \
    dcc_application_command_permission_t gateway_application_command_permissions[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    dcc_role_t gateway_guild_roles[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_guild_role_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    dcc_channel_t gateway_guild_channels[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_guild_channel_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_channel_topics[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][1024]; \
    dcc_scheduled_event_t gateway_guild_scheduled_events[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_guild_scheduled_event_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_scheduled_event_descriptions[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][1024]; \
    char gateway_guild_scheduled_event_images[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][256]; \
    char gateway_guild_scheduled_event_locations[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][1024]; \
    char gateway_guild_scheduled_event_start_times[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]; \
    char gateway_guild_scheduled_event_end_times[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][64]; \
    char gateway_guild_scheduled_event_creator_usernames[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_scheduled_event_creator_global_names[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    char gateway_guild_scheduled_event_creator_avatars[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][128]; \
    dcc_stage_instance_t gateway_guild_stage_instances[DCC_JSON_GATEWAY_OBJECT_LIST_CAP]; \
    char gateway_guild_stage_instance_topics[DCC_JSON_GATEWAY_OBJECT_LIST_CAP][1024]

#endif
