#!/usr/bin/env python3
"""Audit selected DCC REST wrappers against the DPP 10.1.5 REST surface."""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DPP_SOURCE_CANDIDATES = [
    Path(os.environ["DCC_DPP_ROOT"]) / "src/dpp/cluster"
    for _ in [0]
    if os.environ.get("DCC_DPP_ROOT")
] + [
    ROOT.parent / "DPP/src/dpp/cluster",
]

REST_H = ROOT / "include/dcc/rest.h"
REST_HEADER_DIR = ROOT / "include/dcc/rest"
REST_SOURCE_DIR = ROOT / "src/rest"
HTTP_SMOKE = ROOT / "tests/http_smoke.c"
HTTP_SMOKE_SUPPORT_GLOB = "http_smoke*.c"

DPP_REST_CLUSTER_FILES = {
    "appcommand.cpp",
    "automod.cpp",
    "channel.cpp",
    "dm.cpp",
    "emoji.cpp",
    "entitlement.cpp",
    "gateway.cpp",
    "guild.cpp",
    "guild_member.cpp",
    "invite.cpp",
    "message.cpp",
    "role.cpp",
    "scheduled_event.cpp",
    "sku.cpp",
    "stage_instance.cpp",
    "sticker.cpp",
    "template.cpp",
    "thread.cpp",
    "user.cpp",
    "voice.cpp",
    "webhook.cpp",
}

DPP_CLUSTER_NON_REST_FILES = {
    "confirmation.cpp",
    "json_interface.cpp",
    "timer.cpp",
}

DPP_REST_NON_ENDPOINT_METHODS = {
    "user_get_cached",
}

