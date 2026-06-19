#include "internal/json/dcc_json_internal.h"

void dcc_json_gateway_normalize_moderation_events(dcc_json_gateway_payload_t *out) {
    if (out->voice_channel_effect.user_id == 0 && out->gateway.user_id != 0) {
        out->voice_channel_effect.user_id = out->gateway.user_id;
    }
    if (out->voice_channel_effect.channel_id == 0 && out->gateway.channel_id != 0) {
        out->voice_channel_effect.channel_id = out->gateway.channel_id;
    }
    if (out->voice_channel_effect.guild_id == 0 && out->gateway.guild_id != 0) {
        out->voice_channel_effect.guild_id = out->gateway.guild_id;
    }
    if (out->voice_channel_effect.emoji_id == 0 && out->gateway.emoji_id != 0) {
        out->voice_channel_effect.emoji_id = out->gateway.emoji_id;
    }
    if (out->voice_channel_effect.animation_id == 0 && out->gateway.animation_id != 0) {
        out->voice_channel_effect.animation_id = out->gateway.animation_id;
    }
    if (out->voice_channel_effect.sound_id == 0 && out->gateway.sound_id != 0) {
        out->voice_channel_effect.sound_id = out->gateway.sound_id;
    }
    if (out->auto_moderation_action.guild_id == 0 && out->gateway.guild_id != 0) {
        out->auto_moderation_action.guild_id = out->gateway.guild_id;
    }
    if (out->auto_moderation_action.rule_id == 0 && out->gateway.rule_id != 0) {
        out->auto_moderation_action.rule_id = out->gateway.rule_id;
    }
    if (out->auto_moderation_action.user_id == 0 && out->gateway.user_id != 0) {
        out->auto_moderation_action.user_id = out->gateway.user_id;
    }
    if (out->auto_moderation_action.channel_id == 0 && out->gateway.channel_id != 0) {
        out->auto_moderation_action.channel_id = out->gateway.channel_id;
    }
    if (out->auto_moderation_action.message_id == 0 && out->gateway.message_id != 0) {
        out->auto_moderation_action.message_id = out->gateway.message_id;
    }
    if (out->auto_moderation_action.alert_system_message_id == 0 &&
        out->gateway.alert_system_message_id != 0) {
        out->auto_moderation_action.alert_system_message_id = out->gateway.alert_system_message_id;
    }
    if (out->auto_moderation_rule.id == 0 && out->gateway.rule_id != 0) {
        out->auto_moderation_rule.id = out->gateway.rule_id;
    }
    if (out->auto_moderation_rule.guild_id == 0 && out->gateway.guild_id != 0) {
        out->auto_moderation_rule.guild_id = out->gateway.guild_id;
    }
    if (out->auto_moderation_rule.creator_id == 0 && out->gateway.creator_id != 0) {
        out->auto_moderation_rule.creator_id = out->gateway.creator_id;
    }
    if (out->message_delete_bulk.guild_id == 0 && out->gateway.guild_id != 0) {
        out->message_delete_bulk.guild_id = out->gateway.guild_id;
    }
    if (out->message_delete_bulk.channel_id == 0 && out->gateway.channel_id != 0) {
        out->message_delete_bulk.channel_id = out->gateway.channel_id;
    }
}
