#ifndef DCC_JSON_GATEWAY_PAYLOAD_GUILD_UPDATE_RESOURCE_STATE_H
#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_UPDATE_RESOURCE_STATE_H

#define DCC_JSON_GATEWAY_PAYLOAD_GUILD_UPDATE_RESOURCE_FIELDS \
    int has_guild_id_list_update; \
    dcc_guild_id_list_update_t guild_id_list_update; \
    int has_guild_update_notice; \
    dcc_guild_update_notice_t guild_update_notice; \
    int has_scheduled_event_user_update; \
    dcc_scheduled_event_user_update_t scheduled_event_user_update; \
    int has_thread_list_sync; \
    dcc_thread_list_sync_t thread_list_sync; \
    int has_voice_channel_start_time_update; \
    dcc_voice_channel_start_time_update_t voice_channel_start_time_update; \
    int has_voice_channel_status_update; \
    dcc_voice_channel_status_update_t voice_channel_status_update; \
    int has_webhooks_update; \
    dcc_webhooks_update_t webhooks_update; \
    int has_guild_join_request_update; \
    dcc_guild_join_request_update_t guild_join_request_update; \
    int has_guild_application_command_update; \
    dcc_guild_application_command_update_t guild_application_command_update; \
    int has_embedded_activity_update; \
    dcc_embedded_activity_update_t embedded_activity_update; \
    int has_voice_state; \
    dcc_voice_state_t voice_state; \
    char voice_state_request_to_speak_timestamp[64]

#endif