# DPP method -> DCC public REST wrapper(s). This tracks the DPP 10.1.5
# cluster REST surface. Object-builder-only DPP methods map to DCC raw JSON
# wrappers where that is the stable C ABI equivalent.
DPP_TO_DCC = {
    "application_emoji_create": {
        "dcc_rest_create_application_emoji",
        "dcc_rest_create_application_emoji_params",
    },
    "application_emoji_delete": {"dcc_rest_delete_application_emoji"},
    "application_emoji_edit": {
        "dcc_rest_modify_application_emoji",
        "dcc_rest_modify_application_emoji_params",
    },
    "application_emoji_get": {"dcc_rest_get_application_emoji"},
    "application_emojis_get": {"dcc_rest_get_application_emojis"},
    "application_role_connection_get": {"dcc_rest_get_application_role_connection_metadata"},
    "application_role_connection_update": {
        "dcc_rest_update_application_role_connection_metadata",
        "dcc_rest_update_application_role_connection_metadata_params",
    },
    "automod_rule_create": {
        "dcc_rest_create_auto_moderation_rule",
        "dcc_rest_create_auto_moderation_rule_params",
    },
    "automod_rule_delete": {"dcc_rest_delete_auto_moderation_rule"},
    "automod_rule_edit": {
        "dcc_rest_modify_auto_moderation_rule",
        "dcc_rest_modify_auto_moderation_rule_params",
    },
    "automod_rule_get": {"dcc_rest_get_auto_moderation_rule"},
    "automod_rules_get": {"dcc_rest_get_auto_moderation_rules"},
    "channel_create": {
        "dcc_rest_create_guild_channel",
        "dcc_rest_create_guild_channel_params",
    },
    "channel_delete": {"dcc_rest_delete_channel"},
    "channel_delete_permission": {"dcc_rest_delete_channel_permission"},
    "channel_edit": {
        "dcc_rest_modify_channel",
        "dcc_rest_modify_channel_params",
    },
    "channel_edit_permissions": {
        "dcc_rest_modify_channel_permission",
        "dcc_rest_modify_channel_permission_params",
    },
    "channel_edit_positions": {
        "dcc_rest_modify_guild_channel_positions",
        "dcc_rest_modify_guild_channel_positions_params",
    },
    "channel_follow_news": {"dcc_rest_follow_news_channel"},
    "channel_get": {"dcc_rest_get_channel"},
    "channel_invite_create": {"dcc_rest_create_channel_invite", "dcc_rest_create_channel_invite_params"},
    "channel_invites_get": {"dcc_rest_get_channel_invites"},
    "channel_pins_get": {"dcc_rest_get_channel_pins", "dcc_rest_get_channel_pins_page"},
    "channel_set_voice_status": {
        "dcc_rest_set_channel_voice_status",
        "dcc_rest_set_channel_voice_status_params",
    },
    "channel_typing": {"dcc_rest_trigger_channel_typing"},
    "channels_get": {"dcc_rest_get_guild_channels"},
    "create_dm_channel": {"dcc_rest_create_dm_channel", "dcc_rest_create_dm_channel_params"},
    "create_webhook": {"dcc_rest_create_webhook", "dcc_rest_create_webhook_params"},
    "current_application_get": {"dcc_rest_get_current_application"},
    "current_user_connections_get": {"dcc_rest_get_current_user_connections"},
    "current_user_edit": {"dcc_rest_modify_current_user", "dcc_rest_modify_current_user_params"},
    "current_user_get": {"dcc_rest_get_current_user"},
    "current_user_get_dms": {"dcc_rest_get_current_user_dms"},
    "current_user_get_guilds": {"dcc_rest_get_current_user_guilds"},
    "current_user_get_voice_state": {"dcc_rest_get_current_user_voice_state"},
    "current_user_join_thread": {"dcc_rest_join_thread"},
    "current_user_leave_guild": {"dcc_rest_leave_guild"},
    "current_user_leave_thread": {"dcc_rest_leave_thread"},
    "current_user_set_voice_state": {
        "dcc_rest_modify_current_user_voice_state",
        "dcc_rest_modify_current_user_voice_state_params",
    },
    "delete_webhook": {"dcc_rest_delete_webhook"},
    "delete_webhook_message": {
        "dcc_rest_delete_webhook_message",
        "dcc_rest_delete_webhook_message_thread",
    },
    "delete_webhook_with_token": {"dcc_rest_delete_webhook_with_token"},
    "direct_message_create": {"dcc_rest_create_direct_message", "dcc_rest_create_direct_message_builder"},
    "edit_webhook": {"dcc_rest_modify_webhook", "dcc_rest_modify_webhook_params"},
    "edit_webhook_message": {
        "dcc_rest_modify_webhook_message",
        "dcc_rest_modify_webhook_message_builder",
        "dcc_rest_modify_webhook_message_multipart",
        "dcc_rest_modify_webhook_message_multipart_thread",
        "dcc_rest_modify_webhook_message_thread",
    },
    "edit_webhook_with_token": {
        "dcc_rest_modify_webhook_with_token",
        "dcc_rest_modify_webhook_with_token_params",
    },
    "entitlement_consume": {"dcc_rest_consume_entitlement"},
    "entitlement_test_create": {"dcc_rest_create_test_entitlement", "dcc_rest_create_test_entitlement_params"},
    "entitlement_test_delete": {"dcc_rest_delete_test_entitlement"},
    "entitlements_get": {"dcc_rest_get_entitlements", "dcc_rest_get_entitlements_page"},
    "execute_webhook": {
        "dcc_rest_execute_webhook",
        "dcc_rest_execute_webhook_multipart",
        "dcc_rest_execute_webhook_multipart_options",
        "dcc_rest_execute_webhook_options",
    },
    "gdm_add": {"dcc_rest_add_group_dm_recipient", "dcc_rest_add_group_dm_recipient_params"},
    "gdm_remove": {"dcc_rest_remove_group_dm_recipient"},
    "get_channel_webhooks": {"dcc_rest_get_channel_webhooks"},
    "get_gateway_bot": {"dcc_rest_get_gateway_bot"},
    "get_guild_webhooks": {"dcc_rest_get_guild_webhooks"},
    "get_voice_regions": {"dcc_rest_get_voice_regions"},
    "get_webhook": {"dcc_rest_get_webhook"},
    "get_webhook_message": {
        "dcc_rest_get_webhook_message",
        "dcc_rest_get_webhook_message_thread",
    },
    "get_webhook_with_token": {"dcc_rest_get_webhook_with_token"},
    "global_bulk_command_create": {
        "dcc_rest_bulk_overwrite_global_commands",
        "dcc_rest_bulk_overwrite_global_commands_builder",
    },
    "global_bulk_command_delete": {"dcc_rest_delete_all_global_commands"},
    "global_command_create": {"dcc_rest_create_global_command", "dcc_rest_create_global_command_builder"},
    "global_command_delete": {"dcc_rest_delete_global_command"},
    "global_command_edit": {"dcc_rest_edit_global_command", "dcc_rest_edit_global_command_builder"},
    "global_command_get": {"dcc_rest_get_global_command"},
    "global_commands_get": {
        "dcc_rest_get_global_commands",
        "dcc_rest_get_global_commands_with_localizations",
    },
    "guild_add_member": {"dcc_rest_add_guild_member", "dcc_rest_add_guild_member_params"},
    "guild_auditlog_get": {
        "dcc_rest_get_guild_audit_log",
        "dcc_rest_get_guild_audit_log_page",
    },
    "guild_ban_add": {
        "dcc_rest_create_guild_ban",
        "dcc_rest_create_guild_ban_params",
        "dcc_rest_create_guild_ban_seconds",
    },
    "guild_ban_delete": {"dcc_rest_delete_guild_ban"},
    "guild_begin_prune": {
        "dcc_rest_begin_guild_prune",
        "dcc_rest_begin_guild_prune_options",
        "dcc_rest_begin_guild_prune_params",
    },
    "guild_bulk_command_create": {
        "dcc_rest_bulk_overwrite_guild_commands",
        "dcc_rest_bulk_overwrite_guild_commands_builder",
    },
    "guild_bulk_command_delete": {"dcc_rest_delete_all_guild_commands"},
    "guild_bulk_command_edit_permissions": {
        "dcc_rest_bulk_edit_guild_command_permissions",
        "dcc_rest_bulk_edit_guild_command_permissions_params",
    },
    "guild_command_create": {"dcc_rest_create_guild_command", "dcc_rest_create_guild_command_builder"},
    "guild_command_delete": {"dcc_rest_delete_guild_command"},
    "guild_command_edit": {"dcc_rest_edit_guild_command", "dcc_rest_edit_guild_command_builder"},
    "guild_command_edit_permissions": {
        "dcc_rest_edit_guild_command_permissions",
        "dcc_rest_edit_guild_command_permissions_params",
    },
    "guild_command_get": {"dcc_rest_get_guild_command"},
    "guild_command_get_permissions": {"dcc_rest_get_guild_command_permissions"},
    "guild_commands_get": {
        "dcc_rest_get_guild_commands",
        "dcc_rest_get_guild_commands_with_localizations",
    },
    "guild_commands_get_permissions": {"dcc_rest_get_all_guild_command_permissions"},
    "guild_create": {"dcc_rest_create_guild", "dcc_rest_create_guild_params"},
    "guild_create_from_template": {
        "dcc_rest_create_guild_from_template",
        "dcc_rest_create_guild_from_template_params",
    },
    "guild_current_member_edit": {
        "dcc_rest_modify_current_guild_member",
        "dcc_rest_modify_current_guild_member_params",
    },
    "guild_delete": {"dcc_rest_delete_guild"},
    "guild_delete_integration": {"dcc_rest_delete_guild_integration"},
    "guild_edit": {"dcc_rest_modify_guild", "dcc_rest_modify_guild_params"},
    "guild_edit_member": {"dcc_rest_modify_guild_member", "dcc_rest_modify_guild_member_params"},
    "guild_edit_onboarding": {
        "dcc_rest_modify_guild_onboarding",
        "dcc_rest_modify_guild_onboarding_params",
    },
    "guild_edit_welcome_screen": {
        "dcc_rest_modify_guild_welcome_screen",
        "dcc_rest_modify_guild_welcome_screen_params",
    },
    "guild_edit_widget": {"dcc_rest_modify_guild_widget", "dcc_rest_modify_guild_widget_params"},
    "guild_emoji_create": {"dcc_rest_create_guild_emoji", "dcc_rest_create_guild_emoji_params"},
    "guild_emoji_delete": {"dcc_rest_delete_guild_emoji"},
    "guild_emoji_edit": {"dcc_rest_modify_guild_emoji", "dcc_rest_modify_guild_emoji_params"},
    "guild_emoji_get": {"dcc_rest_get_guild_emoji"},
    "guild_emojis_get": {"dcc_rest_get_guild_emojis"},
    "guild_event_create": {
        "dcc_rest_create_guild_scheduled_event",
        "dcc_rest_create_guild_scheduled_event_params",
    },
    "guild_event_delete": {"dcc_rest_delete_guild_scheduled_event"},
    "guild_event_edit": {
        "dcc_rest_modify_guild_scheduled_event",
        "dcc_rest_modify_guild_scheduled_event_params",
    },
    "guild_event_get": {
        "dcc_rest_get_guild_scheduled_event",
        "dcc_rest_get_guild_scheduled_event_with_user_count",
    },
    "guild_event_users_get": {
        "dcc_rest_get_guild_scheduled_event_users",
        "dcc_rest_get_guild_scheduled_event_users_page",
    },
    "guild_events_get": {
        "dcc_rest_get_guild_scheduled_events",
        "dcc_rest_get_guild_scheduled_events_with_user_count",
    },
    "guild_get": {"dcc_rest_get_guild"},
    "guild_get_ban": {"dcc_rest_get_guild_ban"},
    "guild_get_bans": {"dcc_rest_get_guild_bans", "dcc_rest_get_guild_bans_page"},
    "guild_get_integrations": {"dcc_rest_get_guild_integrations"},
    "guild_get_invites": {"dcc_rest_get_guild_invites"},
    "guild_get_member": {"dcc_rest_get_guild_member"},
    "guild_get_members": {"dcc_rest_list_guild_members", "dcc_rest_list_guild_members_page"},
    "guild_get_onboarding": {"dcc_rest_get_guild_onboarding"},
    "guild_get_preview": {"dcc_rest_get_guild_preview"},
    "guild_get_prune_counts": {
        "dcc_rest_get_guild_prune_count",
        "dcc_rest_get_guild_prune_count_options",
    },
    "guild_get_vanity": {"dcc_rest_get_guild_vanity_url"},
    "guild_get_voice_regions": {"dcc_rest_get_guild_voice_regions"},
    "guild_get_welcome_screen": {"dcc_rest_get_guild_welcome_screen"},
    "guild_get_widget": {"dcc_rest_get_guild_widget"},
    "guild_member_add_role": {"dcc_rest_add_guild_member_role", "dcc_rest_add_guild_member_role_params"},
    "guild_member_delete": {"dcc_rest_remove_guild_member"},
    "guild_member_delete_role": {"dcc_rest_remove_guild_member_role"},
    "guild_member_kick": {"dcc_rest_remove_guild_member"},
    "guild_member_move": {"dcc_rest_move_guild_member"},
    "guild_member_remove_role": {"dcc_rest_remove_guild_member_role"},
    "guild_member_timeout": {"dcc_rest_set_guild_member_timeout"},
    "guild_member_timeout_remove": {"dcc_rest_clear_guild_member_timeout"},
    "guild_messages_search": {"dcc_rest_guild_message_search", "dcc_rest_guild_message_search_params"},
    "guild_modify_integration": {"dcc_rest_modify_guild_integration"},
    "guild_search_members": {"dcc_rest_search_guild_members", "dcc_rest_search_guild_members_page"},
    "guild_set_nickname": {
        "dcc_rest_set_current_guild_member_nickname",
        "dcc_rest_set_current_guild_member_nickname_params",
    },
    "guild_sticker_create": {
        "dcc_rest_create_guild_sticker_multipart",
        "dcc_rest_create_guild_sticker_params",
    },
    "guild_sticker_delete": {"dcc_rest_delete_guild_sticker"},
    "guild_sticker_get": {"dcc_rest_get_guild_sticker"},
    "guild_sticker_modify": {"dcc_rest_modify_guild_sticker"},
    "guild_stickers_get": {"dcc_rest_get_guild_stickers"},
    "guild_sync_integration": {"dcc_rest_sync_guild_integration"},
    "guild_template_create": {
        "dcc_rest_create_guild_template",
        "dcc_rest_create_guild_template_params",
    },
    "guild_template_delete": {"dcc_rest_delete_guild_template"},
    "guild_template_modify": {
        "dcc_rest_modify_guild_template",
        "dcc_rest_modify_guild_template_params",
    },
    "guild_template_sync": {"dcc_rest_sync_guild_template"},
    "guild_templates_get": {"dcc_rest_get_guild_templates"},
    "interaction_followup_create": {
        "dcc_rest_interaction_followup_create",
        "dcc_rest_interaction_followup_create_builder",
    },
    "interaction_followup_delete": {"dcc_rest_interaction_followup_delete"},
    "interaction_followup_edit": {
        "dcc_rest_interaction_followup_edit",
        "dcc_rest_interaction_followup_edit_builder",
    },
    "interaction_followup_edit_original": {
        "dcc_rest_interaction_original_response_edit",
        "dcc_rest_interaction_original_response_edit_builder",
    },
    "interaction_followup_get": {"dcc_rest_interaction_followup_get"},
    "interaction_followup_get_original": {"dcc_rest_interaction_original_response_get"},
    "interaction_response_create": {
        "dcc_rest_interaction_response_create",
        "dcc_rest_interaction_response_create_message_builder",
    },
    "interaction_response_edit": {
        "dcc_rest_interaction_original_response_edit",
        "dcc_rest_interaction_original_response_edit_builder",
    },
    "interaction_response_get_original": {"dcc_rest_interaction_original_response_get"},
    "invite_delete": {"dcc_rest_delete_invite"},
    "invite_get": {"dcc_rest_get_invite", "dcc_rest_get_invite_full"},
    "message_add_reaction": {"dcc_rest_add_message_reaction", "dcc_rest_add_message_reaction_params"},
    "message_create": {
        "dcc_rest_create_message",
        "dcc_rest_create_message_builder",
        "dcc_rest_create_message_builder_multipart",
    },
    "message_crosspost": {"dcc_rest_crosspost_message"},
    "message_delete": {"dcc_rest_delete_message"},
    "message_delete_all_reactions": {"dcc_rest_delete_all_message_reactions"},
    "message_delete_bulk": {"dcc_rest_bulk_delete_messages"},
    "message_delete_own_reaction": {"dcc_rest_delete_own_message_reaction"},
    "message_delete_reaction": {"dcc_rest_delete_user_message_reaction"},
    "message_delete_reaction_emoji": {"dcc_rest_delete_all_message_reactions_for_emoji"},
    "message_edit": {
        "dcc_rest_edit_message",
        "dcc_rest_edit_message_builder",
        "dcc_rest_edit_message_builder_multipart",
    },
    "message_edit_flags": {"dcc_rest_edit_message_flags", "dcc_rest_edit_message_flags_params"},
    "message_get": {"dcc_rest_get_message"},
    "message_get_reactions": {"dcc_rest_get_message_reactions", "dcc_rest_get_message_reactions_page"},
    "message_pin": {"dcc_rest_pin_message"},
    "message_unpin": {"dcc_rest_unpin_message"},
    "messages_get": {"dcc_rest_get_channel_messages", "dcc_rest_get_channel_messages_page"},
    "nitro_sticker_get": {"dcc_rest_get_sticker"},
    "poll_end": {"dcc_rest_end_poll"},
    "poll_get_answer_voters": {
        "dcc_rest_get_poll_answer_voters",
        "dcc_rest_get_poll_answer_voters_page",
    },
    "role_create": {"dcc_rest_create_guild_role", "dcc_rest_create_guild_role_params"},
    "role_delete": {"dcc_rest_delete_guild_role"},
    "role_edit": {"dcc_rest_modify_guild_role", "dcc_rest_modify_guild_role_params"},
    "roles_edit_position": {
        "dcc_rest_modify_guild_role_positions",
        "dcc_rest_modify_guild_role_positions_params",
    },
    "roles_get": {"dcc_rest_get_guild_roles"},
    "skus_get": {"dcc_rest_get_skus"},
    "stage_instance_create": {
        "dcc_rest_create_stage_instance",
        "dcc_rest_create_stage_instance_params",
    },
    "stage_instance_delete": {"dcc_rest_delete_stage_instance"},
    "stage_instance_edit": {
        "dcc_rest_modify_stage_instance",
        "dcc_rest_modify_stage_instance_params",
    },
    "stage_instance_get": {"dcc_rest_get_stage_instance"},
    "sticker_packs_get": {"dcc_rest_get_sticker_packs"},
    "template_get": {"dcc_rest_get_template"},
    "thread_create": {
        "dcc_rest_create_thread",
        "dcc_rest_create_thread_params",
    },
    "thread_create_in_forum": {
        "dcc_rest_create_thread",
        "dcc_rest_create_forum_thread_params",
    },
    "thread_create_with_message": {
        "dcc_rest_create_thread_from_message",
        "dcc_rest_create_thread_from_message_params",
    },
    "thread_edit": {
        "dcc_rest_modify_thread",
        "dcc_rest_modify_thread_params",
    },
    "thread_get": {"dcc_rest_get_thread"},
    "thread_member_add": {"dcc_rest_add_thread_member", "dcc_rest_add_thread_member_params"},
    "thread_member_get": {"dcc_rest_get_thread_member"},
    "thread_member_remove": {"dcc_rest_remove_thread_member"},
    "thread_members_get": {"dcc_rest_get_thread_members"},
    "threads_get_active": {"dcc_rest_get_active_threads"},
    "threads_get_joined_private_archived": {
        "dcc_rest_get_joined_private_archived_threads",
        "dcc_rest_get_joined_private_archived_threads_page",
    },
    "threads_get_private_archived": {
        "dcc_rest_get_private_archived_threads",
        "dcc_rest_get_private_archived_threads_page",
    },
    "threads_get_public_archived": {
        "dcc_rest_get_public_archived_threads",
        "dcc_rest_get_public_archived_threads_page",
    },
    "user_application_role_connection_get": {"dcc_rest_get_current_user_application_role_connection"},
    "user_application_role_connection_update": {
        "dcc_rest_update_current_user_application_role_connection",
        "dcc_rest_update_current_user_application_role_connection_params",
    },
    "user_get": {"dcc_rest_get_user"},
    "user_get_voice_state": {"dcc_rest_get_user_voice_state"},
    "user_set_voice_state": {"dcc_rest_modify_user_voice_state", "dcc_rest_modify_user_voice_state_params"},
}


