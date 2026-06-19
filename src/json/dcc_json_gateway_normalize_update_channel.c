#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_update_channel_events(dcc_json_gateway_payload_t *out) {
    if (out->voice_channel_start_time_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->voice_channel_start_time_update.guild_id = out->gateway.guild_id;
    }
    if (out->voice_channel_start_time_update.channel_id == 0 && out->gateway.channel_id != 0) {
        out->voice_channel_start_time_update.channel_id = out->gateway.channel_id;
    }
    if (out->voice_channel_start_time_update.voice_start_time == 0 && out->gateway.timestamp != 0) {
        out->voice_channel_start_time_update.voice_start_time = out->gateway.timestamp;
    }
    if (out->voice_channel_status_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->voice_channel_status_update.guild_id = out->gateway.guild_id;
    }
    if (out->voice_channel_status_update.channel_id == 0 && out->gateway.channel_id != 0) {
        out->voice_channel_status_update.channel_id = out->gateway.channel_id;
    }
    if (out->voice_channel_status_update.status == NULL && out->gateway.status != NULL) {
        out->voice_channel_status_update.status = out->gateway.status;
    }
    if (out->webhooks_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->webhooks_update.guild_id = out->gateway.guild_id;
    }
    if (out->webhooks_update.channel_id == 0 && out->gateway.channel_id != 0) {
        out->webhooks_update.channel_id = out->gateway.channel_id;
    }
    if (out->guild_join_request_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_join_request_update.guild_id = out->gateway.guild_id;
    }
    if (out->guild_join_request_update.user_id == 0 && out->gateway.user_id != 0) {
        out->guild_join_request_update.user_id = out->gateway.user_id;
    }
    if (out->guild_join_request_update.status == NULL && out->gateway.status != NULL) {
        out->guild_join_request_update.status = out->gateway.status;
    }
    if (out->guild_application_command_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->guild_application_command_update.guild_id = out->gateway.guild_id;
    }
    if (out->guild_application_command_update.application_id == 0 && out->gateway.application_id != 0) {
        out->guild_application_command_update.application_id = out->gateway.application_id;
    }
    if (out->embedded_activity_update.guild_id == 0 && out->gateway.guild_id != 0) {
        out->embedded_activity_update.guild_id = out->gateway.guild_id;
    }
    if (out->embedded_activity_update.channel_id == 0 && out->gateway.channel_id != 0) {
        out->embedded_activity_update.channel_id = out->gateway.channel_id;
    }
    if (out->embedded_activity_update.application_id == 0 && out->gateway.application_id != 0) {
        out->embedded_activity_update.application_id = out->gateway.application_id;
    }
}
