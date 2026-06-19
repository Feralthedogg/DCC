#include "json_smoke_gateway_foundation_internal.h"
#include "json_smoke_support.h"

int dcc_json_smoke_gateway_foundation_automation(void) {
    dcc_status_t st = DCC_OK;
    dcc_json_gateway_payload_t payload;

    const char automod_input[] =
        "{\"t\":\"AUTO_MODERATION_ACTION_EXECUTION\",\"s\":104,\"op\":0,\"d\":{"
        "\"guild_id\":\"333\",\"action\":{\"type\":1},\"rule_id\":\"777\","
        "\"rule_trigger_type\":4,\"user_id\":\"446\",\"channel_id\":\"222\","
        "\"message_id\":\"111\",\"alert_system_message_id\":\"112\","
        "\"content\":\"blocked text\",\"matched_keyword\":\"bad\","
        "\"matched_content\":\"bad text\"}}";
    st = dcc_json_parse_gateway_payload(automod_input, sizeof(automod_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.action_type != 1 ||
        payload.gateway.type != 1 ||
        payload.gateway.rule_id != 777 ||
        payload.gateway.trigger_type != 4 ||
        payload.gateway.user_id != 446 ||
        payload.gateway.channel_id != 222 ||
        payload.gateway.message_id != 111 ||
        payload.gateway.alert_system_message_id != 112 ||
        payload.gateway.content == NULL ||
        strcmp(payload.gateway.content, "blocked text") != 0 ||
        payload.gateway.matched_keyword == NULL ||
        strcmp(payload.gateway.matched_keyword, "bad") != 0 ||
        payload.gateway.matched_content == NULL ||
        strcmp(payload.gateway.matched_content, "bad text") != 0 ||
        payload.auto_moderation_action.guild_id != 333 ||
        payload.auto_moderation_action.action_type != 1 ||
        payload.auto_moderation_action.rule_id != 777 ||
        payload.auto_moderation_action.trigger_type != 4 ||
        payload.auto_moderation_action.user_id != 446 ||
        payload.auto_moderation_action.channel_id != 222 ||
        payload.auto_moderation_action.message_id != 111 ||
        payload.auto_moderation_action.alert_system_message_id != 112 ||
        payload.auto_moderation_action.content == NULL ||
        strcmp(payload.auto_moderation_action.content, "blocked text") != 0 ||
        payload.auto_moderation_action.matched_keyword == NULL ||
        strcmp(payload.auto_moderation_action.matched_keyword, "bad") != 0 ||
        payload.auto_moderation_action.matched_content == NULL ||
        strcmp(payload.auto_moderation_action.matched_content, "bad text") != 0) {
        fprintf(stderr, "gateway automod projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char automod_rule_input[] =
        "{\"t\":\"AUTO_MODERATION_RULE_CREATE\",\"s\":105,\"op\":0,\"d\":{"
        "\"id\":\"778\",\"guild_id\":\"333\",\"name\":\"links\","
        "\"creator_id\":\"446\",\"event_type\":1,\"trigger_type\":3,"
        "\"enabled\":true,\"exempt_roles\":[\"555\"],"
        "\"exempt_channels\":[\"222\",\"223\"]}}";
    st = dcc_json_parse_gateway_payload(automod_rule_input, sizeof(automod_rule_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.rule_id != 778 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.name == NULL ||
        strcmp(payload.gateway.name, "links") != 0 ||
        payload.gateway.creator_id != 446 ||
        payload.gateway.event_type != 1 ||
        payload.gateway.trigger_type != 3 ||
        payload.gateway.enabled != 1 ||
        payload.gateway.role_ids == NULL ||
        payload.gateway.role_ids_count != 1 ||
        payload.gateway.role_ids[0] != 555 ||
        payload.gateway.channel_ids == NULL ||
        payload.gateway.channel_ids_count != 2 ||
        payload.gateway.channel_ids[0] != 222 ||
        payload.gateway.channel_ids[1] != 223 ||
        payload.auto_moderation_rule.id != 778 ||
        payload.auto_moderation_rule.guild_id != 333 ||
        payload.auto_moderation_rule.name == NULL ||
        strcmp(payload.auto_moderation_rule.name, "links") != 0 ||
        payload.auto_moderation_rule.creator_id != 446 ||
        payload.auto_moderation_rule.event_type != 1 ||
        payload.auto_moderation_rule.trigger_type != 3 ||
        payload.auto_moderation_rule.enabled != 1 ||
        payload.auto_moderation_rule.role_ids == NULL ||
        payload.auto_moderation_rule.role_ids_count != 1 ||
        payload.auto_moderation_rule.role_ids[0] != 555 ||
        payload.auto_moderation_rule.channel_ids == NULL ||
        payload.auto_moderation_rule.channel_ids_count != 2 ||
        payload.auto_moderation_rule.channel_ids[0] != 222 ||
        payload.auto_moderation_rule.channel_ids[1] != 223) {
        fprintf(stderr, "gateway automod rule projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char poll_input[] =
        "{\"t\":\"MESSAGE_POLL_VOTE_ADD\",\"s\":104,\"op\":0,\"d\":{"
        "\"user_id\":\"446\",\"channel_id\":\"222\",\"message_id\":\"111\","
        "\"guild_id\":\"333\",\"answer_id\":3}}";
    st = dcc_json_parse_gateway_payload(poll_input, sizeof(poll_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.user_id != 446 ||
        payload.gateway.channel_id != 222 ||
        payload.gateway.message_id != 111 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.answer_id != 3 ||
        payload.poll_vote.user_id != 446 ||
        payload.poll_vote.channel_id != 222 ||
        payload.poll_vote.message_id != 111 ||
        payload.poll_vote.guild_id != 333 ||
        payload.poll_vote.answer_id != 3) {
        fprintf(stderr, "gateway poll projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char invite_input[] =
        "{\"t\":\"INVITE_CREATE\",\"s\":105,\"op\":0,\"d\":{"
        "\"channel_id\":\"222\",\"guild_id\":\"333\",\"code\":\"abc\","
        "\"inviter\":{\"id\":\"446\",\"username\":\"member\"},\"target_type\":2,"
        "\"max_age\":60,\"max_uses\":5,\"uses\":1,"
        "\"created_at\":\"2026-06-15T00:00:00.000000+00:00\","
        "\"expires_at\":\"2026-06-16T00:00:00.000000+00:00\"}}";
    st = dcc_json_parse_gateway_payload(invite_input, sizeof(invite_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.channel_id != 222 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.code == NULL ||
        strcmp(payload.gateway.code, "abc") != 0 ||
        payload.gateway.user_id != 446 ||
        payload.gateway.target_type != 2 ||
        payload.gateway.max_age != 60 ||
        payload.gateway.max_uses != 5 ||
        payload.gateway.uses != 1 ||
        payload.gateway.created_at == NULL ||
        payload.gateway.expires_at == NULL ||
        payload.invite.channel_id != 222 ||
        payload.invite.guild_id != 333 ||
        payload.invite.code == NULL ||
        strcmp(payload.invite.code, "abc") != 0 ||
        payload.invite.inviter.id != 446 ||
        payload.invite.inviter.username == NULL ||
        strcmp(payload.invite.inviter.username, "member") != 0 ||
        payload.invite.target_type != 2 ||
        payload.invite.max_age != 60 ||
        payload.invite.max_uses != 5 ||
        payload.invite.uses != 1 ||
        payload.invite.created_at == NULL ||
        payload.invite.expires_at == NULL) {
        fprintf(stderr, "gateway invite projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    const char voice_effect_input[] =
        "{\"t\":\"VOICE_CHANNEL_EFFECT_SEND\",\"s\":106,\"op\":0,\"d\":{"
        "\"channel_id\":\"222\",\"guild_id\":\"333\",\"user_id\":\"446\","
        "\"emoji\":{\"id\":\"9999\",\"name\":\"spark\"},\"animation_type\":1,"
        "\"animation_id\":\"1234\",\"sound_id\":\"5678\",\"sound_volume\":0.5}}";
    st = dcc_json_parse_gateway_payload(voice_effect_input, sizeof(voice_effect_input) - 1, &payload);
    if (st != DCC_OK ||
        payload.gateway.channel_id != 222 ||
        payload.gateway.guild_id != 333 ||
        payload.gateway.user_id != 446 ||
        payload.gateway.emoji_id != 9999 ||
        payload.gateway.animation_type != 1 ||
        payload.gateway.animation_id != 1234 ||
        payload.gateway.sound_id != 5678 ||
        payload.gateway.volume < 0.49 ||
        payload.gateway.volume > 0.51 ||
        payload.voice_channel_effect.channel_id != 222 ||
        payload.voice_channel_effect.guild_id != 333 ||
        payload.voice_channel_effect.user_id != 446 ||
        payload.voice_channel_effect.emoji_id != 9999 ||
        payload.voice_channel_effect.emoji_name == NULL ||
        strcmp(payload.voice_channel_effect.emoji_name, "spark") != 0 ||
        payload.voice_channel_effect.animation_type != 1 ||
        payload.voice_channel_effect.animation_id != 1234 ||
        payload.voice_channel_effect.sound_id != 5678 ||
        payload.voice_channel_effect.volume < 0.49 ||
        payload.voice_channel_effect.volume > 0.51) {
        fprintf(stderr, "gateway voice effect projection validation failed: %s\n", dcc_status_string(st));
        return 1;
    }

    return 0;
}