def parse_dpp_cluster_methods(cluster_dir: Path) -> dict[str, str]:
    methods: dict[str, str] = {}
    for path in sorted(cluster_dir.glob("*.cpp")):
        text = path.read_text()
        for method in re.findall(r"\bcluster::([A-Za-z0-9_]+)\s*\(", text):
            methods.setdefault(method, path.name)
    return methods


def read_rest_public_headers() -> str:
    parts = [REST_H.read_text()]
    if REST_HEADER_DIR.is_dir():
        parts.extend(path.read_text() for path in sorted(REST_HEADER_DIR.rglob("*.h")))
    return "\n".join(parts)


def parse_dcc_header() -> set[str]:
    text = read_rest_public_headers()
    return set(re.findall(r"DCC_API\s+dcc_status_t\s+(dcc_rest_[A-Za-z0-9_]+)\s*\(", text))


def parse_dcc_implementation() -> set[str]:
    implementation: set[str] = set()
    for path in sorted(REST_SOURCE_DIR.glob("*.c")):
        text = path.read_text()
        implementation.update(re.findall(r"^dcc_status_t\s+(dcc_rest_[A-Za-z0-9_]+)\s*\(", text, re.M))
    return implementation


def parse_smoke_calls() -> set[str]:
    parts = [HTTP_SMOKE.read_text()]
    support_dir = ROOT / "tests" / "support"
    if support_dir.is_dir():
        parts.extend(
            path.read_text()
            for path in sorted(support_dir.glob(HTTP_SMOKE_SUPPORT_GLOB))
        )
    text = "\n".join(parts)
    return set(re.findall(r"\b(dcc_rest_[A-Za-z0-9_]+)\s*\(", text))


