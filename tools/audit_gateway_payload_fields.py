#!/usr/bin/env python3
"""Audit high-risk gateway payload field smoke coverage.

The direct gateway parser intentionally projects Discord payloads into fixed
scratch storage. This audit keeps that projection test corpus from silently
shrinking when payload structs or DPP-mapped events change.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PAYLOAD_STATE_HEADER = ROOT / "src/internal/json/dcc_json_gateway_payload_state.h"
JSON_SMOKE = ROOT / "tests/json_smoke.c"
JSON_SMOKE_SUPPORT_GLOB = "json_smoke*.c"
GATEWAY_SMOKE = ROOT / "tests/gateway_smoke.c"
GATEWAY_SMOKE_HANDLER_GLOB = "gateway_smoke_handlers*.c"


REQUIRED_DIRECT_FIXTURE_EVENTS = {
    "APPLICATION_COMMAND_PERMISSIONS_UPDATE",
    "AUTO_MODERATION_ACTION_EXECUTION",
    "AUTO_MODERATION_RULE_CREATE",
    "CHANNEL_CREATE",
    "CHANNEL_PINS_UPDATE",
    "ENTITLEMENT_CREATE",
    "GUILD_CREATE",
    "GUILD_EMOJIS_UPDATE",
    "GUILD_MEMBER_ADD",
    "GUILD_MEMBER_UPDATE",
    "GUILD_MEMBERS_CHUNK",
    "GUILD_ROLE_CREATE",
    "GUILD_SCHEDULED_EVENT_CREATE",
    "GUILD_SOUNDBOARD_SOUND_CREATE",
    "GUILD_SOUNDBOARD_SOUNDS_UPDATE",
    "GUILD_STICKERS_UPDATE",
    "INTEGRATION_CREATE",
    "INTEGRATION_UPDATE",
    "INVITE_CREATE",
    "MESSAGE_CREATE",
    "MESSAGE_POLL_VOTE_ADD",
    "MESSAGE_REACTION_ADD",
    "PRESENCE_UPDATE",
    "READY",
    "STAGE_INSTANCE_CREATE",
    "SUBSCRIPTION_UPDATE",
    "THREAD_LIST_SYNC",
    "THREAD_MEMBER_UPDATE",
    "THREAD_MEMBERS_UPDATE",
    "TYPING_START",
    "USER_UPDATE",
    "VOICE_CHANNEL_EFFECT_SEND",
    "VOICE_CHANNEL_STATUS_UPDATE",
    "VOICE_STATE_UPDATE",
}

REQUIRED_DIRECT_FIELD_REFERENCES = {
    "payload.has_application_command_permissions_update",
    "payload.has_event_name",
    "payload.has_guild",
    "payload.has_guild_id_list_update",
    "payload.has_op",
    "payload.has_resume_gateway_url",
    "payload.has_seq",
    "payload.has_session_id",
    "payload.has_thread_list_sync",
    "payload.has_user",
    "payload.has_voice_channel_status_update",
    "payload.op",
    "payload.seq",
    "payload.event_name",
    "payload.session_id",
    "payload.ready_guild_ids",
    "payload.ready_guild_ids_count",
    "payload.ready_guild_count",
    "payload.resume_gateway_url",
    "payload.message.webhook_id",
    "payload.message.type",
    "payload.message.tts",
    "payload.message.mention_everyone",
    "payload.message.pinned",
    "payload.message.timestamp",
    "payload.message.edited_timestamp",
    "payload.guild.id",
    "payload.guild.name",
    "payload.guild.owner_id",
    "payload.guild.roles",
    "payload.guild.roles_count",
    "payload.guild.channels",
    "payload.guild.channels_count",
    "payload.guild.scheduled_events",
    "payload.guild.scheduled_events_count",
    "payload.guild.stage_instances",
    "payload.guild.stage_instances_count",
    "payload.guild.presences",
    "payload.guild.presences_count",
    "payload.guild.emojis",
    "payload.guild.emojis_count",
    "payload.guild.stickers",
    "payload.guild.stickers_count",
    "payload.guild.soundboard_sounds",
    "payload.guild.soundboard_sounds_count",
    "payload.channel.id",
    "payload.channel.guild_id",
    "payload.channel.parent_id",
    "payload.channel.last_message_id",
    "payload.channel.type",
    "payload.channel.name",
    "payload.channel.topic",
    "payload.channel.position",
    "payload.channel.nsfw",
    "payload.channel.rate_limit_per_user",
    "payload.channel.bitrate",
    "payload.channel.user_limit",
    "payload.role.id",
    "payload.role.guild_id",
    "payload.role.name",
    "payload.role.permissions",
    "payload.role.color",
    "payload.role.position",
    "payload.role.hoist",
    "payload.role.managed",
    "payload.role.mentionable",
    "payload.member.guild_id",
    "payload.member.user.id",
    "payload.member.user.username",
    "payload.member.nick",
    "payload.member.role_ids",
    "payload.member.role_ids_count",
    "payload.member.joined_at",
    "payload.member.pending",
    "payload.member.communication_disabled_until",
    "payload.scheduled_event.id",
    "payload.scheduled_event.guild_id",
    "payload.scheduled_event.channel_id",
    "payload.scheduled_event.name",
    "payload.scheduled_event.description",
    "payload.scheduled_event.image",
    "payload.scheduled_event.location",
    "payload.scheduled_event.creator.id",
    "payload.scheduled_event.creator.username",
    "payload.scheduled_event.creator.avatar",
    "payload.scheduled_event.status",
    "payload.scheduled_event.user_count",
    "payload.scheduled_event.start_time",
    "payload.scheduled_event.end_time",
    "payload.entitlement.id",
    "payload.entitlement.sku_id",
    "payload.entitlement.application_id",
    "payload.entitlement.subscription_id",
    "payload.entitlement.promotion_id",
    "payload.entitlement.gift_code_flags",
    "payload.entitlement.flags",
    "payload.entitlement.user_id",
    "payload.entitlement.guild_id",
    "payload.entitlement.type",
    "payload.entitlement.start_time",
    "payload.entitlement.end_time",
    "payload.subscription.id",
    "payload.subscription.user_id",
    "payload.subscription.sku_ids",
    "payload.subscription.sku_ids_count",
    "payload.subscription.entitlement_ids",
    "payload.subscription.entitlement_ids_count",
    "payload.subscription.current_period_start",
    "payload.subscription.current_period_end",
    "payload.subscription.canceled_at",
    "payload.subscription.status",
    "payload.invite.channel_id",
    "payload.invite.guild_id",
    "payload.invite.code",
    "payload.invite.inviter.id",
    "payload.invite.inviter.username",
    "payload.invite.target_type",
    "payload.invite.max_age",
    "payload.invite.max_uses",
    "payload.invite.uses",
    "payload.invite.created_at",
    "payload.invite.expires_at",
    "payload.stage_instance.id",
    "payload.stage_instance.guild_id",
    "payload.stage_instance.channel_id",
    "payload.stage_instance.topic",
    "payload.stage_instance.privacy_level",
    "payload.stage_instance.discoverable_disabled",
    "payload.stage_instance.scheduled_event_id",
    "payload.soundboard_sound.id",
    "payload.soundboard_sound.guild_id",
    "payload.soundboard_sound.name",
    "payload.soundboard_sound.volume",
    "payload.soundboard_sound.emoji_id",
    "payload.soundboard_sound.emoji_name",
    "payload.soundboard_sound.available",
    "payload.soundboard_sound.user_id",
    "payload.message_reaction.user_id",
    "payload.message_reaction.channel_id",
    "payload.message_reaction.message_id",
    "payload.message_reaction.guild_id",
    "payload.message_reaction.message_author_id",
    "payload.message_reaction.burst",
    "payload.message_reaction.emoji_id",
    "payload.message_reaction.emoji_name",
    "payload.message_reaction.member.guild_id",
    "payload.message_reaction.member.user.id",
    "payload.message_reaction.member.role_ids",
    "payload.message_reaction.emoji.id",
    "payload.message_reaction.emoji.name",
    "payload.message_reaction.emoji.animated",
    "payload.poll_vote.user_id",
    "payload.poll_vote.channel_id",
    "payload.poll_vote.message_id",
    "payload.poll_vote.guild_id",
    "payload.poll_vote.answer_id",
    "payload.voice_channel_effect.channel_id",
    "payload.voice_channel_effect.guild_id",
    "payload.voice_channel_effect.user_id",
    "payload.voice_channel_effect.emoji_id",
    "payload.voice_channel_effect.emoji_name",
    "payload.voice_channel_effect.animation_type",
    "payload.voice_channel_effect.animation_id",
    "payload.voice_channel_effect.sound_id",
    "payload.voice_channel_effect.volume",
    "payload.auto_moderation_action.action_type",
    "payload.auto_moderation_action.rule_id",
    "payload.auto_moderation_action.trigger_type",
    "payload.auto_moderation_action.user_id",
    "payload.auto_moderation_action.channel_id",
    "payload.auto_moderation_action.message_id",
    "payload.auto_moderation_action.alert_system_message_id",
    "payload.auto_moderation_action.content",
    "payload.auto_moderation_action.matched_keyword",
    "payload.auto_moderation_action.matched_content",
    "payload.auto_moderation_rule.id",
    "payload.auto_moderation_rule.guild_id",
    "payload.auto_moderation_rule.name",
    "payload.auto_moderation_rule.creator_id",
    "payload.auto_moderation_rule.event_type",
    "payload.auto_moderation_rule.trigger_type",
    "payload.auto_moderation_rule.enabled",
    "payload.auto_moderation_rule.role_ids",
    "payload.auto_moderation_rule.role_ids_count",
    "payload.auto_moderation_rule.channel_ids",
    "payload.auto_moderation_rule.channel_ids_count",
    "payload.thread_members_update.id",
    "payload.thread_members_update.guild_id",
    "payload.thread_members_update.count",
    "payload.thread_members_update.added_members",
    "payload.thread_members_update.added_members_count",
    "payload.thread_members_update.member_ids",
    "payload.thread_members_update.member_ids_count",
    "payload.thread_members_update.removed_member_ids",
    "payload.thread_members_update.removed_member_ids_count",
    "payload.guild_members_chunk.guild_id",
    "payload.guild_members_chunk.chunk_index",
    "payload.guild_members_chunk.chunk_count",
    "payload.guild_members_chunk.member_ids",
    "payload.guild_members_chunk.member_ids_count",
    "payload.guild_members_chunk.members",
    "payload.guild_members_chunk.members_count",
    "payload.guild_members_chunk.not_found_ids",
    "payload.guild_members_chunk.not_found_ids_count",
    "payload.guild_members_chunk.nonce",
    "payload.channel_pins_update.guild_id",
    "payload.channel_pins_update.channel_id",
    "payload.channel_pins_update.last_pin_timestamp",
    "payload.presence.user.id",
    "payload.presence.user.username",
    "payload.presence.user.avatar",
    "payload.presence.user.flags",
    "payload.presence.guild_id",
    "payload.presence.status",
    "payload.presence.since",
    "payload.presence.desktop_status",
    "payload.presence.mobile_status",
    "payload.presence.web_status",
    "payload.presence.activities",
    "payload.presence.activities_count",
    "payload.user.id",
    "payload.user.username",
    "payload.user.global_name",
    "payload.user.avatar",
    "payload.user.flags",
    "payload.application_command_permissions_update.id",
    "payload.application_command_permissions_update.application_id",
    "payload.application_command_permissions_update.guild_id",
    "payload.application_command_permissions_update.permissions",
    "payload.application_command_permissions_update.permissions_count",
    "payload.guild_id_list_update.guild_id",
    "payload.guild_id_list_update.ids",
    "payload.guild_id_list_update.ids_count",
    "payload.guild_id_list_update.emojis",
    "payload.guild_id_list_update.emojis_count",
    "payload.guild_id_list_update.stickers",
    "payload.guild_id_list_update.stickers_count",
    "payload.guild_id_list_update.soundboard_sounds",
    "payload.guild_id_list_update.soundboard_sounds_count",
    "payload.thread_list_sync.guild_id",
    "payload.thread_list_sync.channel_ids",
    "payload.thread_list_sync.thread_ids",
    "payload.thread_list_sync.threads",
    "payload.thread_list_sync.threads_count",
    "payload.thread_list_sync.member_ids",
    "payload.thread_list_sync.members",
    "payload.thread_list_sync.members_count",
    "payload.voice_channel_status_update.guild_id",
    "payload.voice_channel_status_update.channel_id",
    "payload.voice_channel_status_update.status",
    "payload.voice_state.guild_id",
    "payload.voice_state.channel_id",
    "payload.voice_state.user_id",
    "payload.voice_state.session_id",
    "payload.voice_state.request_to_speak_timestamp",
    "payload.voice_state.deaf",
    "payload.voice_state.mute",
    "payload.voice_state.self_deaf",
    "payload.voice_state.self_mute",
    "payload.voice_state.self_stream",
    "payload.voice_state.self_video",
    "payload.voice_state.suppress",
}

REQUIRED_GATEWAY_DATA_FIELDS = {
    "payload.gateway.action_type",
    "payload.gateway.activities",
    "payload.gateway.activities_count",
    "payload.gateway.added_members",
    "payload.gateway.added_members_count",
    "payload.gateway.afk_channel_id",
    "payload.gateway.afk_timeout",
    "payload.gateway.alert_system_message_id",
    "payload.gateway.animation_id",
    "payload.gateway.animation_type",
    "payload.gateway.answer_id",
    "payload.gateway.application_id",
    "payload.gateway.available",
    "payload.gateway.banner",
    "payload.gateway.burst",
    "payload.gateway.canceled_at",
    "payload.gateway.channel_id",
    "payload.gateway.channel_ids",
    "payload.gateway.channel_ids_count",
    "payload.gateway.chunk_count",
    "payload.gateway.chunk_index",
    "payload.gateway.code",
    "payload.gateway.communication_disabled_until",
    "payload.gateway.content",
    "payload.gateway.count",
    "payload.gateway.created_at",
    "payload.gateway.creator_id",
    "payload.gateway.current_period_end",
    "payload.gateway.current_period_start",
    "payload.gateway.default_message_notifications",
    "payload.gateway.description",
    "payload.gateway.desktop_status",
    "payload.gateway.discoverable_disabled",
    "payload.gateway.discovery_splash",
    "payload.gateway.emoji_id",
    "payload.gateway.emoji_name",
    "payload.gateway.enabled",
    "payload.gateway.end_time",
    "payload.gateway.entitlement_id",
    "payload.gateway.entitlement_ids",
    "payload.gateway.entitlement_ids_count",
    "payload.gateway.entity_id",
    "payload.gateway.entity_type",
    "payload.gateway.event_type",
    "payload.gateway.expires_at",
    "payload.gateway.explicit_content_filter",
    "payload.gateway.flags",
    "payload.gateway.gift_code_flags",
    "payload.gateway.guild_id",
    "payload.gateway.icon",
    "payload.gateway.id",
    "payload.gateway.ids",
    "payload.gateway.ids_count",
    "payload.gateway.join_timestamp",
    "payload.gateway.joined_at",
    "payload.gateway.matched_content",
    "payload.gateway.matched_keyword",
    "payload.gateway.max_age",
    "payload.gateway.max_members",
    "payload.gateway.max_presences",
    "payload.gateway.max_uses",
    "payload.gateway.max_video_channel_users",
    "payload.gateway.member_ids",
    "payload.gateway.member_ids_count",
    "payload.gateway.members",
    "payload.gateway.members_count",
    "payload.gateway.message_author_id",
    "payload.gateway.message_id",
    "payload.gateway.mfa_level",
    "payload.gateway.mobile_status",
    "payload.gateway.name",
    "payload.gateway.nick",
    "payload.gateway.nonce",
    "payload.gateway.not_found_ids",
    "payload.gateway.not_found_ids_count",
    "payload.gateway.nsfw_level",
    "payload.gateway.owner_id",
    "payload.gateway.pending",
    "payload.gateway.preferred_locale",
    "payload.gateway.premium_progress_bar_enabled",
    "payload.gateway.premium_since",
    "payload.gateway.premium_subscription_count",
    "payload.gateway.premium_tier",
    "payload.gateway.privacy_level",
    "payload.gateway.promotion_id",
    "payload.gateway.public_updates_channel_id",
    "payload.gateway.role_id",
    "payload.gateway.role_ids",
    "payload.gateway.role_ids_count",
    "payload.gateway.rule_id",
    "payload.gateway.rules_channel_id",
    "payload.gateway.safety_alerts_channel_id",
    "payload.gateway.scheduled_event_id",
    "payload.gateway.since",
    "payload.gateway.sku_id",
    "payload.gateway.sku_ids",
    "payload.gateway.sku_ids_count",
    "payload.gateway.sound_id",
    "payload.gateway.splash",
    "payload.gateway.start_time",
    "payload.gateway.status",
    "payload.gateway.subscription_id",
    "payload.gateway.system_channel_id",
    "payload.gateway.target_type",
    "payload.gateway.timestamp",
    "payload.gateway.timestamp_text",
    "payload.gateway.topic",
    "payload.gateway.trigger_type",
    "payload.gateway.type",
    "payload.gateway.type_text",
    "payload.gateway.user.avatar",
    "payload.gateway.user.bot",
    "payload.gateway.user.discriminator",
    "payload.gateway.user.flags",
    "payload.gateway.user.global_name",
    "payload.gateway.user.id",
    "payload.gateway.user.username",
    "payload.gateway.user_id",
    "payload.gateway.uses",
    "payload.gateway.vanity_url_code",
    "payload.gateway.verification_level",
    "payload.gateway.volume",
    "payload.gateway.web_status",
    "payload.gateway.widget_channel_id",
    "payload.gateway.widget_enabled",
}


def parse_gateway_payload_object_fields(text: str) -> list[str]:
    match = re.search(r"typedef struct dcc_json_gateway_payload \{(.*?)\n\}\s*dcc_json_gateway_payload_t;", text, re.S)
    if match is None:
        raise ValueError("cannot find dcc_json_gateway_payload_t definition")

    body = match.group(1)
    fields = re.findall(r"\bdcc_[A-Za-z0-9_]+_t\s+([a-z][A-Za-z0-9_]*)\s*;", body)
    return [field for field in fields if field != "gateway"]


def collect_payload_field_refs(text: str) -> set[str]:
    refs: set[str] = set()
    for match in re.findall(r"\bpayload((?:(?:\.|->)[A-Za-z_][A-Za-z0-9_]*){1,5})", text):
        refs.add("payload" + match.replace("->", "."))
    return refs


def read_json_smoke_text() -> str:
    parts = [JSON_SMOKE.read_text()]
    for support in sorted((ROOT / "tests/support").glob(JSON_SMOKE_SUPPORT_GLOB)):
        parts.append(support.read_text())
    return "\n".join(parts)


def direct_projection_seen(object_name: str, text: str) -> bool:
    return re.search(rf"\bpayload(?:\.|->){re.escape(object_name)}\.", text) is not None


def accessor_projection_seen(object_name: str, text: str) -> bool:
    return f"dcc_event_{object_name}(event)" in text


def main() -> int:
    errors: list[str] = []

    header_text = PAYLOAD_STATE_HEADER.read_text()
    json_smoke_text = read_json_smoke_text()
    gateway_smoke_text = GATEWAY_SMOKE.read_text()
    for handler in sorted((ROOT / "tests/support").glob(GATEWAY_SMOKE_HANDLER_GLOB)):
        gateway_smoke_text += "\n" + handler.read_text()
    json_payload_refs = collect_payload_field_refs(json_smoke_text)

    missing_events = sorted(event for event in REQUIRED_DIRECT_FIXTURE_EVENTS if event not in json_smoke_text)
    if missing_events:
        errors.append(f"json_smoke missing direct gateway fixture event(s): {', '.join(missing_events)}")

    missing_direct_fields = sorted(REQUIRED_DIRECT_FIELD_REFERENCES - json_payload_refs)
    if missing_direct_fields:
        errors.append(f"json_smoke missing direct payload field assertion(s): {', '.join(missing_direct_fields)}")

    missing_gateway_fields = sorted(REQUIRED_GATEWAY_DATA_FIELDS - json_payload_refs)
    if missing_gateway_fields:
        errors.append(f"json_smoke missing gateway generic field assertion(s): {', '.join(missing_gateway_fields)}")

    object_fields = parse_gateway_payload_object_fields(header_text)
    missing_object_projection: list[str] = []
    for field in object_fields:
        if direct_projection_seen(field, json_smoke_text) or accessor_projection_seen(field, gateway_smoke_text):
            continue
        missing_object_projection.append(field)

    if missing_object_projection:
        errors.append(
            "gateway payload object field(s) missing json_smoke projection or gateway_smoke accessor coverage: "
            + ", ".join(sorted(missing_object_projection))
        )

    if len(json_payload_refs) < 260:
        errors.append(f"json_smoke payload field assertion corpus unexpectedly small: {len(json_payload_refs)} refs")

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(
        "gateway payload field audit passed: "
        f"{len(REQUIRED_DIRECT_FIXTURE_EVENTS)} direct fixture event(s), "
        f"{len(REQUIRED_DIRECT_FIELD_REFERENCES)} typed field assertion(s), "
        f"{len(REQUIRED_GATEWAY_DATA_FIELDS)} gateway generic field assertion(s), "
        f"{len(object_fields)} payload object projection(s)"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
