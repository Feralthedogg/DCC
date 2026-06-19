#include "internal/json/dcc_json_internal.h"

#include <stddef.h>

void dcc_json_gateway_normalize_guild_collections(dcc_json_gateway_payload_t *out) {
    dcc_snowflake_t guild_id = out->gateway.guild_id != 0 ? out->gateway.guild_id : out->guild.id;
    if (guild_id != 0) {
        for (size_t i = 0; i < out->guild.roles_count; i++) {
            dcc_role_t *role = (dcc_role_t *)&out->guild.roles[i];
            if (role->guild_id == 0) {
                role->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.channels_count; i++) {
            dcc_channel_t *channel = (dcc_channel_t *)&out->guild.channels[i];
            if (channel->guild_id == 0) {
                channel->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.threads_count; i++) {
            dcc_channel_t *thread = (dcc_channel_t *)&out->guild.threads[i];
            if (thread->guild_id == 0) {
                thread->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.scheduled_events_count; i++) {
            dcc_scheduled_event_t *event = (dcc_scheduled_event_t *)&out->guild.scheduled_events[i];
            if (event->guild_id == 0) {
                event->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.stage_instances_count; i++) {
            dcc_stage_instance_t *stage_instance = (dcc_stage_instance_t *)&out->guild.stage_instances[i];
            if (stage_instance->guild_id == 0) {
                stage_instance->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.presences_count; i++) {
            dcc_presence_t *presence = (dcc_presence_t *)&out->guild.presences[i];
            if (presence->guild_id == 0) {
                presence->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.stickers_count; i++) {
            dcc_sticker_t *sticker = (dcc_sticker_t *)&out->guild.stickers[i];
            if (sticker->guild_id == 0) {
                sticker->guild_id = guild_id;
            }
        }
        for (size_t i = 0; i < out->guild.soundboard_sounds_count; i++) {
            dcc_soundboard_sound_t *sound = (dcc_soundboard_sound_t *)&out->guild.soundboard_sounds[i];
            if (sound->guild_id == 0) {
                sound->guild_id = guild_id;
            }
        }
    }
}