def main() -> int:
    errors: list[str] = []
    header = parse_dcc_header()
    implementation = parse_dcc_implementation()
    smoke = parse_smoke_calls()

    for dpp_method, dcc_wrappers in sorted(DPP_TO_DCC.items()):
        for wrapper in sorted(dcc_wrappers):
            if wrapper not in header:
                errors.append(f"{dpp_method}: {wrapper} missing from include/dcc/rest public headers")
            if wrapper not in implementation:
                errors.append(f"{dpp_method}: {wrapper} missing from src/rest/*.c")
            if wrapper not in smoke:
                errors.append(f"{dpp_method}: {wrapper} missing from tests/http_smoke.c")

    for candidate in DPP_SOURCE_CANDIDATES:
        if candidate.exists():
            cluster_files = {path.name for path in candidate.glob("*.cpp")}
            unclassified_files = sorted(cluster_files - DPP_REST_CLUSTER_FILES - DPP_CLUSTER_NON_REST_FILES)
            if unclassified_files:
                errors.append(
                    "DPP cluster source file(s) not classified in REST audit: "
                    f"{', '.join(unclassified_files)}"
                )

            dpp_methods = parse_dpp_cluster_methods(candidate)
            missing = sorted(set(DPP_TO_DCC) - set(dpp_methods))
            if missing:
                errors.append(
                    "tracked DPP REST method(s) missing from "
                    f"{candidate}: {', '.join(missing)}"
                )
            dpp_rest_methods = {
                method
                for method, source_file in dpp_methods.items()
                if source_file in DPP_REST_CLUSTER_FILES
            }
            unclassified = sorted(
                dpp_rest_methods - set(DPP_TO_DCC) - DPP_REST_NON_ENDPOINT_METHODS
            )
            if unclassified:
                errors.append(
                    "DPP REST cluster method(s) not classified in audit: "
                    f"{', '.join(unclassified)}"
                )
            break

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(
        "rest surface audit passed: "
        f"{len(DPP_TO_DCC)} DPP methods mapped to "
        f"{sum(len(v) for v in DPP_TO_DCC.values())} DCC wrappers; "
        f"{len(DPP_REST_NON_ENDPOINT_METHODS)} DPP cache/helper method classified; "
        f"{len(DPP_CLUSTER_NON_REST_FILES)} non-REST cluster source files classified"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
