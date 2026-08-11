#!/usr/bin/env python3
"""Validate the DCC 2 REST endpoint manifest and migration state."""

from __future__ import annotations

import argparse
from collections import Counter, defaultdict
from dataclasses import dataclass
import json
from pathlib import Path
import re
import sys
import tempfile
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MANIFEST = ROOT / "tools/rest_v2_endpoints.json"
ROUTE_DEFINITION_HEADER = "src/internal/rest/dcc_rest_endpoint_routes_internal.h"

TOP_LEVEL_FIELDS = frozenset({
    "schema_version", "generic_operations", "transition_helpers",
    "route_catalog", "endpoints", "transition_composites", "removed_candidates",
})
ENTRY_REQUIRED_FIELDS = frozenset({
    "canonical", "canonical_state", "header", "domain", "task", "method",
    "route_formatter", "route_evidence", "method_evidence", "input",
    "canonical_parameters",
    "source", "multipart", "auth_policy", "audit_log_reason", "sensitive_path",
    "legacy_symbols", "legacy_owners",
})
ENTRY_OPTIONAL_FIELDS = frozenset({
    "collision_justification", "deprecated", "opaque_payload",
    "transition_declarations",
})
ENTRY_FIELDS = ENTRY_REQUIRED_FIELDS | ENTRY_OPTIONAL_FIELDS
ROUTE_FIELDS = frozenset({"templates", "methods", "source_tokens"})
HELPER_FIELDS = frozenset({"symbol", "header", "source", "removal_task"})
COMPOSITE_FIELDS = frozenset({
    "name", "symbols", "owners", "removal_task", "composed_endpoints",
})
OWNER_FIELDS = frozenset({"header", "source"})
REMOVED_CANDIDATE_FIELDS = frozenset({
    "canonical", "domain", "inventory_task", "legacy_symbols", "owners",
    "reason", "removal_task",
})
EVIDENCE_PHASES = frozenset({"baseline", "canonical"})
METHODS = frozenset({"GET", "POST", "PUT", "PATCH", "DELETE"})
DOMAIN_TASKS = {
    "messages": 6,
    "interactions": 6,
    "webhooks": 6,
    "channels": 7,
    "threads": 7,
    "invites": 7,
    "guilds": 8,
    "guild_members": 8,
    "roles": 8,
    "application_commands": 9,
    "auto_moderation": 9,
    "emojis_stickers": 9,
    "message_search": 9,
    "scheduled_events": 9,
    "stage_instances": 9,
    "onboarding": 9,
    "templates": 9,
    "applications": 10,
    "direct_messages": 10,
    "entitlements": 10,
    "gateway": 10,
    "group_dms": 10,
    "lobbies": 10,
    "role_connections": 10,
    "skus": 10,
    "soundboard": 10,
    "users": 10,
    "voice_states": 10,
    "miscellaneous": 10,
}
EXPECTED_DOMAIN_COUNTS = {
    "messages": 21,
    "interactions": 8,
    "webhooks": 12,
    "channels": 12,
    "threads": 13,
    "invites": 10,
    "guilds": 28,
    "guild_members": 12,
    "roles": 7,
    "application_commands": 20,
    "auto_moderation": 5,
    "emojis_stickers": 13,
    "message_search": 1,
    "onboarding": 2,
    "scheduled_events": 6,
    "stage_instances": 4,
    "templates": 6,
    "applications": 3,
    "entitlements": 6,
    "gateway": 2,
    "group_dms": 2,
    "lobbies": 13,
    "role_connections": 5,
    "skus": 2,
    "soundboard": 2,
    "users": 4,
    "voice_states": 5,
}
IDENTIFIER = re.compile(r"[A-Za-z_][A-Za-z0-9_]*\Z")
PUBLIC_DECLARATION = re.compile(
    r"\bDCC_API\b\s+(?P<return_type>[^;{}()]*?)\b"
    r"(?P<name>dcc_rest_[A-Za-z0-9_]+)\s*"
    r"\((?P<args>.*?)\)\s*;",
    re.S,
)
EXTERNAL_DEFINITION = re.compile(
    r"^(?!\s*static\b)(?P<return_type>[A-Za-z_][A-Za-z0-9_\s*]*?)\b"
    r"(?P<name>dcc_rest_[A-Za-z0-9_]+)\s*\((?P<args>.*?)\)\s*\{",
    re.M | re.S,
)
CALL_TAIL = re.compile(
    r"const\s+dcc_rest_call_options_t\s*\*\s*options\s*,\s*"
    r"dcc_rest_request_t\s*\*\s*\*\s*out_request\s*\Z",
)
RAW_ENDPOINT_INPUT = re.compile(
    r"const\s+(?:char|void)\s*\*\s*"
    r"(?:json|json_body|raw_json|payload_json|json_payload|body|body_json|"
    r"request_body|content_json|request_json|payload|data|query)\b"
)
FORBIDDEN_CANONICAL_SUFFIXES = (
    "_async",
    "_sync",
    "_future",
    "_builder",
    "_multipart",
    "_params",
    "_options",
    "_page",
    "_with_token",
    "_from_interaction",
    "_text",
    "_raw",
    "_raw_json",
    "_json",
    "_json_body",
    "_body",
    "_body_json",
    "_payload",
    "_payload_json",
    "_callback",
    "_with_callback",
    "_with_cb",
    "_cb",
)
SUFFIX_CANONICAL_EXCEPTIONS = frozenset({
    # This is Discord's distinct /widget.json representation endpoint, not a
    # raw-JSON overload of dcc_rest_get_guild_widget.
    "dcc_rest_get_guild_widget_json",
})
THREAD_CANONICALS = frozenset({
    "dcc_rest_create_forum_thread",
    "dcc_rest_create_thread",
    "dcc_rest_join_thread",
    "dcc_rest_leave_thread",
})

# This is intentionally immutable. These are generic request/result/firewall
# facilities, not Discord endpoints. dcc_rest_official_body_json_free is the
# sole official-surface entry here because it only releases memory returned by
# endpoint-specific body builders; the thirteen builders themselves belong to
# their endpoint entries.
GENERIC_OPERATIONS = (
    "dcc_rest_async_cancel_active",
    "dcc_rest_async_cancel_all",
    "dcc_rest_async_cancel_pending",
    "dcc_rest_async_pending",
    "dcc_rest_async_status",
    "dcc_rest_async_wait",
    "dcc_rest_call_options_init",
    "dcc_rest_firewall_attach",
    "dcc_rest_firewall_check",
    "dcc_rest_firewall_decision_string",
    "dcc_rest_firewall_deinit",
    "dcc_rest_firewall_detach",
    "dcc_rest_firewall_hard_limit_action_string",
    "dcc_rest_firewall_init",
    "dcc_rest_firewall_options_init",
    "dcc_rest_firewall_options_validate",
    "dcc_rest_firewall_reason_string",
    "dcc_rest_firewall_record_response",
    "dcc_rest_firewall_request_is_critical",
    "dcc_rest_firewall_snapshot",
    "dcc_rest_firewall_snapshot_json",
    "dcc_rest_firewall_state_snapshot",
    "dcc_rest_firewall_status_is_invalid",
    "dcc_rest_future_completed",
    "dcc_rest_future_destroy",
    "dcc_rest_future_wait",
    "dcc_rest_method_name",
    "dcc_rest_request",
    "dcc_rest_request_async",
    "dcc_rest_request_async_priority",
    "dcc_rest_request_cancel",
    "dcc_rest_request_completed",
    "dcc_rest_request_desc_init",
    "dcc_rest_request_destroy",
    "dcc_rest_request_future",
    "dcc_rest_request_future_priority",
    "dcc_rest_request_method",
    "dcc_rest_request_method_async",
    "dcc_rest_request_method_async_priority",
    "dcc_rest_request_method_future",
    "dcc_rest_request_method_future_priority",
    "dcc_rest_request_method_multipart",
    "dcc_rest_request_method_raw",
    "dcc_rest_request_multipart",
    "dcc_rest_request_raw",
    "dcc_rest_request_wait",
    "dcc_rest_response_bool_field",
    "dcc_rest_response_channel_id",
    "dcc_rest_response_guild_id",
    "dcc_rest_response_i64_field",
    "dcc_rest_response_message_id",
    "dcc_rest_response_snowflake_field",
    "dcc_rest_response_string_field",
    "dcc_rest_response_string_field_len",
    "dcc_rest_response_u64_field",
    "dcc_rest_result_clone",
    "dcc_rest_result_free",
    "dcc_rest_result_ok",
    "dcc_rest_result_status",
    "dcc_rest_submit",
)
TRANSITION_HELPER = {
    "symbol": "dcc_rest_official_body_json_free",
    "header": "include/dcc/rest/official_surface.h",
    "source": "src/rest/rest_official_surface.c",
    "removal_task": 10,
}
PLANNED_CANONICALS = frozenset({
    "dcc_rest_create_forum_thread",
    "dcc_rest_create_guild_sticker",
})
BASELINE_IDENTIFIER_COUNTS = {6: 107, 7: 53, 8: 78, 9: 87, 10: 72}
BASELINE_CURRENT_COUNTS = {6: 107, 7: 52, 8: 78, 9: 86, 10: 72}
REMOVED_CANDIDATE_SPECS = {
    "dcc_rest_bulk_edit_guild_command_permissions": {
        "domain": "application_commands",
        "inventory_task": 9,
        "legacy_symbols": ("dcc_rest_bulk_edit_guild_command_permissions_params",),
        "owners": {
            "dcc_rest_bulk_edit_guild_command_permissions": {
                "header": "include/dcc/rest/application_commands/permissions.h",
                "source": "src/rest/rest_application_command_permissions.c",
            },
            "dcc_rest_bulk_edit_guild_command_permissions_params": {
                "header": "include/dcc/rest/application_commands/permissions.h",
                "source": "src/rest/rest_application_command_permissions_params.c",
            },
        },
        "reason": "disabled bulk guild command permission edit",
    },
    "dcc_rest_create_guild": {
        "domain": "guilds",
        "inventory_task": 8,
        "legacy_symbols": ("dcc_rest_create_guild_params",),
        "owners": {
            "dcc_rest_create_guild": {
                "header": "include/dcc/rest/guilds/core.h",
                "source": "src/rest/rest_guilds_core.c",
            },
            "dcc_rest_create_guild_params": {
                "header": "include/dcc/rest/guilds/core.h",
                "source": "src/rest/rest_guilds_core.c",
            },
        },
        "reason": "unavailable Discord operation",
    },
    "dcc_rest_create_guild_from_template": {
        "domain": "templates",
        "inventory_task": 9,
        "legacy_symbols": ("dcc_rest_create_guild_from_template_params",),
        "owners": {
            "dcc_rest_create_guild_from_template": {
                "header": "include/dcc/rest/resources/templates.h",
                "source": "src/rest/rest_templates_create.c",
            },
            "dcc_rest_create_guild_from_template_params": {
                "header": "include/dcc/rest/resources/templates.h",
                "source": "src/rest/rest_templates_create.c",
            },
        },
        "reason": "unavailable guild creation from template",
    },
    "dcc_rest_delete_guild": {
        "domain": "guilds",
        "inventory_task": 8,
        "legacy_symbols": (),
        "owners": {
            "dcc_rest_delete_guild": {
                "header": "include/dcc/rest/guilds/core.h",
                "source": "src/rest/rest_guilds_core.c",
            },
        },
        "reason": "unavailable Discord operation",
    },
    "dcc_rest_get_current_user_dms": {
        "domain": "direct_messages",
        "inventory_task": 10,
        "legacy_symbols": (),
        "owners": {
            "dcc_rest_get_current_user_dms": {
                "header": "include/dcc/rest/resources/users.h",
                "source": "src/rest/rest_users_dms.c",
            },
        },
        "reason": "obsolete bot-facing current-user DM listing",
    },
    "dcc_rest_modify_guild_integration": {
        "domain": "guilds",
        "inventory_task": 8,
        "legacy_symbols": (),
        "owners": {
            "dcc_rest_modify_guild_integration": {
                "header": "include/dcc/rest/guilds/integrations.h",
                "source": "src/rest/rest_guilds_integrations.c",
            },
        },
        "reason": "unavailable Discord operation",
    },
    "dcc_rest_sync_guild_integration": {
        "domain": "guilds",
        "inventory_task": 8,
        "legacy_symbols": (),
        "owners": {
            "dcc_rest_sync_guild_integration": {
                "header": "include/dcc/rest/guilds/integrations.h",
                "source": "src/rest/rest_guilds_integrations.c",
            },
        },
        "reason": "unavailable Discord operation",
    },
}
TRANSITION_DECLARATIONS = {
    "dcc_rest_get_current_application": (
        "include/dcc/rest/official_surface.h",
    ),
}
BODY_BUILDERS = {
    "dcc_rest_build_application_modify_body": "dcc_rest_modify_current_application",
    "dcc_rest_build_bulk_ban_body": "dcc_rest_bulk_ban_guild_members",
    "dcc_rest_build_guild_incident_actions_body": "dcc_rest_modify_guild_incident_actions",
    "dcc_rest_build_guild_soundboard_sound_create_body": "dcc_rest_create_guild_soundboard_sound",
    "dcc_rest_build_guild_soundboard_sound_modify_body": "dcc_rest_modify_guild_soundboard_sound",
    "dcc_rest_build_lobby_body": "dcc_rest_create_lobby",
    "dcc_rest_build_lobby_channel_link_body": "dcc_rest_link_lobby_channel",
    "dcc_rest_build_lobby_create_or_join_body": "dcc_rest_create_or_join_lobby",
    "dcc_rest_build_lobby_member_body": "dcc_rest_add_lobby_member",
    "dcc_rest_build_lobby_member_bulk_body": "dcc_rest_bulk_update_lobby_members",
    "dcc_rest_build_lobby_message_body": "dcc_rest_send_lobby_message",
    "dcc_rest_build_lobby_message_moderation_metadata_body": "dcc_rest_update_lobby_message_moderation_metadata",
    "dcc_rest_build_soundboard_send_body": "dcc_rest_send_soundboard_sound",
}
REVIEWED_COLLISIONS = {
    ("POST", "/webhooks/{}/{}"): (
        frozenset({"dcc_rest_execute_webhook", "dcc_rest_interaction_followup_create"}),
        "Discord webhook execution and interaction followup creation share the tokenized webhook route.",
    ),
    ("GET", "/webhooks/{}/{}/messages/{}"): (
        frozenset({"dcc_rest_get_webhook_message", "dcc_rest_interaction_followup_get"}),
        "Discord webhook messages and interaction followups share the tokenized webhook-message route.",
    ),
    ("PATCH", "/webhooks/{}/{}/messages/{}"): (
        frozenset({"dcc_rest_modify_webhook_message", "dcc_rest_interaction_followup_edit"}),
        "Discord webhook messages and interaction followups share the tokenized webhook-message route.",
    ),
    ("DELETE", "/webhooks/{}/{}/messages/{}"): (
        frozenset({"dcc_rest_delete_webhook_message", "dcc_rest_interaction_followup_delete"}),
        "Discord webhook messages and interaction followups share the tokenized webhook-message route.",
    ),
    ("POST", "/channels/{}/threads"): (
        frozenset({"dcc_rest_create_forum_thread", "dcc_rest_create_thread"}),
        "Discord ordinary-thread and forum-thread creation share the channel-thread route with distinct payload semantics.",
    ),
}
METHOD_TOKENS = {
    "DCC_REST_GET": "GET",
    "DCC_REST_POST": "POST",
    "DCC_REST_PUT": "PUT",
    "DCC_REST_PATCH": "PATCH",
    "DCC_REST_DELETE": "DELETE",
    '"GET"': "GET",
    '"POST"': "POST",
    '"PUT"': "PUT",
    '"PATCH"': "PATCH",
    '"DELETE"': "DELETE",
}
EXPECTED_COMPOSITES = {
    "application_command_bulk_overwrite_dispatch": {
        "symbols": (
            "dcc_rest_bulk_overwrite_application_commands",
            "dcc_rest_bulk_overwrite_application_commands_builder",
        ),
        "header": "include/dcc/rest/application_commands/registration.h",
        "source": "src/rest/rest_application_commands_registration.c",
        "removal_task": 9,
        "composed_endpoints": (
            "dcc_rest_bulk_overwrite_global_commands",
            "dcc_rest_bulk_overwrite_guild_commands",
        ),
    },
    "application_command_create_dispatch": {
        "symbols": (
            "dcc_rest_create_application_command",
            "dcc_rest_create_application_command_builder",
        ),
        "header": "include/dcc/rest/application_commands/registration.h",
        "source": "src/rest/rest_application_commands_registration.c",
        "removal_task": 9,
        "composed_endpoints": (
            "dcc_rest_create_global_command",
            "dcc_rest_create_guild_command",
        ),
    },
    "application_command_delete_all_dispatch": {
        "symbols": ("dcc_rest_delete_all_application_commands",),
        "header": "include/dcc/rest/application_commands/registration.h",
        "source": "src/rest/rest_application_commands_registration.c",
        "removal_task": 9,
        "composed_endpoints": (
            "dcc_rest_bulk_overwrite_global_commands",
            "dcc_rest_bulk_overwrite_guild_commands",
        ),
    },
    "direct_message_create_and_send": {
        "symbols": (
            "dcc_rest_create_direct_message",
            "dcc_rest_create_direct_message_builder",
        ),
        "header": "include/dcc/rest/resources/direct_messages.h",
        "source": "src/rest/rest_direct_messages.c",
        "removal_task": 10,
        "composed_endpoints": (
            "dcc_rest_create_dm_channel",
            "dcc_rest_create_message",
        ),
    },
}
OPAQUE_PAYLOAD_ENDPOINTS = frozenset({
    "dcc_rest_execute_webhook_github",
    "dcc_rest_execute_webhook_slack",
})
SCALAR_ONLY_ENDPOINTS = frozenset({
    # These legacy records contain only the same IDs already required by the
    # route. The Discord operations have no request body.
    "dcc_rest_add_guild_member_role",
    "dcc_rest_add_thread_member",
})
TASK6_MULTIPART_ENDPOINTS = frozenset({
    "dcc_rest_create_message",
    "dcc_rest_edit_message",
    "dcc_rest_execute_webhook",
    "dcc_rest_interaction_followup_create",
    "dcc_rest_interaction_followup_edit",
    "dcc_rest_interaction_original_response_edit",
    "dcc_rest_interaction_response_create",
    "dcc_rest_modify_webhook_message",
})
TASK7_MULTIPART_ENDPOINTS = frozenset({
    "dcc_rest_create_channel_invite",
    "dcc_rest_create_forum_thread",
    "dcc_rest_put_invite_target_users",
})
MULTIPART_ENDPOINTS = (
    TASK6_MULTIPART_ENDPOINTS
    | TASK7_MULTIPART_ENDPOINTS
    | frozenset({"dcc_rest_create_guild_sticker"})
)
TASK7_LEGACY_SYMBOLS = frozenset({
    "dcc_rest_add_thread_member_params",
    "dcc_rest_create_channel_invite_params",
    "dcc_rest_create_dm_channel_params",
    "dcc_rest_create_forum_thread_params",
    "dcc_rest_create_guild_channel_params",
    "dcc_rest_create_thread_params",
    "dcc_rest_create_thread_from_message_params",
    "dcc_rest_get_invite_full",
    "dcc_rest_get_joined_private_archived_threads_page",
    "dcc_rest_get_private_archived_threads_page",
    "dcc_rest_get_public_archived_threads_page",
    "dcc_rest_get_thread",
    "dcc_rest_modify_channel_params",
    "dcc_rest_modify_channel_permission_params",
    "dcc_rest_modify_guild_channel_positions_params",
    "dcc_rest_modify_thread",
    "dcc_rest_modify_thread_params",
    "dcc_rest_set_channel_voice_status_params",
})
AUDIT_REASON_ENDPOINTS = frozenset({
    # Task 6
    "dcc_rest_bulk_delete_messages",
    "dcc_rest_create_webhook",
    "dcc_rest_delete_message",
    "dcc_rest_delete_webhook",
    "dcc_rest_legacy_pin_message",
    "dcc_rest_legacy_unpin_message",
    "dcc_rest_modify_webhook",
    "dcc_rest_pin_message",
    "dcc_rest_unpin_message",
    # Task 7
    "dcc_rest_create_channel_invite",
    "dcc_rest_create_forum_thread",
    "dcc_rest_create_guild_channel",
    "dcc_rest_create_thread",
    "dcc_rest_create_thread_from_message",
    "dcc_rest_delete_channel",
    "dcc_rest_delete_channel_permission",
    "dcc_rest_delete_invite",
    "dcc_rest_follow_news_channel",
    "dcc_rest_modify_channel",
    "dcc_rest_modify_channel_permission",
    "dcc_rest_set_channel_voice_status",
    # Task 8
    "dcc_rest_add_guild_member_role",
    "dcc_rest_begin_guild_prune",
    "dcc_rest_bulk_ban_guild_members",
    "dcc_rest_create_guild_ban",
    "dcc_rest_create_guild_role",
    "dcc_rest_create_guild_soundboard_sound",
    "dcc_rest_delete_guild_ban",
    "dcc_rest_delete_guild_integration",
    "dcc_rest_delete_guild_role",
    "dcc_rest_delete_guild_soundboard_sound",
    "dcc_rest_modify_current_guild_member",
    "dcc_rest_modify_guild",
    "dcc_rest_modify_guild_member",
    "dcc_rest_modify_guild_role",
    "dcc_rest_modify_guild_role_positions",
    "dcc_rest_modify_guild_soundboard_sound",
    "dcc_rest_modify_guild_welcome_screen",
    "dcc_rest_modify_guild_widget",
    "dcc_rest_remove_guild_member",
    "dcc_rest_remove_guild_member_role",
    "dcc_rest_set_current_guild_member_nickname",
    # Task 9
    "dcc_rest_create_auto_moderation_rule",
    "dcc_rest_create_guild_emoji",
    "dcc_rest_create_guild_scheduled_event",
    "dcc_rest_create_guild_sticker",
    "dcc_rest_create_stage_instance",
    "dcc_rest_delete_auto_moderation_rule",
    "dcc_rest_delete_guild_emoji",
    "dcc_rest_delete_guild_sticker",
    "dcc_rest_delete_stage_instance",
    "dcc_rest_modify_auto_moderation_rule",
    "dcc_rest_modify_guild_emoji",
    "dcc_rest_modify_guild_onboarding",
    "dcc_rest_modify_guild_scheduled_event",
    "dcc_rest_modify_guild_sticker",
    "dcc_rest_modify_stage_instance",
})
AUTH_POLICIES = frozenset({
    "BOT", "NONE", "BEARER", "BOT_OR_BEARER", "NONE_OR_BOT",
    "WEBHOOK_TOKEN_OR_BOT",
})
AUTH_NONE_ENDPOINTS = frozenset({
    "dcc_rest_delete_webhook_message",
    "dcc_rest_execute_webhook",
    "dcc_rest_execute_webhook_github",
    "dcc_rest_execute_webhook_slack",
    "dcc_rest_get_webhook_message",
    "dcc_rest_modify_webhook_message",
    "dcc_rest_interaction_followup_create",
    "dcc_rest_interaction_followup_delete",
    "dcc_rest_interaction_followup_edit",
    "dcc_rest_interaction_followup_get",
    "dcc_rest_interaction_original_response_delete",
    "dcc_rest_interaction_original_response_edit",
    "dcc_rest_interaction_original_response_get",
    "dcc_rest_interaction_response_create",
    "dcc_rest_get_guild_widget_json",
    "dcc_rest_get_guild_widget_png",
    "dcc_rest_get_gateway",
})
AUTH_BEARER_ENDPOINTS = frozenset({
    "dcc_rest_create_lobby_channel_invite_for_self",
    "dcc_rest_get_current_user_guild_member",
    "dcc_rest_edit_guild_command_permissions",
    "dcc_rest_add_group_dm_recipient",
    "dcc_rest_remove_group_dm_recipient",
    "dcc_rest_create_or_join_lobby",
    "dcc_rest_get_lobby_messages",
    "dcc_rest_leave_lobby",
    "dcc_rest_link_lobby_channel",
    "dcc_rest_send_lobby_message",
    "dcc_rest_delete_current_user_application_role_connection",
    "dcc_rest_get_current_user_application_role_connection",
    "dcc_rest_update_current_user_application_role_connection",
    "dcc_rest_get_current_user_connections",
})
AUTH_BOT_OR_BEARER_ENDPOINTS = frozenset({
    "dcc_rest_get_current_user_guilds",
    "dcc_rest_bulk_overwrite_global_commands",
    "dcc_rest_bulk_overwrite_guild_commands",
    "dcc_rest_create_global_command",
    "dcc_rest_create_guild_command",
    "dcc_rest_delete_global_command",
    "dcc_rest_delete_guild_command",
    "dcc_rest_edit_global_command",
    "dcc_rest_edit_guild_command",
    "dcc_rest_get_all_guild_command_permissions",
    "dcc_rest_get_global_command",
    "dcc_rest_get_global_commands",
    "dcc_rest_get_guild_command",
    "dcc_rest_get_guild_command_permissions",
    "dcc_rest_get_guild_commands",
    "dcc_rest_get_sku_subscription",
    "dcc_rest_get_sku_subscriptions",
    "dcc_rest_get_current_user",
})
AUTH_NONE_OR_BOT_ENDPOINTS = frozenset({"dcc_rest_get_invite"})
AUTH_WEBHOOK_TOKEN_OR_BOT_ENDPOINTS = frozenset({
    "dcc_rest_delete_webhook",
    "dcc_rest_get_webhook",
    "dcc_rest_modify_webhook",
})
SENSITIVE_PATH_ENDPOINTS = frozenset({
    "dcc_rest_delete_webhook",
    "dcc_rest_delete_webhook_message",
    "dcc_rest_execute_webhook",
    "dcc_rest_execute_webhook_github",
    "dcc_rest_execute_webhook_slack",
    "dcc_rest_get_webhook",
    "dcc_rest_get_webhook_message",
    "dcc_rest_interaction_followup_create",
    "dcc_rest_interaction_followup_delete",
    "dcc_rest_interaction_followup_edit",
    "dcc_rest_interaction_followup_get",
    "dcc_rest_interaction_original_response_delete",
    "dcc_rest_interaction_original_response_edit",
    "dcc_rest_interaction_original_response_get",
    "dcc_rest_interaction_response_create",
    "dcc_rest_modify_webhook",
    "dcc_rest_modify_webhook_message",
})
EXPECTED_AUTH_COUNTS = {
    6: {"BOT": 24, "NONE": 14, "WEBHOOK_TOKEN_OR_BOT": 3},
    7: {"BEARER": 1, "BOT": 33, "NONE_OR_BOT": 1},
    8: {"BEARER": 1, "BOT": 43, "BOT_OR_BEARER": 1, "NONE": 2},
    9: {"BEARER": 1, "BOT": 42, "BOT_OR_BEARER": 14},
    10: {"BEARER": 11, "BOT": 29, "BOT_OR_BEARER": 3, "NONE": 1},
}
EXPECTED_AUDIT_COUNTS = {6: 9, 7: 12, 8: 21, 9: 15, 10: 0}
EXPECTED_MULTIPART_COUNTS = {6: 8, 7: 3, 8: 0, 9: 1, 10: 0}
REVIEWED_ENDPOINT_INPUTS = {
    "dcc_rest_get_message_reactions": "dcc_rest_reaction_query_t",
    "dcc_rest_get_thread_member": "dcc_rest_thread_member_query_t",
    "dcc_rest_get_thread_members": "dcc_rest_thread_member_query_t",
}


def expected_auth_policy(canonical: str) -> str:
    if canonical in AUTH_NONE_ENDPOINTS:
        return "NONE"
    if canonical in AUTH_BEARER_ENDPOINTS:
        return "BEARER"
    if canonical in AUTH_BOT_OR_BEARER_ENDPOINTS:
        return "BOT_OR_BEARER"
    if canonical in AUTH_NONE_OR_BOT_ENDPOINTS:
        return "NONE_OR_BOT"
    if canonical in AUTH_WEBHOOK_TOKEN_OR_BOT_ENDPOINTS:
        return "WEBHOOK_TOKEN_OR_BOT"
    return "BOT"


def capability_evidence_tokens(entry: dict[str, Any]) -> tuple[str, ...]:
    policy = entry["auth_policy"]
    auth = f"DCC_ENDPOINT_AUTH_POLICY_{policy}"
    audit = (
        "DCC_ENDPOINT_AUDIT_REASON_ALLOWED"
        if entry["audit_log_reason"]
        else "DCC_ENDPOINT_AUDIT_REASON_DENIED"
    )
    path = (
        "DCC_ENDPOINT_PATH_SENSITIVE"
        if entry["sensitive_path"]
        else "DCC_ENDPOINT_PATH_PUBLIC"
    )
    if entry["sensitive_path"]:
        return (
            auth,
            audit,
            path,
            "DCC_ENDPOINT_ROUTE_KEY_OPAQUE",
            json.dumps(entry["canonical"]),
        )
    return auth, audit, path


@dataclass(frozen=True)
class Declaration:
    name: str
    return_type: str
    args: str
    path: str


@dataclass(frozen=True)
class Definition:
    name: str
    return_type: str
    args: str
    path: str
    body: str


@dataclass(frozen=True)
class CToken:
    kind: str
    value: str
    start: int
    end: int


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=ROOT)
    parser.add_argument("--manifest", type=Path)
    parser.add_argument("--progress-through", type=int, choices=range(6, 11))
    parser.add_argument("--self-test", action="store_true")
    return parser.parse_args()


def normalized_args(value: str) -> str:
    return " ".join(value.split())


C_TOKEN = re.compile(
    r"""
    (?P<preprocessor>^[ \t]*\#(?:\\\r?\n|[^\r\n])*)
  | (?P<whitespace>[ \t\v\f]+|\r\n|\r|\n)
  | (?P<line_comment>//[^\n]*(?:\n|\Z))
  | (?P<block_comment>/\*.*?\*/)
  | (?P<string>(?:u8|u|U|L)?"(?:\\.|[^"\\])*")
  | (?P<character>(?:u8|u|U|L)?'(?:\\.|[^'\\])*')
  | (?P<identifier>[A-Za-z_][A-Za-z0-9_]*)
  | (?P<number>(?:0[xX][0-9A-Fa-f]+|0[bB][01]+|(?:\d+(?:\.\d*)?|\.\d+))(?:[EePp][+-]?\d+)?[A-Za-z0-9_]*)
  | (?P<punctuation>\#\#|->|\+\+|--|<<=?|>>=?|<=|>=|==|!=|&&|\|\||\+=|-=|\*=|/=|%=|&=|\|=|\^=|\.\.\.|.)
    """,
    re.M | re.S | re.X,
)


def c_condition_static_truth(condition: str) -> bool | None:
    """Evaluate a literal-only C preprocessor integer expression.

    Identifiers and unsupported operators deliberately produce ``None``: both
    branches must remain visible when the active branch depends on the build.
    """
    condition = re.sub(r"/\*.*?\*/|//[^\r\n]*", " ", condition, flags=re.S)
    condition = re.sub(r"\\\r?\n", " ", condition).strip()
    token_pattern = re.compile(
        r"\s*(?:"
        r"(?P<integer>(?:0[xX][0-9A-Fa-f]+|0[bB][01]+|0[0-7]*|[1-9][0-9]*)(?:[uUlL]*))"
        r"|(?P<operator>\&\&|\|\||==|!=|<=|>=|[!<>()+\-]))"
    )
    tokens: list[tuple[str, str]] = []
    offset = 0
    while offset < len(condition):
        match = token_pattern.match(condition, offset)
        if match is None:
            return None
        kind = "integer" if match.group("integer") is not None else "operator"
        tokens.append((kind, match.group(kind)))
        offset = match.end()
    if not tokens:
        return None

    def integer_value(literal: str) -> int:
        number = re.sub(r"[uUlL]+\Z", "", literal)
        if number.lower().startswith("0x"):
            return int(number[2:], 16)
        if number.lower().startswith("0b"):
            return int(number[2:], 2)
        if len(number) > 1 and number.startswith("0"):
            return int(number, 8)
        return int(number, 10)

    index = 0

    def primary() -> int | None:
        nonlocal index
        if index >= len(tokens):
            return None
        kind, value = tokens[index]
        if kind == "integer":
            index += 1
            return integer_value(value)
        if value != "(":
            return None
        index += 1
        result = logical_or()
        if result is None or index >= len(tokens) or tokens[index][1] != ")":
            return None
        index += 1
        return result

    def unary() -> int | None:
        nonlocal index
        if index < len(tokens) and tokens[index][1] in {"!", "+", "-"}:
            operator = tokens[index][1]
            index += 1
            value = unary()
            if value is None:
                return None
            if operator == "!":
                return int(not value)
            return value if operator == "+" else -value
        return primary()

    def comparison() -> int | None:
        nonlocal index
        left = unary()
        if left is None:
            return None
        operations = {
            "==": lambda lhs, rhs: lhs == rhs,
            "!=": lambda lhs, rhs: lhs != rhs,
            "<": lambda lhs, rhs: lhs < rhs,
            "<=": lambda lhs, rhs: lhs <= rhs,
            ">": lambda lhs, rhs: lhs > rhs,
            ">=": lambda lhs, rhs: lhs >= rhs,
        }
        while index < len(tokens) and tokens[index][1] in operations:
            operator = tokens[index][1]
            index += 1
            right = unary()
            if right is None:
                return None
            left = int(operations[operator](left, right))
        return left

    def logical_and() -> int | None:
        nonlocal index
        left = comparison()
        if left is None:
            return None
        while index < len(tokens) and tokens[index][1] == "&&":
            index += 1
            right = comparison()
            if right is None:
                return None
            left = int(bool(left) and bool(right))
        return left

    def logical_or() -> int | None:
        nonlocal index
        left = logical_and()
        if left is None:
            return None
        while index < len(tokens) and tokens[index][1] == "||":
            index += 1
            right = logical_and()
            if right is None:
                return None
            left = int(bool(left) or bool(right))
        return left

    value = logical_or()
    if value is None or index != len(tokens):
        return None
    return value != 0


def c_condition_is_static_zero(condition: str) -> bool:
    return c_condition_static_truth(condition) is False


def c_inactive_conditional_mask(text: str) -> str:
    """Mask branches made unreachable by literal integer ``#if`` expressions."""
    directives = {
        match.start(): match.group(0)
        for match in C_TOKEN.finditer(text)
        if match.lastgroup == "preprocessor"
    }
    masked = list(text)
    frames: list[dict[str, bool]] = []
    offset = 0
    preserve_conditional_until = 0
    for line in text.splitlines(keepends=True):
        directive_text = directives.get(offset)
        inactive_before = frames[-1]["inactive"] if frames else False
        mask_line = inactive_before
        if offset < preserve_conditional_until:
            # The first physical line already updated the frame. Preserve the
            # remainder so applying the active-source view is idempotent.
            mask_line = False
        elif directive_text is not None:
            directive = re.match(
                r"^[ \t]*\#[ \t]*(?P<kind>if|ifdef|ifndef|elif|else|endif)\b(?P<tail>.*)\Z",
                directive_text,
                re.S,
            )
            if directive is not None:
                kind = directive.group("kind")
                tail = directive.group("tail")
                if kind in {"if", "ifdef", "ifndef"}:
                    truth = c_condition_static_truth(tail) if kind == "if" else None
                    known = truth is not None
                    taken = truth is True
                    frames.append({
                        "parent_inactive": inactive_before,
                        "known": known,
                        "taken": taken,
                        "inactive": inactive_before or truth is False,
                    })
                    mask_line = inactive_before or truth is False
                elif kind == "elif" and frames:
                    frame = frames[-1]
                    if frame["parent_inactive"]:
                        frame["inactive"] = True
                    elif frame["known"]:
                        if frame["taken"]:
                            frame["inactive"] = True
                        else:
                            truth = c_condition_static_truth(tail)
                            if truth is None:
                                frame["known"] = False
                                frame["inactive"] = False
                            else:
                                frame["inactive"] = not truth
                                frame["taken"] = truth
                    else:
                        # Unknown platform/configuration branches remain visible
                        # so discovery does not lose potentially active API.
                        frame["inactive"] = False
                elif kind == "else" and frames:
                    frame = frames[-1]
                    if frame["parent_inactive"]:
                        frame["inactive"] = True
                    elif frame["known"]:
                        frame["inactive"] = frame["taken"]
                        frame["taken"] = True
                    else:
                        frame["inactive"] = False
                elif kind == "endif" and frames:
                    frames.pop()
                preserve_conditional_until = offset + len(directive_text)
                mask_line = False
        if mask_line:
            for index in range(offset, offset + len(line)):
                if masked[index] not in {"\r", "\n"}:
                    masked[index] = " "
        offset += len(line)
    return "".join(masked)


def c_active_source(text: str) -> str:
    """Return a same-length C view with statically inactive branches masked."""
    return c_inactive_conditional_mask(text)


def c_lex_tokens(text: str) -> list[CToken]:
    """Return active semantic C tokens, excluding trivia and directives."""
    active = c_active_source(text)
    tokens: list[CToken] = []
    ignored = {"preprocessor", "whitespace", "line_comment", "block_comment"}
    for match in C_TOKEN.finditer(active):
        kind = match.lastgroup or "punctuation"
        if kind not in ignored:
            tokens.append(CToken(kind, match.group(0), match.start(), match.end()))
    return tokens


def c_discovery_mask(text: str) -> str:
    """Mask non-code C regions while preserving offsets and line boundaries."""
    inactive_masked = c_active_source(text)
    masked = list(inactive_masked)
    ignored = {"preprocessor", "line_comment", "block_comment", "string", "character"}
    for match in C_TOKEN.finditer(inactive_masked):
        if (match.lastgroup or "punctuation") not in ignored:
            continue
        for index in range(match.start(), match.end()):
            if masked[index] not in {"\r", "\n"}:
                masked[index] = " "
    return "".join(masked)


def c_active_preprocessor_source(text: str) -> str:
    """Keep active directives while masking inactive code and lexical decoys."""
    active = c_active_source(text)
    masked = list(active)
    ignored = {"line_comment", "block_comment", "string", "character"}
    for match in C_TOKEN.finditer(active):
        kind = match.lastgroup or "punctuation"
        # A directive is one token, so its route string must remain available.
        if kind == "preprocessor" or kind not in ignored:
            continue
        for index in range(match.start(), match.end()):
            if masked[index] not in {"\r", "\n"}:
                masked[index] = " "
    return "".join(masked)


def c_active_identifiers(text: str) -> set[str]:
    """Collect exact identifiers from active non-preprocessor C code."""
    return {
        token.value
        for token in c_lex_tokens(text)
        if token.kind == "identifier"
    }


def c_active_macro_definitions(text: str) -> set[str]:
    """Collect exact macro names from active ``#define`` directives."""
    active = c_active_preprocessor_source(text)
    result: set[str] = set()
    for match in C_TOKEN.finditer(active):
        if match.lastgroup != "preprocessor":
            continue
        definition = re.match(
            r"^[ \t]*\#[ \t]*define[ \t]+"
            r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\b",
            match.group(0),
        )
        if definition is not None:
            result.add(definition.group("name"))
    return result


def c_definition_body(text: str, body_start: int) -> str:
    """Slice one definition body without counting braces in C trivia/literals."""
    depth = 0
    began = False
    for token in c_lex_tokens(text[body_start:]):
        if token.kind != "punctuation" or token.value not in {"{", "}"}:
            continue
        if token.value == "{":
            depth += 1
            began = True
        elif began:
            depth -= 1
            if depth == 0:
                return text[body_start:body_start + token.end]
    return ""


def split_c_parameters(value: str) -> list[str] | None:
    """Parse a normalized C parameter list into exact top-level declarations."""
    if normalized_args(value) == "void":
        return []
    tokens = c_lex_tokens(value)
    if not tokens:
        return None
    depths = {"(": 0, "[": 0, "{": 0}
    closing = {")": "(", "]": "[", "}": "{"}
    start = 0
    result: list[str] = []
    for token in tokens:
        if token.kind != "punctuation":
            continue
        if token.value in depths:
            depths[token.value] += 1
        elif token.value in closing:
            opener = closing[token.value]
            depths[opener] -= 1
            if depths[opener] < 0:
                return None
        elif token.value == "," and all(depth == 0 for depth in depths.values()):
            parameter = normalized_args(value[start:token.start])
            if not parameter:
                return None
            result.append(parameter)
            start = token.end
    if any(depth != 0 for depth in depths.values()):
        return None
    parameter = normalized_args(value[start:])
    if not parameter:
        return None
    result.append(parameter)
    return result


def c_parameter_name(parameter: str) -> str | None:
    match = re.search(
        r"(?P<name>[A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]*\])?\s*\Z",
        parameter,
    )
    return match.group("name") if match is not None else None


def c_string_literal_contents(token: CToken) -> str | None:
    if token.kind != "string":
        return None
    quote = token.value.find('"')
    return token.value[quote + 1:-1]


def c_string_groups(tokens: list[CToken]) -> list[tuple[int, int, str]]:
    groups: list[tuple[int, int, str]] = []
    index = 0
    while index < len(tokens):
        if tokens[index].kind != "string":
            index += 1
            continue
        end = index
        pieces: list[str] = []
        while end < len(tokens) and tokens[end].kind == "string":
            pieces.append(c_string_literal_contents(tokens[end]) or "")
            end += 1
        groups.append((index, end, "".join(pieces)))
        index = end
    return groups


def c_token_is_string_macro(token: CToken) -> bool:
    return (
        token.kind == "identifier"
        and token.value == token.value.upper()
        and "_" in token.value
    )


def exact_c_evidence_present(body: str, evidence: str) -> bool:
    """Match evidence as an exact C token sequence or whole route literal."""
    body_tokens = c_lex_tokens(body)
    string_groups = c_string_groups(body_tokens)
    if evidence.startswith("/"):
        return any(contents == evidence for _, _, contents in string_groups)
    evidence_tokens = c_lex_tokens(evidence)
    if not evidence_tokens:
        return False
    if all(token.kind == "string" for token in evidence_tokens):
        wanted = "".join(c_string_literal_contents(token) or "" for token in evidence_tokens)
        return any(contents == wanted for _, _, contents in string_groups)
    width = len(evidence_tokens)
    wanted = [(token.kind, token.value) for token in evidence_tokens]
    for index in range(len(body_tokens) - width + 1):
        if [(token.kind, token.value) for token in body_tokens[index:index + width]] != wanted:
            continue
        if (
            width == 1
            and evidence_tokens[0].kind == "identifier"
            and evidence.startswith("DCC_REST_ROUTE_")
        ):
            neighbors = []
            if index > 0:
                neighbors.append(body_tokens[index - 1])
            if index + 1 < len(body_tokens):
                neighbors.append(body_tokens[index + 1])
            if any(
                token.kind == "string" or c_token_is_string_macro(token)
                for token in neighbors
            ):
                continue
        return True
    return False


def public_declarations(root: Path) -> dict[str, list[Declaration]]:
    result: dict[str, list[Declaration]] = defaultdict(list)
    include_root = root / "include/dcc/rest"
    for path in sorted(include_root.rglob("*.h")):
        text = path.read_text(encoding="utf-8")
        masked = c_discovery_mask(text)
        relative = path.relative_to(root).as_posix()
        for match in PUBLIC_DECLARATION.finditer(masked):
            item = Declaration(
                match.group("name"),
                normalized_args(match.group("return_type")),
                normalized_args(match.group("args")),
                relative,
            )
            result[item.name].append(item)
    return result


def external_definitions(root: Path) -> dict[str, list[Definition]]:
    result: dict[str, list[Definition]] = defaultdict(list)
    source_root = root / "src/rest"
    for path in sorted(source_root.rglob("*.c")):
        text = path.read_text(encoding="utf-8")
        masked = c_discovery_mask(text)
        relative = path.relative_to(root).as_posix()
        for match in EXTERNAL_DEFINITION.finditer(masked):
            body_start = match.end() - 1
            body = c_definition_body(text, body_start)
            item = Definition(
                match.group("name"),
                normalized_args(match.group("return_type")),
                normalized_args(match.group("args")),
                relative,
                body,
            )
            result[item.name].append(item)
    return result


def internal_declarations(root: Path) -> dict[str, list[Declaration]]:
    result: dict[str, list[Declaration]] = defaultdict(list)
    pattern = re.compile(
        r"\b(?P<name>dcc_rest_[A-Za-z0-9_]+)\s*\((?P<args>.*?)\)\s*;",
        re.S,
    )
    internal_root = root / "src/internal"
    for path in sorted(internal_root.rglob("*.h")):
        text = path.read_text(encoding="utf-8")
        masked = c_discovery_mask(text)
        relative = path.relative_to(root).as_posix()
        for match in pattern.finditer(masked):
            item = Declaration(
                match.group("name"),
                "",
                normalized_args(match.group("args")),
                relative,
            )
            result[item.name].append(item)
    return result


def internal_declaration_names(root: Path) -> set[str]:
    return set(internal_declarations(root))


def load_manifest(path: Path) -> dict[str, Any]:
    try:
        value = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"manifest is missing: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"manifest is malformed JSON: {exc}") from exc
    if not isinstance(value, dict):
        raise ValueError("manifest top level must be an object")
    return value


def normalized_template(template: str) -> str:
    return re.sub(r"\{[A-Za-z_][A-Za-z0-9_]*\}", "{}", template)


def printf_route_template(literal: str) -> str | None:
    if not literal.startswith("/"):
        return None
    converted = re.sub(r"%(?:\.\*)?(?:ll|z)?[usdi]", "{}", literal.split("?", 1)[0])
    if "%" in converted:
        return None
    return normalized_template(converted)


def route_definition_templates(root: Path) -> tuple[dict[str, set[str]], list[str]]:
    path = root / ROUTE_DEFINITION_HEADER
    if not path.is_file():
        return {}, [f"canonical route definition table is missing: {ROUTE_DEFINITION_HEADER}"]
    text = c_active_preprocessor_source(path.read_text(encoding="utf-8"))
    result: dict[str, set[str]] = defaultdict(set)
    for match in re.finditer(
        r"^[ \t]*\#[ \t]*define[ \t]+(DCC_REST_ROUTE_[A-Z0-9_]+)[ \t]+"
        r"\"([^\"\\]*(?:\\.[^\"\\]*)*)\"[ \t]*$",
        text,
        re.M,
    ):
        template = printf_route_template(match.group(2))
        if template is not None:
            result[match.group(1)].add(template)
    return dict(result), []


def reviewed_helper_route_templates(tokens: set[str]) -> set[str] | None:
    if "dcc_rest_alloc_message_reaction_path" in tokens:
        if '"/@me"' in tokens:
            return {"/channels/{}/messages/{}/reactions/{}/@me"}
        if '"/%llu"' in tokens:
            return {"/channels/{}/messages/{}/reactions/{}/{}"}
        if "reaction, NULL" in tokens:
            return {"/channels/{}/messages/{}/reactions/{}"}
    if "dcc_rest_alloc_template_path" in tokens:
        return {"/guilds/templates/{}"}
    if "dcc_rest_alloc_guild_template_path" in tokens:
        return {"/guilds/{}/templates/{}"}
    if "dcc_rest_interaction_webhook_path" in tokens:
        if '"messages/@original"' in tokens:
            return {"/webhooks/{}/{}/messages/@original"}
        if '"messages"' in tokens:
            return {"/webhooks/{}/{}/messages/{}"}
        if "NULL, 0" in tokens:
            return {"/webhooks/{}/{}"}
    if "dcc_rest_webhook_token_path" in tokens:
        if '"messages"' in tokens:
            return {"/webhooks/{}/{}/messages/{}"}
        if "NULL, 0" in tokens:
            return {"/webhooks/{}/{}"}
    if "dcc_rest_webhook_compat_path" in tokens:
        if '"github"' in tokens:
            return {"/webhooks/{}/{}/github"}
        if '"slack"' in tokens:
            return {"/webhooks/{}/{}/slack"}
    return None


def evidence_route_templates(
    data: dict[str, Any],
    entry: dict[str, Any],
    phase: str,
    entry_by_symbol: dict[str, dict[str, Any]],
    route_definitions: dict[str, set[str]],
) -> tuple[set[str], list[str]]:
    templates: set[str] = set()
    errors: list[str] = []
    evidence = entry.get("route_evidence")
    if not isinstance(evidence, dict) or not isinstance(evidence.get(phase), dict):
        return templates, errors
    for symbol, token_value in evidence[phase].items():
        if not isinstance(token_value, list) or not all(isinstance(token, str) for token in token_value):
            continue
        tokens = set(token_value)
        resolved = False
        helper_templates = reviewed_helper_route_templates(tokens)
        if helper_templates is not None:
            templates.update(helper_templates)
            resolved = True
        for token in tokens:
            direct = printf_route_template(token)
            if direct is not None:
                templates.add(direct)
                resolved = True
            if token in route_definitions:
                templates.update(route_definitions[token])
                resolved = True
            target = entry_by_symbol.get(token)
            if target is not None:
                target_route = data.get("route_catalog", {}).get(target.get("route_formatter"))
                if isinstance(target_route, dict) and isinstance(target_route.get("templates"), list):
                    templates.update(
                        normalized_template(template)
                        for template in target_route["templates"]
                        if isinstance(template, str)
                    )
                    resolved = True
        if not resolved:
            errors.append(
                f"{entry.get('canonical')}: unresolved {phase} route evidence for {symbol}"
            )
    return templates, errors


def sorted_unique_strings(value: Any) -> bool:
    return (
        isinstance(value, list)
        and bool(value)
        and all(isinstance(item, str) and item for item in value)
        and value == sorted(set(value))
    )


PARAMETER_CONTRACT_FIELDS = frozenset({"declaration", "role"})
PARAMETER_ROLES = frozenset({
    "client", "path", "input", "input_count", "options", "out_request",
})
COMMON_CANONICAL_PARAMETERS = (
    {"declaration": "dcc_client_t *client", "role": "client"},
    {
        "declaration": "const dcc_rest_call_options_t *options",
        "role": "options",
    },
    {"declaration": "dcc_rest_request_t **out_request", "role": "out_request"},
)
ROUTE_STRING_PARAMETER_NAMES = frozenset({
    "code",
    "instance_id",
    "interaction_token",
    "invite_code",
    "reaction",
    "webhook_token",
})
def canonical_parameter_contract_errors(
    entry: dict[str, Any],
    route: dict[str, Any] | None,
) -> list[str]:
    """Validate the manifest's endpoint-specific approved parameter roles."""
    canonical = entry.get("canonical", "<unknown>")
    value = entry.get("canonical_parameters")
    if not isinstance(value, list) or not value:
        return [f"{canonical}: canonical_parameters must be a non-empty array"]
    if any(not isinstance(item, dict) or set(item) != PARAMETER_CONTRACT_FIELDS for item in value):
        return [
            f"{canonical}: every canonical parameter must contain exactly declaration and role"
        ]
    if len(value) < 3:
        return [f"{canonical}: canonical_parameters omits the common submit parameters"]
    declarations = [item.get("declaration") for item in value]
    roles = [item.get("role") for item in value]
    if any(not isinstance(item, str) or not item for item in declarations):
        return [f"{canonical}: canonical parameter declarations must be non-empty strings"]
    if any(role not in PARAMETER_ROLES for role in roles):
        return [f"{canonical}: canonical_parameters contains an unknown role"]
    if any(item != normalized_args(item) for item in declarations):
        return [f"{canonical}: canonical_parameters must use normalized declarations"]
    parsed = split_c_parameters(", ".join(declarations))
    if parsed != declarations:
        return [f"{canonical}: canonical_parameters is not a valid exact C parameter list"]
    names = [c_parameter_name(item) for item in declarations]
    if any(name is None for name in names) or len(set(names)) != len(names):
        return [f"{canonical}: canonical_parameters has missing or duplicate parameter names"]
    errors: list[str] = []
    if value[0] != COMMON_CANONICAL_PARAMETERS[0]:
        errors.append(f"{canonical}: canonical_parameters must begin with dcc_client_t *client")
    if value[-2:] != list(COMMON_CANONICAL_PARAMETERS[1:]):
        errors.append(f"{canonical}: canonical_parameters must end in options and out_request")

    middle = list(value[1:-2])
    typed_input = entry.get("input")
    if isinstance(typed_input, str):
        if typed_input.endswith("[]"):
            element = typed_input[:-2]
            input_items = [item for item in middle if item["role"] == "input"]
            count_items = [item for item in middle if item["role"] == "input_count"]
            input_index = middle.index(input_items[0]) if len(input_items) == 1 else -1
            count_index = middle.index(count_items[0]) if len(count_items) == 1 else -1
            valid_pair = (
                len(input_items) == 1
                and len(count_items) == 1
                and count_index == input_index + 1
                and re.fullmatch(
                    rf"const {re.escape(element)} \*[A-Za-z_][A-Za-z0-9_]*",
                    input_items[0]["declaration"],
                ) is not None
                and re.fullmatch(
                    r"size_t [A-Za-z_][A-Za-z0-9_]*_count",
                    count_items[0]["declaration"],
                ) is not None
            )
            if not valid_pair:
                errors.append(f"{canonical}: canonical_parameters omits typed array input {typed_input}")
        else:
            input_items = [item for item in middle if item["role"] == "input"]
            valid_input = (
                len(input_items) == 1
                and re.fullmatch(
                    rf"const {re.escape(typed_input)} \*[A-Za-z_][A-Za-z0-9_]*",
                    input_items[0]["declaration"],
                ) is not None
            )
            if not valid_input:
                errors.append(f"{canonical}: canonical_parameters omits typed input {typed_input}")
            if any(item["role"] == "input_count" for item in middle):
                errors.append(f"{canonical}: scalar typed input has an unapproved input_count")
    elif any(item["role"] in {"input", "input_count"} for item in middle):
        errors.append(f"{canonical}: input-free endpoint has an unapproved input role")

    numeric_count = 0
    string_count = 0
    for item in middle:
        parameter = item["declaration"]
        if item["role"] in {"input", "input_count"}:
            continue
        if item["role"] != "path":
            errors.append(
                f"{canonical}: middle parameter {parameter!r} must have a path or input role"
            )
            continue
        if re.fullmatch(r"dcc_snowflake_t [A-Za-z_][A-Za-z0-9_]*_id", parameter):
            numeric_count += 1
        elif parameter == "uint32_t answer_id":
            numeric_count += 1
        else:
            string_match = re.fullmatch(r"const char \*(?P<name>[A-Za-z_][A-Za-z0-9_]*)", parameter)
            if string_match is not None and string_match.group("name") in ROUTE_STRING_PARAMETER_NAMES:
                string_count += 1
            else:
                errors.append(
                    f"{canonical}: canonical_parameters contains unapproved role {parameter!r}"
                )

    templates = route.get("templates", []) if isinstance(route, dict) else []
    numeric_slots = 0
    string_slots = 0
    for template in templates if isinstance(templates, list) else []:
        placeholders = re.findall(r"\{([A-Za-z_][A-Za-z0-9_]*)\}", template)
        numeric_slots = max(
            numeric_slots,
            sum(
                placeholder.startswith("snowflake_")
                or placeholder.startswith("integer_")
                or placeholder.endswith("_id")
                for placeholder in placeholders
            ),
        )
        string_slots = max(
            string_slots,
            sum(
                placeholder.startswith("segment_")
                or placeholder in {
                    "emoji", "interaction_token", "template_code", "webhook_token",
                }
                for placeholder in placeholders
            ),
        )
    if numeric_count != numeric_slots:
        errors.append(
            f"{canonical}: canonical_parameters provides {numeric_count} numeric route roles, "
            f"expected {numeric_slots}"
        )
    if string_count != string_slots:
        errors.append(
            f"{canonical}: canonical_parameters provides {string_count} string route roles, "
            f"expected {string_slots}"
        )
    return errors


def validate_manifest(data: dict[str, Any], root: Path, *, check_files: bool = True) -> list[str]:
    errors: list[str] = []
    top_unknown = sorted(set(data) - TOP_LEVEL_FIELDS)
    top_missing = sorted(TOP_LEVEL_FIELDS - set(data))
    if top_unknown:
        errors.append("unknown top-level field(s): " + ", ".join(top_unknown))
    if top_missing:
        errors.append("missing top-level field(s): " + ", ".join(top_missing))
    if data.get("schema_version") != 1:
        errors.append("schema_version must be 1")

    generic = data.get("generic_operations")
    if not isinstance(generic, list) or not all(isinstance(item, str) for item in generic):
        errors.append("generic_operations must be a string array")
        generic = []
    elif generic != sorted(generic) or len(generic) != len(set(generic)):
        errors.append("generic_operations must be sorted and unique")
    if tuple(generic) != GENERIC_OPERATIONS:
        errors.append("generic_operations differs from the immutable infrastructure allowlist")

    helpers = data.get("transition_helpers")
    if not isinstance(helpers, list):
        errors.append("transition_helpers must be an array")
        helpers = []
    elif [item.get("symbol") if isinstance(item, dict) else None for item in helpers] != sorted(
        item.get("symbol") if isinstance(item, dict) else "" for item in helpers
    ):
        errors.append("transition_helpers must be sorted by symbol")
    for index, helper in enumerate(helpers):
        label = f"transition_helpers[{index}]"
        if not isinstance(helper, dict) or set(helper) != HELPER_FIELDS:
            errors.append(f"{label} must contain exactly symbol, header, source, and removal_task")
            continue
        for field in ("symbol", "header", "source"):
            if not isinstance(helper[field], str) or not helper[field]:
                errors.append(f"{label}.{field} must be a non-empty string")
        if helper["removal_task"] != 10:
            errors.append(f"{label}.removal_task must be 10")
    if helpers != [TRANSITION_HELPER]:
        errors.append("transition_helpers differs from the exact Task 10 helper classification")

    removed_candidates = data.get("removed_candidates")
    if not isinstance(removed_candidates, list):
        errors.append("removed_candidates must be an array")
        removed_candidates = []
    expected_removed_candidates = [
        {
            "canonical": canonical,
            "domain": spec["domain"],
            "inventory_task": spec["inventory_task"],
            "legacy_symbols": list(spec["legacy_symbols"]),
            "owners": spec["owners"],
            "reason": spec["reason"],
            "removal_task": 7,
        }
        for canonical, spec in sorted(REMOVED_CANDIDATE_SPECS.items())
    ]
    if removed_candidates != expected_removed_candidates:
        errors.append(
            "removed_candidates differs from the exact seven-operation removal ledger"
        )
    for index, candidate in enumerate(removed_candidates):
        label = f"removed_candidates[{index}]"
        if not isinstance(candidate, dict):
            continue
        unknown = sorted(set(candidate) - REMOVED_CANDIDATE_FIELDS)
        missing = sorted(REMOVED_CANDIDATE_FIELDS - set(candidate))
        if unknown:
            errors.append(f"{label} has unknown field(s): {', '.join(unknown)}")
        if missing:
            errors.append(f"{label} is missing field(s): {', '.join(missing)}")
            continue

    route_catalog = data.get("route_catalog")
    if not isinstance(route_catalog, dict):
        errors.append("route_catalog must be an object")
        route_catalog = {}
    elif list(route_catalog) != sorted(route_catalog):
        errors.append("route_catalog keys must be sorted")
    for identity, route in route_catalog.items():
        label = f"route_catalog.{identity}"
        if not isinstance(identity, str) or not IDENTIFIER.fullmatch(identity):
            errors.append(f"{label} is not an identifier")
            continue
        if not isinstance(route, dict):
            errors.append(f"{label} must be an object")
            continue
        unknown = sorted(set(route) - ROUTE_FIELDS)
        missing = sorted(ROUTE_FIELDS - set(route))
        if unknown:
            errors.append(f"{label} has unknown field(s): {', '.join(unknown)}")
        if missing:
            errors.append(f"{label} is missing field(s): {', '.join(missing)}")
            continue
        templates = route["templates"]
        if not sorted_unique_strings(templates):
            errors.append(f"{label}.templates must be a sorted unique non-empty string array")
            templates = []
        normalized_seen: set[str] = set()
        for template in templates:
            stripped = re.sub(r"\{[A-Za-z_][A-Za-z0-9_]*\}", "", template)
            if not template.startswith("/") or "{" in stripped or "}" in stripped:
                errors.append(f"{label}.templates has an invalid normalized Discord route: {template}")
            normalized = normalized_template(template)
            if normalized in normalized_seen:
                errors.append(f"{label}.templates repeats a normalized route: {normalized}")
            normalized_seen.add(normalized)
        methods = route["methods"]
        if not isinstance(methods, list) or not methods or any(method not in METHODS for method in methods):
            errors.append(f"{label}.methods must be a non-empty HTTP method array")
        elif methods != sorted(set(methods)):
            errors.append(f"{label}.methods must be sorted and unique")
        if not sorted_unique_strings(route["source_tokens"]):
            errors.append(f"{label}.source_tokens must be a sorted unique non-empty string array")

    endpoints = data.get("endpoints")
    if not isinstance(endpoints, list):
        errors.append("endpoints must be an array")
        return errors
    canonical_order = [entry.get("canonical") if isinstance(entry, dict) else None for entry in endpoints]
    if canonical_order != sorted(canonical_order, key=lambda value: "" if value is None else str(value)):
        errors.append("endpoint entries must be sorted by canonical symbol")

    classified: list[str] = list(generic)
    classified.extend(
        helper["symbol"] for helper in helpers if isinstance(helper, dict) and isinstance(helper.get("symbol"), str)
    )
    for candidate in removed_candidates:
        if not isinstance(candidate, dict):
            continue
        canonical = candidate.get("canonical")
        legacy = candidate.get("legacy_symbols")
        if isinstance(canonical, str):
            classified.append(canonical)
        if isinstance(legacy, list):
            classified.extend(symbol for symbol in legacy if isinstance(symbol, str))
    route_owners: dict[tuple[str, str], set[str]] = defaultdict(set)
    entry_by_canonical: dict[str, dict[str, Any]] = {}
    entry_by_symbol: dict[str, dict[str, Any]] = {}
    planned_seen: set[str] = set()
    for index, entry in enumerate(endpoints):
        label = f"endpoints[{index}]"
        if not isinstance(entry, dict):
            errors.append(f"{label} must be an object")
            continue
        unknown = sorted(set(entry) - ENTRY_FIELDS)
        missing = sorted(ENTRY_REQUIRED_FIELDS - set(entry))
        if unknown:
            errors.append(f"{label} has unknown field(s): {', '.join(unknown)}")
        if missing:
            errors.append(f"{label} is missing field(s): {', '.join(missing)}")
            continue
        canonical = entry["canonical"]
        if not isinstance(canonical, str) or not IDENTIFIER.fullmatch(canonical) or not canonical.startswith("dcc_rest_"):
            errors.append(f"{label}.canonical is not a dcc_rest_* identifier")
            continue
        if (
            canonical.endswith(FORBIDDEN_CANONICAL_SUFFIXES)
            and canonical not in SUFFIX_CANONICAL_EXCEPTIONS
        ):
            errors.append(f"{label}.canonical has a forbidden terminal overload suffix")
        if canonical.endswith("_thread") and canonical not in THREAD_CANONICALS:
            errors.append(f"{label}.canonical has an unreviewed _thread terminal suffix")
        entry_by_canonical[canonical] = entry
        canonical_state = entry["canonical_state"]
        if canonical_state not in {"current", "planned-at-baseline"}:
            errors.append(f"{label}.canonical_state must be current or planned-at-baseline")
        elif canonical_state == "planned-at-baseline":
            planned_seen.add(canonical)
            if canonical not in PLANNED_CANONICALS:
                errors.append(f"{label} marks an unreviewed planned canonical: {canonical}")
        elif canonical in PLANNED_CANONICALS:
            errors.append(f"{label} must retain planned-at-baseline state for {canonical}")
        for field in ("header", "domain", "method", "route_formatter", "source"):
            if not isinstance(entry[field], str) or not entry[field]:
                errors.append(f"{label}.{field} must be a non-empty string")
        domain = entry["domain"]
        task = entry["task"]
        if domain not in DOMAIN_TASKS:
            errors.append(f"{label}.domain is unknown: {domain}")
        elif task != DOMAIN_TASKS[domain]:
            errors.append(f"{label} assigns {domain} to task {task}, expected {DOMAIN_TASKS[domain]}")
        if task not in {6, 7, 8, 9, 10}:
            errors.append(f"{label}.task must be 6, 7, 8, 9, or 10")
        if entry["method"] not in METHODS:
            errors.append(f"{label}.method is unknown: {entry['method']}")
        if entry["input"] is not None and (
            not isinstance(entry["input"], str)
            or re.fullmatch(r"dcc_[A-Za-z0-9_]+_t(?:\[\])?", entry["input"]) is None
        ):
            errors.append(f"{label}.input must be null or a typed dcc_*_t record/array")
        if canonical in SCALAR_ONLY_ENDPOINTS and entry["input"] is not None:
            errors.append(f"{label}.input must be null for the reviewed scalar-only endpoint")
        if not isinstance(entry["multipart"], bool):
            errors.append(f"{label}.multipart must be boolean")
        auth_policy = entry["auth_policy"]
        if not isinstance(auth_policy, str) or auth_policy not in AUTH_POLICIES:
            errors.append(f"{label}.auth_policy is unknown: {auth_policy!r}")
        expected_policy = expected_auth_policy(canonical)
        if auth_policy != expected_policy:
            errors.append(
                f"{label}.auth_policy must be {expected_policy} for the reviewed endpoint"
            )
        if not isinstance(entry["audit_log_reason"], bool):
            errors.append(f"{label}.audit_log_reason must be boolean")
        expected_audit = canonical in AUDIT_REASON_ENDPOINTS
        if entry["audit_log_reason"] is not expected_audit:
            errors.append(
                f"{label}.audit_log_reason differs from the exact reviewed capability"
            )
        if not isinstance(entry["sensitive_path"], bool):
            errors.append(f"{label}.sensitive_path must be boolean")
        expected_sensitive_path = canonical in SENSITIVE_PATH_ENDPOINTS
        sensitive_parameters = (
            entry["canonical_parameters"]
            if isinstance(entry["canonical_parameters"], list)
            else []
        )
        path_declares_token = any(
            parameter.get("role") == "path"
            and parameter.get("declaration") in {
                "const char *interaction_token", "const char *webhook_token",
            }
            for parameter in sensitive_parameters
            if isinstance(parameter, dict)
        )
        if entry["sensitive_path"] is not expected_sensitive_path:
            errors.append(
                f"{label}.sensitive_path differs from the exact reviewed capability"
            )
        if path_declares_token is not expected_sensitive_path:
            errors.append(
                f"{label}.sensitive_path disagrees with token-bearing path roles"
            )
        expected_input = REVIEWED_ENDPOINT_INPUTS.get(canonical)
        if expected_input is not None and entry["input"] != expected_input:
            errors.append(
                f"{label}.input must be {expected_input} for the reviewed endpoint contract"
            )
        if isinstance(entry["multipart"], bool):
            expected_multipart = canonical in MULTIPART_ENDPOINTS
            if entry["multipart"] != expected_multipart:
                errors.append(
                    f"{label}.multipart differs from the exact reviewed capability"
                )
        legacy = entry["legacy_symbols"]
        if not isinstance(legacy, list) or not all(isinstance(item, str) and IDENTIFIER.fullmatch(item) for item in legacy):
            errors.append(f"{label}.legacy_symbols must be an identifier array")
            legacy = []
        elif legacy != sorted(legacy) or len(legacy) != len(set(legacy)):
            errors.append(f"{label}.legacy_symbols must be sorted and unique")
        if canonical in legacy:
            errors.append(f"{label}.legacy_symbols repeats its canonical symbol")
        legacy_owners = entry["legacy_owners"]
        if not isinstance(legacy_owners, dict) or set(legacy_owners) != set(legacy):
            errors.append(f"{label}.legacy_owners must exactly own every legacy symbol")
            legacy_owners = {}
        elif list(legacy_owners) != sorted(legacy_owners):
            errors.append(f"{label}.legacy_owners keys must be sorted")
        for symbol, owner in legacy_owners.items():
            owner_label = f"{label}.legacy_owners.{symbol}"
            if not isinstance(owner, dict) or set(owner) != OWNER_FIELDS:
                errors.append(f"{owner_label} must contain exactly header and source")
                continue
            for field in OWNER_FIELDS:
                if not isinstance(owner[field], str) or not owner[field]:
                    errors.append(f"{owner_label}.{field} must be a non-empty string")
        transition_declarations = entry.get("transition_declarations", [])
        if not sorted_unique_strings(transition_declarations) and transition_declarations != []:
            errors.append(f"{label}.transition_declarations must be a sorted unique string array")
            transition_declarations = []
        expected_transition = list(TRANSITION_DECLARATIONS.get(canonical, ()))
        if transition_declarations != expected_transition:
            errors.append(
                f"{label}.transition_declarations differs from the exact reviewed duplicate declarations"
            )
        route_evidence = entry["route_evidence"]
        if not isinstance(route_evidence, dict) or set(route_evidence) != EVIDENCE_PHASES:
            errors.append(f"{label}.route_evidence must contain exactly baseline and canonical")
            route_evidence = {}
        method_evidence = entry["method_evidence"]
        if not isinstance(method_evidence, dict) or set(method_evidence) != EVIDENCE_PHASES:
            errors.append(f"{label}.method_evidence must contain exactly baseline and canonical")
            method_evidence = {}
        group_symbols = {canonical, *legacy}
        for field, evidence in (("route_evidence", route_evidence), ("method_evidence", method_evidence)):
            for phase in sorted(EVIDENCE_PHASES):
                phase_evidence = evidence.get(phase)
                if not isinstance(phase_evidence, dict) or not phase_evidence:
                    errors.append(
                        f"{label}.{field}.{phase} must be a non-empty symbol-to-token object"
                    )
                    continue
                if list(phase_evidence) != sorted(phase_evidence):
                    errors.append(f"{label}.{field}.{phase} keys must be sorted")
                if phase == "canonical" and set(phase_evidence) != {canonical}:
                    errors.append(
                        f"{label}.{field}.canonical must use only the canonical endpoint symbol"
                    )
                for symbol, token_value in phase_evidence.items():
                    if symbol not in group_symbols:
                        errors.append(f"{label}.{field}.{phase} owns unrelated symbol {symbol}")
                    if field == "route_evidence":
                        if not sorted_unique_strings(token_value):
                            errors.append(
                                f"{label}.{field}.{phase}.{symbol} must be a sorted unique "
                                "non-empty token array"
                            )
                    elif not isinstance(token_value, str) or not token_value:
                        errors.append(
                            f"{label}.{field}.{phase}.{symbol} must be a non-empty token"
                        )

        classified.extend([canonical, *legacy])
        for symbol in [canonical, *legacy]:
            entry_by_symbol[symbol] = entry
        if check_files:
            for field in ("header", "source"):
                candidate = root / entry[field]
                if not candidate.is_file():
                    errors.append(f"{label}.{field} does not exist: {entry[field]}")
        route = route_catalog.get(entry["route_formatter"])
        errors.extend(
            f"{label}: {error}"
            for error in canonical_parameter_contract_errors(
                entry,
                route if isinstance(route, dict) else None,
            )
        )
        if route is None:
            errors.append(f"{label}.route_formatter is absent from route_catalog")
        elif isinstance(route, dict) and ROUTE_FIELDS <= set(route):
            route_methods = route["methods"] if isinstance(route["methods"], list) else []
            route_templates = route["templates"] if sorted_unique_strings(route["templates"]) else []
            route_tokens = route["source_tokens"] if sorted_unique_strings(route["source_tokens"]) else []
            if entry["method"] not in route_methods:
                errors.append(
                    f"{label}.method {entry['method']} is not allowed by route "
                    f"{entry['route_formatter']}"
                )
            if isinstance(entry["method"], str) and entry["method"] in METHODS:
                for template in route_templates:
                    route_owners[(entry["method"], normalized_template(template))].add(canonical)
            for phase, phase_evidence in route_evidence.items():
                for symbol, tokens in phase_evidence.items():
                    if not isinstance(tokens, list):
                        continue
                    for token in tokens:
                        if token not in route_tokens:
                            errors.append(
                                f"{label}.route_evidence.{phase}.{symbol} token is absent from "
                                f"route_catalog.{entry['route_formatter']}.source_tokens"
                            )
        opaque = entry.get("opaque_payload", False)
        if not isinstance(opaque, bool):
            errors.append(f"{label}.opaque_payload must be boolean when present")
        if opaque != (canonical in OPAQUE_PAYLOAD_ENDPOINTS):
            errors.append(f"{label} violates the exact Slack/GitHub opaque-payload allowlist")
        if opaque and entry["input"] != "dcc_rest_webhook_compat_payload_t":
            errors.append(f"{label} opaque payload must use dcc_rest_webhook_compat_payload_t")
        if "deprecated" in entry and not isinstance(entry["deprecated"], bool):
            errors.append(f"{label}.deprecated must be boolean")

    if planned_seen != PLANNED_CANONICALS:
        errors.append(
            "planned canonical allowlist mismatch: "
            + ", ".join(sorted(planned_seen ^ PLANNED_CANONICALS))
        )

    for index, entry in enumerate(endpoints):
        if not isinstance(entry, dict) or not ENTRY_REQUIRED_FIELDS <= set(entry):
            continue
        label = f"endpoints[{index}]"
        evidence = entry["method_evidence"]
        if not isinstance(evidence, dict):
            continue
        for phase, phase_evidence in evidence.items():
            if not isinstance(phase_evidence, dict):
                continue
            for symbol, token in phase_evidence.items():
                if not isinstance(token, str):
                    continue
                evidenced_method = METHOD_TOKENS.get(token)
                if evidenced_method is None:
                    target = entry_by_symbol.get(token)
                    if target is None:
                        errors.append(
                            f"{label}.method_evidence.{phase}.{symbol} token must be an exact "
                            "method token or classified endpoint call target"
                        )
                    else:
                        evidenced_method = target["method"]
                if evidenced_method is not None and evidenced_method != entry["method"]:
                    errors.append(
                        f"{label}.method {entry['method']} disagrees with evidence token "
                        f"{token} ({evidenced_method})"
                    )

    route_definitions, route_definition_errors = route_definition_templates(root)
    errors.extend(route_definition_errors)
    for entry in endpoints:
        if not isinstance(entry, dict) or not ENTRY_REQUIRED_FIELDS <= set(entry):
            continue
        route = route_catalog.get(entry.get("route_formatter"))
        if not isinstance(route, dict) or not sorted_unique_strings(route.get("templates")):
            continue
        expected_templates = {
            normalized_template(template) for template in route["templates"]
        }
        for phase in sorted(EVIDENCE_PHASES):
            evidenced, evidence_errors = evidence_route_templates(
                data,
                entry,
                phase,
                entry_by_symbol,
                route_definitions,
            )
            errors.extend(evidence_errors)
            if evidenced != expected_templates:
                errors.append(
                    f"{entry['canonical']}: {phase} route evidence resolves to "
                    f"{sorted(evidenced)}, catalog declares {sorted(expected_templates)}"
                )

    collision_reasons: dict[str, set[str]] = defaultdict(set)
    observed_collisions: set[tuple[str, str]] = set()
    for key, owners in sorted(route_owners.items()):
        if len(owners) <= 1:
            continue
        observed_collisions.add(key)
        reviewed = REVIEWED_COLLISIONS.get(key)
        if reviewed is None or owners != reviewed[0]:
            errors.append(
                f"unreviewed method/route collision {key[0]} {key[1]}: "
                + ", ".join(sorted(owners))
            )
            continue
        reason = reviewed[1]
        for canonical in owners:
            collision_reasons[canonical].add(reason)
    for key, reviewed in REVIEWED_COLLISIONS.items():
        if route_owners.get(key, set()) != reviewed[0]:
            errors.append(
                f"reviewed collision set missing or changed for {key[0]} {key[1]}"
            )
    for entry in endpoints:
        if not isinstance(entry, dict) or "canonical" not in entry:
            continue
        reasons = collision_reasons.get(entry["canonical"], set())
        supplied = entry.get("collision_justification")
        if len(reasons) == 1:
            expected = next(iter(reasons))
            if supplied != expected:
                errors.append(
                    f"{entry['canonical']}: collision_justification must match the reviewed reason"
                )
        elif supplied is not None:
            errors.append(f"{entry['canonical']}: unreviewed collision_justification")

    composites = data.get("transition_composites")
    if not isinstance(composites, list):
        errors.append("transition_composites must be an array")
        composites = []
    composite_names = [item.get("name") if isinstance(item, dict) else None for item in composites]
    if composite_names != sorted(composite_names, key=lambda value: "" if value is None else str(value)):
        errors.append("transition_composites must be sorted by name")
    canonical_symbols = {
        entry["canonical"] for entry in endpoints if isinstance(entry, dict) and "canonical" in entry
    }
    composite_symbol_count = 0
    for index, composite in enumerate(composites):
        label = f"transition_composites[{index}]"
        if not isinstance(composite, dict):
            errors.append(f"{label} must be an object")
            continue
        unknown = sorted(set(composite) - COMPOSITE_FIELDS)
        missing = sorted(COMPOSITE_FIELDS - set(composite))
        if unknown:
            errors.append(f"{label} has unknown field(s): {', '.join(unknown)}")
        if missing:
            errors.append(f"{label} is missing field(s): {', '.join(missing)}")
            continue
        if not isinstance(composite["name"], str) or not IDENTIFIER.fullmatch(composite["name"]):
            errors.append(f"{label}.name must be an identifier")
        symbols = composite["symbols"]
        if not isinstance(symbols, list) or not symbols or not all(
            isinstance(symbol, str) and IDENTIFIER.fullmatch(symbol) for symbol in symbols
        ):
            errors.append(f"{label}.symbols must be a non-empty identifier array")
            symbols = []
        elif symbols != sorted(set(symbols)):
            errors.append(f"{label}.symbols must be sorted and unique")
        owners = composite["owners"]
        if not isinstance(owners, dict) or set(owners) != set(symbols):
            errors.append(f"{label}.owners must exactly own every composite symbol")
            owners = {}
        for symbol, owner in owners.items():
            owner_label = f"{label}.owners.{symbol}"
            if not isinstance(owner, dict) or set(owner) != OWNER_FIELDS:
                errors.append(f"{owner_label} must contain exactly header and source")
                continue
            for field in OWNER_FIELDS:
                if not isinstance(owner[field], str) or not owner[field]:
                    errors.append(f"{owner_label}.{field} must be a non-empty string")
        removal_task = composite["removal_task"]
        if removal_task not in {9, 10}:
            errors.append(f"{label}.removal_task must be 9 or 10")
        composed = composite["composed_endpoints"]
        if not isinstance(composed, list) or len(composed) < 2 or composed != sorted(set(composed)):
            errors.append(f"{label}.composed_endpoints must be a sorted unique array of at least two endpoints")
            composed = []
        elif not set(composed) <= canonical_symbols:
            errors.append(f"{label}.composed_endpoints references an unknown canonical endpoint")
        classified.extend(symbols)
        composite_symbol_count += len(symbols)

        expected = EXPECTED_COMPOSITES.get(composite.get("name"))
        if expected is None:
            errors.append(f"{label}.name is not a reviewed transition composite")
        else:
            expected_owners = {
                symbol: {"header": expected["header"], "source": expected["source"]}
                for symbol in expected["symbols"]
            }
            if tuple(symbols) != expected["symbols"]:
                errors.append(f"{label}.symbols differs from the reviewed composite")
            if owners != expected_owners:
                errors.append(f"{label}.owners differs from the reviewed composite")
            if removal_task != expected["removal_task"]:
                errors.append(f"{label}.removal_task differs from the reviewed composite")
            if tuple(composed) != expected["composed_endpoints"]:
                errors.append(f"{label}.composed_endpoints differs from the reviewed composite")

    if set(composite_names) != set(EXPECTED_COMPOSITES):
        errors.append("transition_composites differs from the exact reviewed composite set")

    duplicates = sorted(name for name, count in Counter(classified).items() if count > 1)
    if duplicates:
        errors.append("symbols classified more than once across all categories: " + ", ".join(duplicates))
    used_route_formatters = {
        entry.get("route_formatter")
        for entry in endpoints
        if isinstance(entry, dict) and isinstance(entry.get("route_formatter"), str)
    }
    unused_routes = sorted(set(route_catalog) - used_route_formatters)
    if unused_routes:
        errors.append("unused route_catalog entries: " + ", ".join(unused_routes))
    task_entry_counts = Counter(
        entry.get("task")
        for entry in endpoints
        if isinstance(entry, dict) and isinstance(entry.get("task"), int)
    )
    expected_entries = {6: 41, 7: 35, 8: 47, 9: 57, 10: 44}
    if len(endpoints) != 224 or dict(sorted(task_entry_counts.items())) != expected_entries:
        errors.append(
            f"endpoint count invariant failed: total={len(endpoints)} tasks={dict(sorted(task_entry_counts.items()))}"
        )
    domain_counts = Counter(
        entry.get("domain")
        for entry in endpoints
        if isinstance(entry, dict) and isinstance(entry.get("domain"), str)
    )
    if dict(sorted(domain_counts.items())) != dict(sorted(EXPECTED_DOMAIN_COUNTS.items())):
        errors.append(f"endpoint domain count invariant failed: {dict(sorted(domain_counts.items()))}")

    task7_legacy = {
        symbol
        for entry in endpoints
        if isinstance(entry, dict) and entry.get("task") == 7
        for symbol in entry.get("legacy_symbols", [])
        if isinstance(symbol, str)
    }
    if task7_legacy != TASK7_LEGACY_SYMBOLS:
        errors.append(
            "Task 7 legacy-symbol set differs from the exact 18-symbol removal contract"
        )
    active_canonicals = {
        entry.get("canonical") for entry in endpoints if isinstance(entry, dict)
    }
    unavailable_active = sorted(active_canonicals & set(REMOVED_CANDIDATE_SPECS))
    if unavailable_active:
        errors.append(
            "removal-ledger endpoint resurrected in active inventory: "
            + ", ".join(unavailable_active)
        )

    auth_counts: dict[int, Counter[str]] = defaultdict(Counter)
    audit_counts: Counter[int] = Counter()
    multipart_counts: Counter[int] = Counter()
    for entry in endpoints:
        if not isinstance(entry, dict) or not isinstance(entry.get("task"), int):
            continue
        task = entry["task"]
        if isinstance(entry.get("auth_policy"), str):
            auth_counts[task][entry["auth_policy"]] += 1
        if entry.get("audit_log_reason") is True:
            audit_counts[task] += 1
        if entry.get("multipart") is True:
            multipart_counts[task] += 1
    normalized_auth_counts = {
        task: dict(sorted(counts.items())) for task, counts in sorted(auth_counts.items())
    }
    if normalized_auth_counts != EXPECTED_AUTH_COUNTS:
        errors.append(f"endpoint auth-policy count invariant failed: {normalized_auth_counts}")
    normalized_audit_counts = {
        task: audit_counts.get(task, 0) for task in sorted(expected_entries)
    }
    if normalized_audit_counts != EXPECTED_AUDIT_COUNTS:
        errors.append(f"endpoint audit-reason count invariant failed: {normalized_audit_counts}")
    normalized_multipart_counts = {
        task: multipart_counts.get(task, 0) for task in sorted(expected_entries)
    }
    if normalized_multipart_counts != EXPECTED_MULTIPART_COUNTS:
        errors.append(f"endpoint multipart count invariant failed: {normalized_multipart_counts}")

    task_symbol_counts = Counter()
    current_symbol_counts = Counter()
    for entry in endpoints:
        if (
            isinstance(entry, dict)
            and isinstance(entry.get("legacy_symbols"), list)
            and isinstance(entry.get("task"), int)
        ):
            task_symbol_counts[entry.get("task")] += 1 + len(entry["legacy_symbols"])
            current_symbol_counts[entry.get("task")] += len(entry["legacy_symbols"])
            if entry.get("canonical_state") == "current":
                current_symbol_counts[entry.get("task")] += 1
    if dict(sorted(task_symbol_counts.items())) != BASELINE_IDENTIFIER_COUNTS:
        errors.append(f"endpoint symbol count invariant failed: {dict(sorted(task_symbol_counts.items()))}")
    if dict(sorted(current_symbol_counts.items())) != BASELINE_CURRENT_COUNTS:
        errors.append(
            f"baseline current endpoint symbol count invariant failed: {dict(sorted(current_symbol_counts.items()))}"
        )
    removed_symbol_count = sum(
        1 + len(candidate.get("legacy_symbols", []))
        for candidate in removed_candidates
        if isinstance(candidate, dict)
    )
    if (
        len(generic) != 60
        or len(helpers) != 1
        or sum(task_symbol_counts.values()) != 397
        or removed_symbol_count != 10
        or composite_symbol_count != 7
    ):
        errors.append(
            "public classification count invariant failed: "
            f"generic={len(generic)} helper={len(helpers)} "
            f"endpoint_identifiers={sum(task_symbol_counts.values())} "
            f"removed={removed_symbol_count} composite={composite_symbol_count}"
        )
    if (
        len(generic)
        + len(helpers)
        + sum(current_symbol_counts.values())
        + removed_symbol_count
        + composite_symbol_count
        != 473
    ):
        errors.append("baseline 473-public-symbol classification invariant failed")

    body_builder_owners: dict[str, str] = {}
    unexpected_builders: list[str] = []
    for entry in endpoints:
        if not isinstance(entry, dict):
            continue
        legacy_symbols = entry.get("legacy_symbols")
        if not isinstance(legacy_symbols, list):
            legacy_symbols = []
        for symbol in [entry.get("canonical"), *legacy_symbols]:
            if isinstance(symbol, str) and symbol.startswith("dcc_rest_build_"):
                if symbol in body_builder_owners:
                    continue
                body_builder_owners[symbol] = entry.get("canonical", "")
                if symbol not in BODY_BUILDERS:
                    unexpected_builders.append(symbol)
    if body_builder_owners != BODY_BUILDERS:
        errors.append(
            "official body-builder endpoint mapping differs from the reviewed 13-symbol mapping"
        )
    if unexpected_builders:
        errors.append("unreviewed public body builder(s): " + ", ".join(sorted(unexpected_builders)))
    return errors


def exact_symbol_owner_errors(
    symbol: str,
    declaration_paths: list[str],
    definition_path: str,
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
    *,
    present: bool,
) -> list[str]:
    errors: list[str] = []
    decls = declarations.get(symbol, [])
    defs = definitions.get(symbol, [])
    if not present:
        if decls:
            errors.append(f"{symbol}: declaration remains after its removal task")
        if defs:
            errors.append(f"{symbol}: definition remains after its removal task")
        return errors
    actual_declarations = sorted(item.path for item in decls)
    if actual_declarations != sorted(declaration_paths):
        errors.append(
            f"{symbol}: declaration owner/count mismatch; actual={actual_declarations} "
            f"expected={sorted(declaration_paths)}"
        )
    actual_definitions = [item.path for item in defs]
    if actual_definitions != [definition_path]:
        errors.append(
            f"{symbol}: definition owner/count mismatch; actual={actual_definitions} "
            f"expected={[definition_path]}"
        )
    return errors


def canonical_signature_errors(
    entry: dict[str, Any],
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
) -> list[str]:
    errors: list[str] = []
    canonical = entry["canonical"]
    decls = declarations.get(canonical, [])
    defs = definitions.get(canonical, [])
    if len(decls) == 1 and len(defs) == 1 and decls[0].args != defs[0].args:
        errors.append(f"{canonical}: public declaration and external definition arguments differ")
    expected_parameters = [
        parameter["declaration"] for parameter in entry["canonical_parameters"]
    ]
    for item in [*decls, *defs]:
        if item.return_type != "dcc_status_t":
            errors.append(
                f"{canonical}: {item.path} return type is {item.return_type!r}, expected dcc_status_t"
            )
        args = item.args
        if not re.match(r"^dcc_client_t\s*\*\s*client\b", args):
            errors.append(f"{canonical}: {item.path} does not begin with dcc_client_t *client")
        if not CALL_TAIL.search(args):
            errors.append(f"{canonical}: {item.path} does not end in call options and request output")
        if "dcc_rest_cb" in args or re.search(r"\buser_data\b", args):
            errors.append(f"{canonical}: {item.path} exposes callback/user_data")
        if RAW_ENDPOINT_INPUT.search(args):
            errors.append(f"{canonical}: {item.path} exposes raw endpoint JSON/query input")
        if "dcc_rest_future_t" in args:
            errors.append(f"{canonical}: {item.path} exposes a future")
        actual_parameters = split_c_parameters(args)
        if actual_parameters != expected_parameters:
            errors.append(
                f"{canonical}: {item.path} parameters differ from the exact manifest contract; "
                f"actual={actual_parameters} expected={expected_parameters}"
            )
    return errors


def canonical_endpoint_errors(
    entry: dict[str, Any],
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
) -> list[str]:
    canonical = entry["canonical"]
    errors = exact_symbol_owner_errors(
        canonical,
        [entry["header"]],
        entry["source"],
        declarations,
        definitions,
        present=True,
    )
    if declarations.get(canonical) and definitions.get(canonical):
        errors.extend(canonical_signature_errors(entry, declarations, definitions))
    for transition_path in entry.get("transition_declarations", []):
        if any(item.path == transition_path for item in declarations.get(canonical, [])):
            errors.append(
                f"{canonical}: transition duplicate declaration remains in {transition_path}"
            )
    for legacy in entry["legacy_symbols"]:
        errors.extend(
            exact_symbol_owner_errors(
                legacy,
                [],
                "",
                declarations,
                definitions,
                present=False,
            )
        )
    return errors


def canonical_capability_errors(
    entry: dict[str, Any],
    definitions: dict[str, list[Definition]],
) -> list[str]:
    canonical = entry["canonical"]
    defs = definitions.get(canonical, [])
    if len(defs) != 1:
        return []
    errors: list[str] = []
    for token in capability_evidence_tokens(entry):
        if not exact_c_evidence_present(defs[0].body, token):
            errors.append(
                f"{canonical}: capability token {token!r} is absent from exact "
                f"definition body {defs[0].path}"
            )
    return errors


def baseline_endpoint_errors(
    entry: dict[str, Any],
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
) -> list[str]:
    errors: list[str] = []
    canonical = entry["canonical"]
    if entry["canonical_state"] == "current":
        errors.extend(
            exact_symbol_owner_errors(
                canonical,
                [entry["header"], *entry.get("transition_declarations", [])],
                entry["source"],
                declarations,
                definitions,
                present=True,
            )
        )
    else:
        errors.extend(
            exact_symbol_owner_errors(
                canonical,
                [],
                "",
                declarations,
                definitions,
                present=False,
            )
        )
    for legacy in entry["legacy_symbols"]:
        owner = entry["legacy_owners"][legacy]
        errors.extend(
            exact_symbol_owner_errors(
                legacy,
                [owner["header"]],
                owner["source"],
                declarations,
                definitions,
                present=True,
            )
        )
    return errors


def future_endpoint_errors(
    entry: dict[str, Any],
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
) -> list[str]:
    baseline = baseline_endpoint_errors(entry, declarations, definitions)
    canonical = canonical_endpoint_errors(entry, declarations, definitions)
    if not baseline or not canonical:
        return []
    return [
        f"{entry['canonical']}: future endpoint matches neither its exact baseline "
        f"state nor its final canonical state (baseline: {baseline[0]}; canonical: {canonical[0]})"
    ]


def opaque_payload_contract_errors(root: Path) -> list[str]:
    code_texts: list[str] = []
    code_identifiers: set[str] = set()
    macro_definitions: set[str] = set()
    for path in sorted((root / "include/dcc/rest").rglob("*.h")):
        raw = path.read_text(encoding="utf-8")
        code_texts.append(c_discovery_mask(raw))
        code_identifiers.update(c_active_identifiers(raw))
        macro_definitions.update(c_active_macro_definitions(raw))
    text = "\n".join(code_texts)
    match = re.search(
        r"typedef\s+struct\s+dcc_rest_webhook_compat_payload\s*\{(?P<body>.*?)\}"
        r"\s*dcc_rest_webhook_compat_payload_t\s*;",
        text,
        re.S,
    )
    if match is None:
        return ["missing exact dcc_rest_webhook_compat_payload_t public record"]
    fields = [
        " ".join(field.split())
        for field in match.group("body").split(";")
        if field.strip()
    ]
    expected = ["size_t size", "uint32_t version", "const void *body", "size_t body_len"]
    errors: list[str] = []
    if fields != expected:
        errors.append(
            "dcc_rest_webhook_compat_payload_t fields must be exactly "
            "size/version/body/body_len"
        )
    for token in (
        "DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION",
        "DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT",
    ):
        if token not in macro_definitions:
            errors.append(f"missing opaque webhook payload contract token: {token}")
    init_function = "dcc_rest_webhook_compat_payload_init"
    if init_function not in code_identifiers:
        errors.append(f"missing opaque webhook payload contract token: {init_function}")
    return errors


def sensitive_transport_contract_errors(root: Path) -> list[str]:
    """Check Task 7's shared secret ownership and redacted identity seam."""
    source_paths = [
        *sorted((root / "src/rest").rglob("*.c")),
        *sorted((root / "src/internal/rest").rglob("*.h")),
    ]
    active_sources = {
        path.relative_to(root).as_posix(): c_active_source(
            path.read_text(encoding="utf-8")
        )
        for path in source_paths
    }
    combined = "\n".join(active_sources.values())
    identifiers = c_active_identifiers(combined)
    errors: list[str] = []
    required_identifiers = {
        "dcc_endpoint_secure_zero",
        "dcc_endpoint_sensitive_route_fingerprint",
        "dcc_endpoint_test_sensitive_probe_begin",
        "dcc_endpoint_test_sensitive_probe_snapshot",
        "dcc_endpoint_test_sensitive_probe_end",
        "dcc_endpoint_test_sensitive_force_failure_once",
        "dcc_endpoint_test_preparation_probe_begin",
        "dcc_endpoint_test_preparation_probe_snapshot",
        "dcc_endpoint_test_preparation_probe_end",
        "DCC_ENDPOINT_PATH_SENSITIVE",
        "DCC_ENDPOINT_PATH_PUBLIC",
        "DCC_ENDPOINT_ROUTE_KEY_OPAQUE",
    }
    missing = sorted(required_identifiers - identifiers)
    if missing:
        errors.append(
            "sensitive transport contract token(s) missing: " + ", ".join(missing)
        )
    if not exact_c_evidence_present(combined, '"dcc-rest-route-token-v1"'):
        errors.append(
            "sensitive route fingerprint omits the exact dcc-rest-route-token-v1 domain"
        )

    def private_definition_body(name: str) -> str:
        match = re.search(
            rf"\b{re.escape(name)}\s*\([^;{{}}]*\)\s*\{{",
            combined,
            re.S,
        )
        return c_definition_body(combined, match.end() - 1) if match else ""

    secure_zero_body = private_definition_body("dcc_endpoint_secure_zero")
    if not secure_zero_body or not any(
        exact_c_evidence_present(secure_zero_body, token)
        for token in ("volatile", "memset_s", "explicit_bzero", "SecureZeroMemory")
    ):
        errors.append(
            "secure-zero primitive lacks optimization-resistant write evidence"
        )
    fingerprint_body = private_definition_body(
        "dcc_endpoint_sensitive_route_fingerprint"
    )
    if (
        not fingerprint_body
        or not exact_c_evidence_present(
            fingerprint_body, '"dcc-rest-route-token-v1"'
        )
        or not exact_c_evidence_present(
            fingerprint_body, "dcc_endpoint_secure_zero"
        )
    ):
        errors.append(
            "sensitive route fingerprint must use the exact domain and wipe temporaries"
        )

    async_header = active_sources.get(
        "src/internal/rest/dcc_rest_async_request_internal.h", ""
    )
    if re.search(r"\bchar\s*\*\s*wire_path\s*;", async_header) is None or \
            re.search(r"\bchar\s*\*\s*operation\s*;", async_header) is None:
        errors.append(
            "async REST state must own distinct wire_path and nonsecret operation strings"
        )

    definitions = external_definitions(root)
    submit_defs = definitions.get("dcc_rest_submit", [])
    if len(submit_defs) != 1 or not exact_c_evidence_present(
        submit_defs[0].body if submit_defs else "",
        '"dcc_rest_submit"',
    ):
        errors.append("dcc_rest_submit must submit the fixed nonsecret operation identity")
    route_defs = definitions.get("dcc_rest_route_key", [])
    if len(route_defs) != 1 or not exact_c_evidence_present(
        route_defs[0].body if route_defs else "",
        "dcc_endpoint_sensitive_route_fingerprint",
    ):
        errors.append("rate-limit route keys must use the sensitive route fingerprint helper")
    free_defs = definitions.get("dcc_rest_async_request_free", [])
    if len(free_defs) != 1 or not exact_c_evidence_present(
        free_defs[0].body if free_defs else "",
        "dcc_endpoint_secure_zero",
    ):
        errors.append("async request teardown does not use the secure-zero primitive")
    prepared_defs = definitions.get("dcc_rest_prepared_request_deinit", [])
    if len(prepared_defs) != 1 or not exact_c_evidence_present(
        prepared_defs[0].body if prepared_defs else "",
        "dcc_endpoint_secure_zero",
    ):
        errors.append(
            "prepared Authorization/token teardown does not use the secure-zero primitive"
        )

    raw_operation_assignments = sorted(
        path
        for path, source in active_sources.items()
        if re.search(r"\.operation\s*=\s*(?:request->)?path\b", c_discovery_mask(source))
    )
    if raw_operation_assignments:
        errors.append(
            "observer operation still aliases a wire path in: "
            + ", ".join(raw_operation_assignments)
        )
    return errors


def audit_state_from_inventory(
    data: dict[str, Any],
    root: Path,
    progress_through: int | None,
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
    internal: dict[str, list[Declaration]],
    *,
    validate_schema: bool = True,
    check_opaque_contract: bool = True,
) -> tuple[list[str], dict[tuple[int, str], int]]:
    errors = validate_manifest(data, root) if validate_schema else []
    if errors:
        return errors, {}

    classified = set(data["generic_operations"])
    classified.update(item["symbol"] for item in data["transition_helpers"])
    for entry in data["endpoints"]:
        classified.add(entry["canonical"])
        classified.update(entry["legacy_symbols"])
    for candidate in data["removed_candidates"]:
        classified.add(candidate["canonical"])
        classified.update(candidate["legacy_symbols"])
    for composite in data["transition_composites"]:
        classified.update(composite["symbols"])

    unclassified_declarations = sorted(set(declarations) - classified)
    if unclassified_declarations:
        errors.append(
            "unclassified public dcc_rest_* declaration(s): "
            + ", ".join(unclassified_declarations)
        )
    unclassified_definitions = sorted(set(definitions) - set(declarations) - set(internal))
    if unclassified_definitions:
        errors.append(
            "external dcc_rest_* definition(s) have neither public nor internal declaration: "
            + ", ".join(unclassified_definitions)
        )
    for symbol, items in internal.items():
        if len(items) != 1:
            errors.append(f"{symbol}: expected one internal declaration, found {len(items)}")
        if symbol not in declarations and len(definitions.get(symbol, [])) != 1:
            errors.append(
                f"{symbol}: internal declaration must correspond to exactly one external definition"
            )

    for symbol in data["generic_operations"]:
        decls = declarations.get(symbol, [])
        defs = definitions.get(symbol, [])
        if len(decls) != 1:
            errors.append(f"{symbol}: immutable generic must have exactly one public declaration")
        if len(defs) != 1:
            errors.append(f"{symbol}: immutable generic must have exactly one external definition")

    helper = data["transition_helpers"][0]
    helper_present = progress_through is not None and progress_through < helper["removal_task"]
    errors.extend(
        exact_symbol_owner_errors(
            helper["symbol"],
            [helper["header"]],
            helper["source"],
            declarations,
            definitions,
            present=helper_present,
        )
    )

    removal_required = progress_through is None or progress_through >= 7
    for candidate in data["removed_candidates"]:
        for symbol, owner in candidate["owners"].items():
            if removal_required:
                errors.extend(
                    exact_symbol_owner_errors(
                        symbol,
                        [],
                        "",
                        declarations,
                        definitions,
                        present=False,
                    )
                )
                continue
            # Before Task 7 the reviewed unavailable symbol may still be in
            # its exact baseline owner or may have been removed early. A
            # partial/moved resurrection is never accepted.
            if not declarations.get(symbol) and not definitions.get(symbol):
                continue
            errors.extend(
                exact_symbol_owner_errors(
                    symbol,
                    [owner["header"]],
                    owner["source"],
                    declarations,
                    definitions,
                    present=True,
                )
            )

    for composite in data["transition_composites"]:
        composite_present = (
            progress_through is not None and progress_through < composite["removal_task"]
        )
        for symbol in composite["symbols"]:
            owner = composite["owners"][symbol]
            errors.extend(
                exact_symbol_owner_errors(
                    symbol,
                    [owner["header"]],
                    owner["source"],
                    declarations,
                    definitions,
                    present=composite_present,
                )
            )

    entry_by_symbol: dict[str, dict[str, Any]] = {}
    for entry in data["endpoints"]:
        entry_by_symbol[entry["canonical"]] = entry
        for legacy in entry["legacy_symbols"]:
            entry_by_symbol[legacy] = entry
    for entry in data["endpoints"]:
        canonical_now = not canonical_endpoint_errors(entry, declarations, definitions)
        baseline_now = not baseline_endpoint_errors(entry, declarations, definitions)
        canonical_required = progress_through is None or entry["task"] <= progress_through
        if canonical_now:
            evidence_phases = ["canonical"]
        elif baseline_now:
            evidence_phases = ["baseline"]
            if canonical_required:
                evidence_phases.append("canonical")
        else:
            evidence_phases = ["canonical" if canonical_required else "baseline"]
        for evidence_field in ("route_evidence", "method_evidence"):
            for evidence_phase in evidence_phases:
                for symbol, token_value in entry[evidence_field][evidence_phase].items():
                    defs = definitions.get(symbol, [])
                    if len(defs) != 1:
                        errors.append(
                            f"{entry['canonical']}: {evidence_field} symbol {symbol} must have "
                            "exactly one current definition"
                        )
                        continue
                    tokens = token_value if isinstance(token_value, list) else [token_value]
                    for token in tokens:
                        if not exact_c_evidence_present(defs[0].body, token):
                            errors.append(
                                f"{entry['canonical']}: {evidence_field} token {token!r} is absent "
                                f"from exact definition body {symbol}"
                            )
        for phase_evidence in entry["route_evidence"].values():
            for symbol, tokens in phase_evidence.items():
                if not isinstance(tokens, list):
                    continue
                for target_token in tokens:
                    target = entry_by_symbol.get(target_token)
                    if target is None:
                        continue
                    source_route = data["route_catalog"][entry["route_formatter"]]
                    target_route = data["route_catalog"][target["route_formatter"]]
                    source_templates = {normalized_template(item) for item in source_route["templates"]}
                    target_templates = {normalized_template(item) for item in target_route["templates"]}
                    if source_templates != target_templates:
                        errors.append(
                            f"{entry['canonical']}: route call-target evidence {target_token} has "
                            "different normalized templates"
                        )

    debt: dict[tuple[int, str], int] = defaultdict(int)
    for entry in data["endpoints"]:
        task = entry["task"]
        if progress_through is None or task <= progress_through:
            errors.extend(canonical_endpoint_errors(entry, declarations, definitions))
            if progress_through is None or progress_through >= 7:
                errors.extend(canonical_capability_errors(entry, definitions))
        else:
            errors.extend(future_endpoint_errors(entry, declarations, definitions))
            if canonical_endpoint_errors(entry, declarations, definitions):
                debt[(task, entry["domain"])] += 1

    if progress_through is not None:
        future_groups = {
            (entry["task"], entry["domain"])
            for entry in data["endpoints"]
            if entry["task"] > progress_through
        }
        for task, domain in sorted(future_groups):
            if debt.get((task, domain), 0) == 0:
                errors.append(
                    f"stale progress allowance: task {task} domain {domain} has no "
                    "remaining REST violation"
                )

    if check_opaque_contract:
        errors.extend(opaque_payload_contract_errors(root))
        if progress_through is None or progress_through >= 7:
            errors.extend(sensitive_transport_contract_errors(root))
    return errors, dict(debt)


def audit_state(
    data: dict[str, Any],
    root: Path,
    progress_through: int | None,
) -> tuple[list[str], dict[tuple[int, str], int]]:
    return audit_state_from_inventory(
        data,
        root,
        progress_through,
        public_declarations(root),
        external_definitions(root),
        internal_declarations(root),
    )


def synthetic_canonical_args(entry: dict[str, Any]) -> str:
    return ", ".join(
        parameter["declaration"] for parameter in entry["canonical_parameters"]
    )


def synthetic_add_symbol(
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
    symbol: str,
    declaration_paths: list[str],
    definition_path: str,
    args: str,
    return_type: str = "dcc_status_t",
) -> None:
    declarations[symbol] = [
        Declaration(symbol, return_type, args, path) for path in declaration_paths
    ]
    definitions[symbol] = [Definition(symbol, return_type, args, definition_path, "{ }")]


def synthetic_append_evidence(
    definitions: dict[str, list[Definition]],
    symbol: str,
    token: str,
) -> None:
    item = definitions[symbol][0]
    source_token = json.dumps(token) if token.startswith("/") else token
    definitions[symbol][0] = Definition(
        item.name,
        item.return_type,
        item.args,
        item.path,
        item.body[:-1] + f" ; {source_token} ; }}",
    )


def synthetic_evidence_source(token: str) -> str:
    return json.dumps(token) if token.startswith("/") else token


def synthetic_set_endpoint_state(
    entry: dict[str, Any],
    declarations: dict[str, list[Declaration]],
    definitions: dict[str, list[Definition]],
    *,
    canonical: bool,
) -> None:
    symbols = [entry["canonical"], *entry["legacy_symbols"]]
    for symbol in symbols:
        declarations.pop(symbol, None)
        definitions.pop(symbol, None)
    if canonical:
        synthetic_add_symbol(
            declarations,
            definitions,
            entry["canonical"],
            [entry["header"]],
            entry["source"],
            synthetic_canonical_args(entry),
        )
        phase = "canonical"
    else:
        if entry["canonical_state"] == "current":
            synthetic_add_symbol(
                declarations,
                definitions,
                entry["canonical"],
                [entry["header"], *entry.get("transition_declarations", [])],
                entry["source"],
                "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
            )
        for legacy in entry["legacy_symbols"]:
            owner = entry["legacy_owners"][legacy]
            synthetic_add_symbol(
                declarations,
                definitions,
                legacy,
                [owner["header"]],
                owner["source"],
                "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
            )
        phase = "baseline"
    for evidence_field in ("route_evidence", "method_evidence"):
        for symbol, token_value in entry[evidence_field][phase].items():
            tokens = token_value if isinstance(token_value, list) else [token_value]
            for token in tokens:
                synthetic_append_evidence(definitions, symbol, token)
    if canonical:
        for token in capability_evidence_tokens(entry):
            synthetic_append_evidence(definitions, entry["canonical"], token)


def synthetic_inventory(
    data: dict[str, Any],
    progress_through: int | None,
) -> tuple[
    dict[str, list[Declaration]],
    dict[str, list[Definition]],
    dict[str, list[Declaration]],
]:
    declarations: dict[str, list[Declaration]] = defaultdict(list)
    definitions: dict[str, list[Definition]] = defaultdict(list)
    internal: dict[str, list[Declaration]] = defaultdict(list)
    for symbol in data["generic_operations"]:
        synthetic_add_symbol(
            declarations,
            definitions,
            symbol,
            ["include/dcc/rest/synthetic_generic.h"],
            "src/rest/synthetic_generic.c",
            "void",
        )
    helper = data["transition_helpers"][0]
    if progress_through is not None and progress_through < helper["removal_task"]:
        synthetic_add_symbol(
            declarations,
            definitions,
            helper["symbol"],
            [helper["header"]],
            helper["source"],
            "char *json",
        )
    if progress_through is not None and progress_through < 7:
        for candidate in data["removed_candidates"]:
            for symbol, owner in candidate["owners"].items():
                synthetic_add_symbol(
                    declarations,
                    definitions,
                    symbol,
                    [owner["header"]],
                    owner["source"],
                    "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
                )
    for composite in data["transition_composites"]:
        if progress_through is not None and progress_through < composite["removal_task"]:
            for symbol in composite["symbols"]:
                owner = composite["owners"][symbol]
                synthetic_add_symbol(
                    declarations,
                    definitions,
                    symbol,
                    [owner["header"]],
                    owner["source"],
                    "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
                )
    for entry in data["endpoints"]:
        canonical = progress_through is None or entry["task"] <= progress_through
        synthetic_set_endpoint_state(
            entry,
            declarations,
            definitions,
            canonical=canonical,
        )
    return declarations, definitions, internal


def run_self_tests(data: dict[str, Any], root: Path) -> list[str]:
    failures: list[str] = []
    base_errors = validate_manifest(data, root)
    if base_errors:
        return ["base manifest failed schema validation: " + " | ".join(base_errors[:3])]

    rejected = 0

    def expect_rejected(label: str, mutate: Any) -> None:
        nonlocal rejected
        candidate = json.loads(json.dumps(data))
        mutate(candidate)
        if not validate_manifest(candidate, root, check_files=False):
            failures.append(f"self-test did not reject {label}")
        else:
            rejected += 1

    def expect_lexical_rejected(label: str, body: str, evidence: str) -> None:
        nonlocal rejected
        if exact_c_evidence_present(body, evidence):
            failures.append(f"self-test did not reject {label}")
        else:
            rejected += 1

    expect_lexical_rejected(
        "route identifier suffix",
        "{ DCC_REST_ROUTE_CHANNEL_MESSAGE_PIN; }",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    expect_lexical_rejected(
        "route identifier prefix",
        "{ PREFIX_DCC_REST_ROUTE_CHANNEL_MESSAGE; }",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    expect_lexical_rejected(
        "route literal prefix",
        '{ use("/wrong/channels/%llu"); }',
        "/channels/%llu",
    )
    expect_lexical_rejected(
        "route literal suffix",
        '{ use("/channels/%llu/wrong"); }',
        "/channels/%llu",
    )
    expect_lexical_rejected(
        "adjacent route literal suffix",
        '{ use("/channels/%llu" "/wrong"); }',
        "/channels/%llu",
    )
    expect_lexical_rejected(
        "adjacent quoted helper suffix",
        '{ helper("messages" "/wrong"); }',
        '"messages"',
    )
    expect_lexical_rejected(
        "route macro literal concatenation",
        '{ use(DCC_REST_ROUTE_CHANNEL_MESSAGE "/wrong"); }',
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    expect_lexical_rejected(
        "route macro identifier concatenation",
        "{ use(DCC_REST_ROUTE_CHANNEL_MESSAGE DCC_REST_ROUTE_SUFFIX); }",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    if not exact_c_evidence_present(
        '{ use("/channels/" "%llu"); }',
        "/channels/%llu",
    ):
        failures.append("self-test rejected valid adjacent C route literal")

    condition_truth_cases = {
        "0": False,
        "1U": True,
        "1LL": True,
        "0x2UL": True,
        "0b1": True,
        "(((1)))": True,
        "!0": True,
        "1 && 2": True,
        "0 || (3 >= 2)": True,
        "(2 != 2) || (4 < 3)": False,
        "DCC_PLATFORM_CONDITION": None,
    }
    for expression, expected_truth in condition_truth_cases.items():
        if c_condition_static_truth(expression) is not expected_truth:
            failures.append(
                f"literal conditional evaluator returned the wrong result for {expression!r}"
            )
    active_view_probe = r'''
#if 1U
ACTIVE_BEFORE;
#else
INACTIVE_ELSE;
#endif
ACTIVE_AFTER;
#if 0 && \
    (1 > 0)
INACTIVE_CONTINUED;
#elif !0
ACTIVE_ELIF;
#else
INACTIVE_ELIF_ELSE;
#endif
ACTIVE_TAIL;
'''
    active_view_once = c_active_source(active_view_probe)
    if (
        c_active_source(active_view_once) != active_view_once
        or c_active_identifiers(active_view_once)
        != {"ACTIVE_BEFORE", "ACTIVE_AFTER", "ACTIVE_ELIF", "ACTIVE_TAIL"}
    ):
        failures.append("active C source view is not idempotent across conditional branches")

    expect_lexical_rejected(
        "route evidence in inactive literal-false branch",
        """{
#if 0
    DCC_REST_ROUTE_CHANNEL_MESSAGE;
#endif
    DCC_REST_ROUTE_WRONG;
}""",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    expect_lexical_rejected(
        "route evidence in literal-true else branch",
        """{
#if 1LL
    DCC_REST_ROUTE_WRONG;
#else
    DCC_REST_ROUTE_CHANNEL_MESSAGE;
#endif
}""",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    )
    if not exact_c_evidence_present(
        """{
#if 0
    DCC_REST_ROUTE_WRONG;
#elif (!0 && (2 > 1))
    DCC_REST_ROUTE_CHANNEL_MESSAGE;
#else
    DCC_REST_ROUTE_WRONG;
#endif
}""",
        "DCC_REST_ROUTE_CHANNEL_MESSAGE",
    ):
        failures.append("self-test rejected evidence in active literal #elif branch")

    boundary_source = r'''
dcc_status_t dcc_rest_boundary_first(void) {
    const char *open = "{";
    const char *close = "}";
    const char escaped = '}';
    const char *escaped_quote = "\\\"}";
    /* } { */
#define DCC_BOUNDARY_BRACES { \
    }
    DCC_REST_ROUTE_WRONG;
    return DCC_OK;
}
dcc_status_t dcc_rest_boundary_second(void) {
    DCC_REST_ROUTE_CHANNEL_MESSAGE;
    return DCC_OK;
}
'''
    boundary_match = next(EXTERNAL_DEFINITION.finditer(boundary_source), None)
    boundary_body = (
        c_definition_body(boundary_source, boundary_match.end() - 1)
        if boundary_match is not None else ""
    )
    if (
        not boundary_body
        or not exact_c_evidence_present(boundary_body, "DCC_REST_ROUTE_WRONG")
        or exact_c_evidence_present(boundary_body, "DCC_REST_ROUTE_CHANNEL_MESSAGE")
    ):
        failures.append("lexer-aware definition boundary self-test failed")

    inactive_boundary_source = r'''
dcc_status_t dcc_rest_inactive_boundary_first(void) {
#if 0
}
dcc_status_t dcc_rest_inactive_boundary_decoy(void) {
    DCC_REST_ROUTE_INACTIVE_DECOY;
}
#endif
    DCC_REST_ROUTE_ACTIVE_AFTER_INACTIVE_BRACE;
    return DCC_OK;
}
dcc_status_t dcc_rest_inactive_boundary_second(void) {
    DCC_REST_ROUTE_NEXT_ACTIVE_FUNCTION;
    return DCC_OK;
}
'''
    inactive_boundary_match = next(
        EXTERNAL_DEFINITION.finditer(c_discovery_mask(inactive_boundary_source)),
        None,
    )
    inactive_boundary_body = (
        c_definition_body(
            inactive_boundary_source,
            inactive_boundary_match.end() - 1,
        )
        if inactive_boundary_match is not None else ""
    )
    if (
        not inactive_boundary_body
        or not exact_c_evidence_present(
            inactive_boundary_body,
            "DCC_REST_ROUTE_ACTIVE_AFTER_INACTIVE_BRACE",
        )
        or exact_c_evidence_present(
            inactive_boundary_body,
            "DCC_REST_ROUTE_INACTIVE_DECOY",
        )
        or exact_c_evidence_present(
            inactive_boundary_body,
            "DCC_REST_ROUTE_NEXT_ACTIVE_FUNCTION",
        )
    ):
        failures.append("inactive conditional definition boundary self-test failed")
    parsed_definitions = external_definitions(root)
    boundary_expectations = {
        "dcc_rest_build_auto_moderation_action": "dcc_rest_buffer_append_cstr",
        "dcc_rest_modify_guild_widget_params": "dcc_rest_buffer_deinit",
    }
    for symbol, terminal_token in boundary_expectations.items():
        items = parsed_definitions.get(symbol, [])
        if len(items) != 1 or not exact_c_evidence_present(items[0].body, terminal_token):
            failures.append(f"lexer-aware definition boundary regressed for {symbol}")

    with tempfile.TemporaryDirectory(prefix="dcc-rest-v2-discovery-") as directory:
        discovery_root = Path(directory)
        public_dir = discovery_root / "include/dcc/rest"
        source_dir = discovery_root / "src/rest"
        internal_dir = discovery_root / "src/internal"
        public_dir.mkdir(parents=True)
        source_dir.mkdir(parents=True)
        internal_dir.mkdir(parents=True)
        (public_dir / "probe.h").write_text(
            r'''
/* DCC_API dcc_status_t dcc_rest_public_comment_decoy(void); */
static const char *public_string_decoy =
    "DCC_API dcc_status_t dcc_rest_public_string_decoy(void);";
#define DCC_PUBLIC_MACRO_DECOY DCC_API dcc_status_t dcc_rest_public_macro_decoy(void);
#if ((0U))
DCC_API dcc_status_t dcc_rest_public_if0_decoy(void);
#if 1
DCC_API dcc_status_t dcc_rest_public_nested_if0_decoy(void);
#else
DCC_API dcc_status_t dcc_rest_public_nested_else_decoy(void);
#endif
#elif 0LL
DCC_API dcc_status_t dcc_rest_public_elif0_decoy(void);
#else
DCC_API dcc_status_t dcc_rest_public_conditional_real(void);
#endif
  #  if 1U
DCC_API dcc_status_t dcc_rest_public_truthy_real(void);
  #  else
DCC_API dcc_status_t dcc_rest_public_truthy_else_decoy(void);
  #  endif
#if 0
DCC_API dcc_status_t dcc_rest_public_elif_prelude_decoy(void);
#elif ((0x1ULL))
DCC_API dcc_status_t dcc_rest_public_truthy_elif_real(void);
#else
DCC_API dcc_status_t dcc_rest_public_truthy_elif_else_decoy(void);
#endif
#if (!0 && (3 >= 2))
DCC_API dcc_status_t dcc_rest_public_expression_real(void);
#else
DCC_API dcc_status_t dcc_rest_public_expression_else_decoy(void);
#endif
DCC_API dcc_status_t dcc_rest_public_real(void);
''',
            encoding="utf-8",
        )
        (source_dir / "probe.c").write_text(
            r'''
/* dcc_status_t dcc_rest_external_comment_decoy(void) { return DCC_OK; } */
static const char *external_string_decoy =
    "dcc_status_t dcc_rest_external_string_decoy(void) { return DCC_OK; }";
#define DCC_EXTERNAL_MACRO_DECOY dcc_status_t dcc_rest_external_macro_decoy(void) { }
#if 0x0UL
dcc_status_t dcc_rest_external_if0_decoy(void) { return DCC_OK; }
#else
dcc_status_t dcc_rest_external_conditional_real(void) { return DCC_OK; }
#endif
#if 1LL
dcc_status_t dcc_rest_external_truthy_real(void) { return DCC_OK; }
#else
dcc_status_t dcc_rest_external_truthy_else_decoy(void) { return DCC_OK; }
#endif
#if 0
dcc_status_t dcc_rest_external_elif_prelude_decoy(void) { return DCC_OK; }
#elif (0b1U || 0)
dcc_status_t dcc_rest_external_truthy_elif_real(void) { return DCC_OK; }
#else
dcc_status_t dcc_rest_external_truthy_elif_else_decoy(void) { return DCC_OK; }
#endif
dcc_status_t dcc_rest_external_real(void) {
    const char *brace = "}";
    return DCC_OK;
}
''',
            encoding="utf-8",
        )
        (internal_dir / "probe.h").write_text(
            r'''
/* dcc_rest_internal_comment_decoy(void); */
static const char *internal_string_decoy = "dcc_rest_internal_string_decoy(void);";
#define DCC_INTERNAL_MACRO_DECOY dcc_rest_internal_macro_decoy(void);
#if (0L)
dcc_status_t dcc_rest_internal_if0_decoy(void);
#else
dcc_status_t dcc_rest_internal_conditional_real(void);
#endif
#if (2 > 1)
dcc_status_t dcc_rest_internal_truthy_real(void);
#else
dcc_status_t dcc_rest_internal_truthy_else_decoy(void);
#endif
#if 0
dcc_status_t dcc_rest_internal_elif_prelude_decoy(void);
#elif !0
dcc_status_t dcc_rest_internal_truthy_elif_real(void);
#else
dcc_status_t dcc_rest_internal_truthy_elif_else_decoy(void);
#endif
dcc_status_t dcc_rest_internal_real(void);
''',
            encoding="utf-8",
        )
        discovery_results = {
            "public": set(public_declarations(discovery_root)),
            "external": set(external_definitions(discovery_root)),
            "internal": set(internal_declarations(discovery_root)),
        }
        discovery_expected = {
            "public": {
                "dcc_rest_public_conditional_real",
                "dcc_rest_public_expression_real",
                "dcc_rest_public_real",
                "dcc_rest_public_truthy_elif_real",
                "dcc_rest_public_truthy_real",
            },
            "external": {
                "dcc_rest_external_conditional_real",
                "dcc_rest_external_real",
                "dcc_rest_external_truthy_elif_real",
                "dcc_rest_external_truthy_real",
            },
            "internal": {
                "dcc_rest_internal_conditional_real",
                "dcc_rest_internal_real",
                "dcc_rest_internal_truthy_elif_real",
                "dcc_rest_internal_truthy_real",
            },
        }
        for kind in discovery_expected:
            if discovery_results[kind] != discovery_expected[kind]:
                failures.append(
                    f"lexer-aware {kind} discovery accepted a comment/string/macro decoy"
                )
            else:
                rejected += 1

    with tempfile.TemporaryDirectory(prefix="dcc-rest-v2-route-definitions-") as directory:
        route_root = Path(directory)
        route_path = route_root / ROUTE_DEFINITION_HEADER
        route_path.parent.mkdir(parents=True)
        route_path.write_text(
            r'''
/*
#define DCC_REST_ROUTE_COMMENT_DECOY "/comment/%llu"
*/
// #define DCC_REST_ROUTE_LINE_COMMENT_DECOY "/line-comment/%llu"
#if 0
#define DCC_REST_ROUTE_IF0_DECOY "/if0/%llu"
#endif
#if (1U && (2 == 2))
  #  define DCC_REST_ROUTE_ACTIVE_PROBE "/active/%llu"
#else
#define DCC_REST_ROUTE_TRUTHY_ELSE_DECOY "/truthy-else/%llu"
#endif
#if 0
#define DCC_REST_ROUTE_ELIF_PRELUDE_DECOY "/elif-prelude/%llu"
#elif ((0x1LL))
#define DCC_REST_ROUTE_ACTIVE_ELIF_PROBE "/active-elif/%llu"
#else
#define DCC_REST_ROUTE_ELIF_ELSE_DECOY "/elif-else/%llu"
#endif
''',
            encoding="utf-8",
        )
        route_templates, route_errors = route_definition_templates(route_root)
        expected_route_templates = {
            "DCC_REST_ROUTE_ACTIVE_PROBE": {"/active/{}"},
            "DCC_REST_ROUTE_ACTIVE_ELIF_PROBE": {"/active-elif/{}"},
        }
        if route_errors or route_templates != expected_route_templates:
            failures.append(
                "active preprocessor route definition view accepted an inactive/comment decoy"
            )
        else:
            rejected += 1

    with tempfile.TemporaryDirectory(prefix="dcc-rest-v2-opaque-contract-") as directory:
        opaque_root = Path(directory)
        opaque_path = opaque_root / "include/dcc/rest/probe.h"
        opaque_path.parent.mkdir(parents=True)
        active_opaque_contract = r'''
typedef struct dcc_rest_webhook_compat_payload {
    size_t size;
    uint32_t version;
    const void *body;
    size_t body_len;
} dcc_rest_webhook_compat_payload_t;
#define DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION 1U
#define DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT { 0 }
void dcc_rest_webhook_compat_payload_init(
    dcc_rest_webhook_compat_payload_t *payload);
'''
        opaque_path.write_text(active_opaque_contract, encoding="utf-8")
        if opaque_payload_contract_errors(opaque_root):
            failures.append("active opaque webhook payload contract self-test failed")

        opaque_decoys = {
            "comment-only opaque contract": f"/*{active_opaque_contract}*/\n",
            "inactive opaque contract": f"#if 0\n{active_opaque_contract}#endif\n",
            "prefixed/suffixed opaque identifiers": r'''
typedef struct dcc_rest_webhook_compat_payload {
    size_t size;
    uint32_t version;
    const void *body;
    size_t body_len;
} dcc_rest_webhook_compat_payload_t;
#define DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION_SUFFIX 1U
#define PREFIX_DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT { 0 }
void dcc_rest_webhook_compat_payload_init_suffix(
    dcc_rest_webhook_compat_payload_t *payload);
''',
            "opaque identifiers used but not defined": r'''
typedef struct dcc_rest_webhook_compat_payload {
    size_t size;
    uint32_t version;
    const void *body;
    size_t body_len;
} dcc_rest_webhook_compat_payload_t;
#if DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION
#endif
#define DCC_REST_OTHER_VERSION DCC_REST_WEBHOOK_COMPAT_PAYLOAD_VERSION
#define DCC_REST_OTHER_INIT DCC_REST_WEBHOOK_COMPAT_PAYLOAD_INIT
void dcc_rest_other_init(void) {
    (void)dcc_rest_webhook_compat_payload_init;
}
''',
        }
        for label, source in opaque_decoys.items():
            opaque_path.write_text(source, encoding="utf-8")
            opaque_errors = opaque_payload_contract_errors(opaque_root)
            if not opaque_errors:
                failures.append(f"self-test did not reject {label}")
            else:
                rejected += 1

    expect_rejected("unknown top-level field", lambda value: value.update({"surprise": True}))
    expect_rejected("unknown entry field", lambda value: value["endpoints"][0].update({"surprise": True}))
    expect_rejected("unsupported schema", lambda value: value.update({"schema_version": 2}))
    expect_rejected("unsorted entries", lambda value: value["endpoints"].reverse())
    expect_rejected("unknown domain", lambda value: value["endpoints"][0].update({"domain": "unknown"}))
    expect_rejected(
        "wrong task",
        lambda value: value["endpoints"][0].update({
            "task": 6 if value["endpoints"][0]["task"] != 6 else 10,
        }),
    )
    expect_rejected("unknown method", lambda value: value["endpoints"][0].update({"method": "TRACE"}))
    expect_rejected(
        "duplicate canonical symbol",
        lambda value: value["endpoints"][1].update({"canonical": value["endpoints"][0]["canonical"]}),
    )
    expect_rejected(
        "duplicate legacy symbol",
        lambda value: value["endpoints"][1]["legacy_symbols"].append(value["endpoints"][0]["canonical"]),
    )
    expect_rejected(
        "method/route collision",
        lambda value: value["endpoints"][1].update({
            "method": value["endpoints"][0]["method"],
            "route_formatter": value["endpoints"][0]["route_formatter"],
        }),
    )
    expect_rejected(
        "expanded opaque payload allowlist",
        lambda value: value["endpoints"][0].update({"opaque_payload": True}),
    )
    expect_rejected(
        "mutable generic allowlist",
        lambda value: value["generic_operations"].pop(),
    )
    expect_rejected(
        "mutable transition helper",
        lambda value: value["transition_helpers"][0].update({"removal_task": 9}),
    )
    expect_rejected(
        "expanded planned canonical allowlist",
        lambda value: value["endpoints"][0].update({"canonical_state": "planned-at-baseline"}),
    )
    expect_rejected(
        "missing auth-policy field",
        lambda value: value["endpoints"][0].pop("auth_policy"),
    )
    expect_rejected(
        "invalid auth-policy value",
        lambda value: value["endpoints"][0].update({"auth_policy": "ANY"}),
    )
    expect_rejected(
        "wrong Get Invite None-or-Bot auth policy",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_get_invite"
        ).update({"auth_policy": "BOT"}),
    )
    expect_rejected(
        "wrong self-lobby Bearer-only auth policy",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_create_lobby_channel_invite_for_self"
        ).update({"auth_policy": "BOT"}),
    )
    expect_rejected(
        "missing audit-reason capability",
        lambda value: value["endpoints"][0].pop("audit_log_reason"),
    )
    expect_rejected(
        "non-boolean audit-reason capability",
        lambda value: value["endpoints"][0].update({"audit_log_reason": "no"}),
    )
    expect_rejected(
        "false channel-position audit capability",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_modify_guild_channel_positions"
        ).update({"audit_log_reason": True}),
    )
    expect_rejected(
        "missing sensitive-path capability",
        lambda value: value["endpoints"][0].pop("sensitive_path"),
    )
    expect_rejected(
        "token-bearing path marked public",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_interaction_response_create"
        ).update({"sensitive_path": False}),
    )
    expect_rejected(
        "public path marked sensitive",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_get_channel"
        ).update({"sensitive_path": True}),
    )
    expect_rejected(
        "missing removal ledger",
        lambda value: value.pop("removed_candidates"),
    )
    expect_rejected(
        "missing removal-ledger owner",
        lambda value: value["removed_candidates"][0]["owners"].pop(
            value["removed_candidates"][0]["canonical"]
        ),
    )
    expect_rejected(
        "wrong removal-ledger reason",
        lambda value: value["removed_candidates"][0].update({"reason": "available"}),
    )

    def stale_231_inventory(value: dict[str, Any]) -> None:
        for index, original in enumerate(value["endpoints"][:7]):
            duplicate = json.loads(json.dumps(original))
            duplicate["canonical"] = f"dcc_rest_stale_candidate_{index}"
            value["endpoints"].append(duplicate)
        value["endpoints"].sort(key=lambda entry: entry["canonical"])

    expect_rejected("stale 231-entry candidate inventory", stale_231_inventory)

    def resurrect_unavailable_candidate(value: dict[str, Any]) -> None:
        entry = json.loads(json.dumps(value["endpoints"][0]))
        old = entry["canonical"]
        new = "dcc_rest_create_guild"
        entry["canonical"] = new
        for evidence_field in ("route_evidence", "method_evidence"):
            for phase in EVIDENCE_PHASES:
                evidence = entry[evidence_field][phase]
                if old in evidence:
                    evidence[new] = evidence.pop(old)
                    entry[evidence_field][phase] = dict(sorted(evidence.items()))
        value["endpoints"].append(entry)
        value["endpoints"].sort(key=lambda item: item["canonical"])

    expect_rejected("unavailable endpoint resurrection", resurrect_unavailable_candidate)

    def swap_task7_multipart_capability(value: dict[str, Any]) -> None:
        next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_create_forum_thread"
        )["multipart"] = False
        next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_get_active_threads"
        )["multipart"] = True

    expect_rejected("wrong Task 7 multipart set with unchanged count", swap_task7_multipart_capability)

    def remove_thread_member_query(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_get_thread_member"
        )
        entry["input"] = None
        entry["canonical_parameters"] = [
            parameter for parameter in entry["canonical_parameters"]
            if parameter["role"] != "input"
        ]

    expect_rejected("missing Get Thread Member query", remove_thread_member_query)
    def coherent_suffix_rename(value: dict[str, Any], suffix: str) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_consume_entitlement"
        )
        old = entry["canonical"]
        new = old + suffix
        entry["canonical"] = new
        for evidence_field in ("route_evidence", "method_evidence"):
            for phase in EVIDENCE_PHASES:
                evidence = entry[evidence_field][phase]
                if old in evidence:
                    evidence[new] = evidence.pop(old)
                    entry[evidence_field][phase] = dict(sorted(evidence.items()))
        value["endpoints"].sort(key=lambda item: item["canonical"])

    for suffix in FORBIDDEN_CANONICAL_SUFFIXES:
        expect_rejected(
            f"coherent canonical terminal overload suffix {suffix}",
            lambda value, suffix=suffix: coherent_suffix_rename(value, suffix),
        )
    expect_rejected(
        "missing canonical parameter contract",
        lambda value: value["endpoints"][0].pop("canonical_parameters"),
    )
    expect_rejected(
        "unknown canonical parameter role",
        lambda value: value["endpoints"][0]["canonical_parameters"][0].update({
            "role": "mystery",
        }),
    )
    def remove_required_path_parameter(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_get_message"
        )
        entry["canonical_parameters"] = [
            parameter
            for parameter in entry["canonical_parameters"]
            if parameter["declaration"] != "dcc_snowflake_t message_id"
        ]
    expect_rejected("missing required path parameter role", remove_required_path_parameter)
    def add_unapproved_raw_contract(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_crosspost_message"
        )
        entry["canonical_parameters"].insert(-2, {
            "declaration": "const void *request_payload",
            "role": "input",
        })
        entry["canonical_parameters"].insert(-2, {
            "declaration": "size_t request_payload_count",
            "role": "input_count",
        })
    expect_rejected("coherent unapproved raw pointer/count contract", add_unapproved_raw_contract)
    def restore_redundant_path_record(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_add_thread_member"
        )
        entry["input"] = "dcc_thread_member_params_t"
        entry["canonical_parameters"].insert(-2, {
            "declaration": "const dcc_thread_member_params_t *params",
            "role": "input",
        })
    expect_rejected(
        "redundant path-only typed input",
        restore_redundant_path_record,
    )
    def restore_generic_reaction_page(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_get_message_reactions"
        )
        entry["input"] = "dcc_rest_id_page_t"
        parameter = next(
            item for item in entry["canonical_parameters"]
            if item["role"] == "input"
        )
        parameter["declaration"] = "const dcc_rest_id_page_t *page"
    expect_rejected(
        "reaction query without official reaction type",
        restore_generic_reaction_page,
    )
    expect_rejected(
        "interaction callback multipart capability removal",
        lambda value: next(
            item for item in value["endpoints"]
            if item["canonical"] == "dcc_rest_interaction_response_create"
        ).update({"multipart": False}),
    )
    expect_rejected(
        "missing route evidence phase",
        lambda value: value["endpoints"][0]["route_evidence"].pop("baseline"),
    )
    def mutate_noncollision_template(value: dict[str, Any]) -> None:
        entry = next(
            item for item in value["endpoints"]
            if "collision_justification" not in item
        )
        value["route_catalog"][entry["route_formatter"]]["templates"] = [
            "/definitely-wrong/{id}",
        ]
    expect_rejected(
        "noncollision route template drift",
        mutate_noncollision_template,
    )
    expect_rejected(
        "wrong canonical method evidence",
        lambda value: value["endpoints"][0]["method_evidence"]["canonical"].update({
            value["endpoints"][0]["canonical"]: "DCC_REST_GET"
            if value["endpoints"][0]["method"] != "GET" else "DCC_REST_POST",
        }),
    )
    expect_rejected(
        "unreviewed collision justification",
        lambda value: value["endpoints"][0].update({"collision_justification": "because"}),
    )
    expect_rejected(
        "wrong transition composite owner",
        lambda value: next(iter(value["transition_composites"][0]["owners"].values())).update({
            "source": "src/rest/wrong.c",
        }),
    )
    expect_rejected(
        "wrong transition declaration metadata",
        lambda value: next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_get_current_application"
        ).update({"transition_declarations": []}),
    )
    def move_body_builder(value: dict[str, Any]) -> None:
        source = next(
            entry for entry in value["endpoints"]
            if "dcc_rest_build_application_modify_body" in entry["legacy_symbols"]
        )
        target = next(
            entry for entry in value["endpoints"]
            if entry["canonical"] == "dcc_rest_add_group_dm_recipient"
        )
        symbol = "dcc_rest_build_application_modify_body"
        source["legacy_symbols"].remove(symbol)
        owner = source["legacy_owners"].pop(symbol)
        target["legacy_symbols"].append(symbol)
        target["legacy_symbols"].sort()
        target["legacy_owners"][symbol] = owner
        target["legacy_owners"] = dict(sorted(target["legacy_owners"].items()))
    expect_rejected("wrong official body-builder mapping", move_body_builder)
    with tempfile.TemporaryDirectory(prefix="dcc-rest-v2-audit-") as directory:
        missing = Path(directory) / "missing.h"
        candidate = json.loads(json.dumps(data))
        candidate["endpoints"][0]["header"] = missing.as_posix()
        if not validate_manifest(candidate, root):
            failures.append("self-test did not reject missing manifest file")
        else:
            rejected += 1

    def expect_state_rejected(
        label: str,
        progress: int | None,
        mutate: Any,
        needle: str | None = None,
    ) -> None:
        nonlocal rejected
        declarations, definitions, internal = synthetic_inventory(data, progress)
        mutate(declarations, definitions, internal)
        state_errors, _ = audit_state_from_inventory(
            data,
            root,
            progress,
            declarations,
            definitions,
            internal,
            validate_schema=False,
            check_opaque_contract=False,
        )
        if not state_errors or (needle is not None and not any(needle in item for item in state_errors)):
            failures.append(f"self-test did not reject {label}")
        else:
            rejected += 1

    declarations, definitions, internal = synthetic_inventory(data, 6)
    state_errors, _ = audit_state_from_inventory(
        data,
        root,
        6,
        declarations,
        definitions,
        internal,
        validate_schema=False,
        check_opaque_contract=False,
    )
    if state_errors:
        failures.append("synthetic progress-through-6 inventory failed: " + " | ".join(state_errors[:3]))

    declarations, definitions, internal = synthetic_inventory(data, 7)
    state_errors, state_debt = audit_state_from_inventory(
        data,
        root,
        7,
        declarations,
        definitions,
        internal,
        validate_schema=False,
        check_opaque_contract=False,
    )
    if state_errors or sum(state_debt.values()) != 148:
        failures.append(
            "synthetic progress-through-7 inventory failed: "
            + " | ".join(state_errors[:3])
            + f" (deferred={sum(state_debt.values())})"
        )

    expect_state_rejected(
        "unclassified public declaration",
        6,
        lambda declarations, definitions, internal: declarations.update({
            "dcc_rest_unclassified_endpoint": [
                Declaration(
                    "dcc_rest_unclassified_endpoint",
                    "dcc_status_t",
                    "void",
                    "include/dcc/rest/fake.h",
                )
            ],
        }),
        "unclassified public",
    )
    expect_state_rejected(
        "unclassified external definition",
        6,
        lambda declarations, definitions, internal: definitions.update({
            "dcc_rest_unclassified_endpoint": [
                Definition(
                    "dcc_rest_unclassified_endpoint",
                    "dcc_status_t",
                    "void",
                    "src/rest/fake.c",
                    "{ }",
                )
            ],
        }),
        "neither public nor internal",
    )

    def resurrect_removed_candidate(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        candidate = data["removed_candidates"][0]
        symbol = candidate["canonical"]
        owner = candidate["owners"][symbol]
        synthetic_add_symbol(
            declarations,
            definitions,
            symbol,
            [owner["header"]],
            owner["source"],
            "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
        )

    expect_state_rejected(
        "removal-ledger symbol resurrection",
        7,
        resurrect_removed_candidate,
        "removal task",
    )

    def replace_capability_token(
        definitions: Any,
        symbol: str,
        old: str,
        new: str,
    ) -> None:
        item = definitions[symbol][0]
        definitions[symbol][0] = Definition(
            item.name,
            item.return_type,
            item.args,
            item.path,
            item.body.replace(old, new),
        )

    def wrong_task7_auth_capability(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        replace_capability_token(
            definitions,
            "dcc_rest_get_invite",
            "DCC_ENDPOINT_AUTH_POLICY_NONE_OR_BOT",
            "DCC_ENDPOINT_AUTH_POLICY_BOT",
        )

    expect_state_rejected(
        "source auth capability mismatch",
        7,
        wrong_task7_auth_capability,
        "AUTH_POLICY_NONE_OR_BOT",
    )

    def wrong_task7_audit_capability(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        replace_capability_token(
            definitions,
            "dcc_rest_modify_guild_channel_positions",
            "DCC_ENDPOINT_AUDIT_REASON_DENIED",
            "DCC_ENDPOINT_AUDIT_REASON_ALLOWED",
        )

    expect_state_rejected(
        "source audit capability mismatch",
        7,
        wrong_task7_audit_capability,
        "AUDIT_REASON_DENIED",
    )

    def wrong_sensitive_path_capability(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        replace_capability_token(
            definitions,
            "dcc_rest_interaction_response_create",
            "DCC_ENDPOINT_PATH_SENSITIVE",
            "DCC_ENDPOINT_PATH_PUBLIC",
        )

    expect_state_rejected(
        "source sensitive-path capability mismatch",
        7,
        wrong_sensitive_path_capability,
        "PATH_SENSITIVE",
    )

    def missing_opaque_route_key_capability(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        replace_capability_token(
            definitions,
            "dcc_rest_execute_webhook",
            "DCC_ENDPOINT_ROUTE_KEY_OPAQUE",
            "DCC_ENDPOINT_ROUTE_KEY_RAW",
        )

    expect_state_rejected(
        "sensitive route key is not opaque",
        7,
        missing_opaque_route_key_capability,
        "ROUTE_KEY_OPAQUE",
    )

    def raw_sensitive_operation_identity(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        replace_capability_token(
            definitions,
            "dcc_rest_interaction_followup_get",
            '"dcc_rest_interaction_followup_get"',
            '"/webhooks/123/raw-secret/messages/456"',
        )

    expect_state_rejected(
        "sensitive observer operation is a raw path",
        7,
        raw_sensitive_operation_identity,
        '"dcc_rest_interaction_followup_get"',
    )

    first_task6 = next(entry for entry in data["endpoints"] if entry["task"] == 6)
    def duplicate_declaration(declarations: Any, definitions: Any, internal: Any) -> None:
        declarations[first_task6["canonical"]].append(declarations[first_task6["canonical"]][0])
    expect_state_rejected("duplicate public declaration", 6, duplicate_declaration)

    def duplicate_definition(declarations: Any, definitions: Any, internal: Any) -> None:
        definitions[first_task6["canonical"]].append(definitions[first_task6["canonical"]][0])
    expect_state_rejected("duplicate external definition", 6, duplicate_definition)

    later_with_legacy = next(
        entry for entry in data["endpoints"]
        if entry["task"] > 6 and entry["legacy_symbols"]
    )
    def wrong_later_owner(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = later_with_legacy["legacy_symbols"][0]
        item = declarations[symbol][0]
        declarations[symbol][0] = Declaration(
            item.name,
            item.return_type,
            item.args,
            "include/dcc/rest/wrong.h",
        )
    expect_state_rejected("wrong later-task owner", 6, wrong_later_owner, "matches neither")

    def missing_later_symbol(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = later_with_legacy["legacy_symbols"][0]
        declarations.pop(symbol)
        definitions.pop(symbol)
    expect_state_rejected("invented or missing later-task legacy symbol", 6, missing_later_symbol)

    def lingering_task6_overload(declarations: Any, definitions: Any, internal: Any) -> None:
        legacy = first_task6["legacy_symbols"][0]
        owner = first_task6["legacy_owners"][legacy]
        synthetic_add_symbol(
            declarations,
            definitions,
            legacy,
            [owner["header"]],
            owner["source"],
            "dcc_client_t *client, dcc_rest_cb cb, void *user_data",
        )
    task6_with_legacy = next(
        entry for entry in data["endpoints"]
        if entry["task"] == 6 and entry["legacy_symbols"]
    )
    first_task6 = task6_with_legacy
    expect_state_rejected("lingering Task 6 overload", 6, lingering_task6_overload)

    def raw_task6_signature(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = first_task6["canonical"]
        args = "dcc_client_t *client, const char *json_body, dcc_rest_cb cb, void *user_data"
        declarations[symbol][0] = Declaration(
            symbol,
            declarations[symbol][0].return_type,
            args,
            declarations[symbol][0].path,
        )
        item = definitions[symbol][0]
        definitions[symbol][0] = Definition(
            symbol,
            item.return_type,
            args,
            item.path,
            item.body,
        )
    expect_state_rejected("Task 6 raw callback signature", 6, raw_task6_signature, "callback/user_data")

    def void_canonical_declaration(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = first_task6["canonical"]
        item = declarations[symbol][0]
        declarations[symbol][0] = Declaration(
            item.name,
            "void",
            item.args,
            item.path,
        )
    expect_state_rejected(
        "void canonical declaration return",
        6,
        void_canonical_declaration,
        "expected dcc_status_t",
    )

    def pointer_canonical_definition(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = first_task6["canonical"]
        item = definitions[symbol][0]
        definitions[symbol][0] = Definition(
            item.name,
            "char *",
            item.args,
            item.path,
            item.body,
        )
    expect_state_rejected(
        "pointer canonical definition return",
        6,
        pointer_canonical_definition,
        "expected dcc_status_t",
    )

    scalar_task6 = next(
        entry for entry in data["endpoints"]
        if entry["task"] == 6 and entry["input"] is None
    )
    def disguised_raw_payload(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol = scalar_task6["canonical"]
        args = (
            "dcc_client_t *client, const char *payload, "
            "const dcc_rest_call_options_t *options, dcc_rest_request_t **out_request"
        )
        declarations[symbol][0] = Declaration(
            symbol,
            declarations[symbol][0].return_type,
            args,
            declarations[symbol][0].path,
        )
        item = definitions[symbol][0]
        definitions[symbol][0] = Definition(
            symbol,
            item.return_type,
            args,
            item.path,
            item.body,
        )
    expect_state_rejected(
        "disguised raw scalar payload",
        6,
        disguised_raw_payload,
        "raw endpoint JSON/query input",
    )

    def add_task6_signature_parameters(parameters: list[str]) -> Any:
        def mutate(declarations: Any, definitions: Any, internal: Any) -> None:
            symbol = scalar_task6["canonical"]
            contract = [
                parameter["declaration"]
                for parameter in scalar_task6["canonical_parameters"]
            ]
            args = ", ".join([*contract[:-2], *parameters, *contract[-2:]])
            declaration = declarations[symbol][0]
            declarations[symbol][0] = Declaration(
                symbol,
                declaration.return_type,
                args,
                declaration.path,
            )
            definition = definitions[symbol][0]
            definitions[symbol][0] = Definition(
                symbol,
                definition.return_type,
                args,
                definition.path,
                definition.body,
            )
        return mutate

    disguised_parameter_sets = {
        "wire payload": ["const char *wire_payload"],
        "payload bytes": ["const char *payload_bytes"],
        "reordered const payload": ["char const *payload"],
        "byte body": ["const uint8_t *body"],
        "void request payload": [
            "const void *request_payload",
            "size_t request_payload_len",
        ],
        "meaningless pointer count": [
            "const dcc_snowflake_t *items",
            "size_t item_count",
        ],
    }
    for label, parameters in disguised_parameter_sets.items():
        expect_state_rejected(
            f"Task 6 unapproved {label} parameters",
            6,
            add_task6_signature_parameters(parameters),
            "exact manifest contract",
        )

    def wrong_route_evidence(declarations: Any, definitions: Any, internal: Any) -> None:
        symbol, tokens = next(iter(first_task6["route_evidence"]["canonical"].items()))
        token = tokens[0]
        item = definitions[symbol][0]
        definitions[symbol][0] = Definition(
            item.name,
            item.return_type,
            item.args,
            item.path,
            item.body.replace(synthetic_evidence_source(token), "DCC_REST_ROUTE_WRONG"),
        )
    expect_state_rejected("route evidence outside exact body", 6, wrong_route_evidence, "route_evidence token")

    get_message_entry = next(
        entry for entry in data["endpoints"]
        if entry["canonical"] == "dcc_rest_get_message"
    )
    def replace_get_message_route(replacement: str) -> Any:
        def mutate(declarations: Any, definitions: Any, internal: Any) -> None:
            symbol = get_message_entry["canonical"]
            item = definitions[symbol][0]
            definitions[symbol][0] = Definition(
                item.name,
                item.return_type,
                item.args,
                item.path,
                item.body.replace("DCC_REST_ROUTE_CHANNEL_MESSAGE", replacement),
            )
        return mutate

    for label, replacement in {
        "route identifier extension": "DCC_REST_ROUTE_CHANNEL_MESSAGE_PIN",
        "route identifier prefix": "PREFIX_DCC_REST_ROUTE_CHANNEL_MESSAGE",
        "route macro literal suffix": 'DCC_REST_ROUTE_CHANNEL_MESSAGE "/wrong"',
        "route macro identifier suffix": (
            "DCC_REST_ROUTE_CHANNEL_MESSAGE DCC_REST_ROUTE_SUFFIX"
        ),
    }.items():
        expect_state_rejected(
            label,
            6,
            replace_get_message_route(replacement),
            "route_evidence token",
        )

    def baseline_branch_mutator(canonical: str, token: str) -> Any:
        entry = next(item for item in data["endpoints"] if item["canonical"] == canonical)
        def mutate(declarations: Any, definitions: Any, internal: Any) -> None:
            synthetic_set_endpoint_state(
                entry,
                declarations,
                definitions,
                canonical=False,
            )
            symbol = next(
                symbol for symbol, tokens in entry["route_evidence"]["baseline"].items()
                if token in tokens
            )
            item = definitions[symbol][0]
            definitions[symbol][0] = Definition(
                item.name,
                item.return_type,
                item.args,
                item.path,
                item.body.replace(synthetic_evidence_source(token), "ROUTE_BRANCH_DRIFT"),
            )
        return mutate

    expect_state_rejected(
        "reaction suffix route drift",
        6,
        baseline_branch_mutator("dcc_rest_add_message_reaction", '"/@me"'),
        'route_evidence token \'"/@me"\'',
    )
    expect_state_rejected(
        "optional webhook no-token route drift",
        6,
        baseline_branch_mutator("dcc_rest_get_webhook", "/webhooks/%llu"),
        "route_evidence token '/webhooks/%llu'",
    )
    expect_state_rejected(
        "interaction original suffix route drift",
        6,
        baseline_branch_mutator(
            "dcc_rest_interaction_original_response_get",
            '"messages/@original"',
        ),
        "messages/@original",
    )
    expect_state_rejected(
        "compat webhook suffix route drift",
        6,
        baseline_branch_mutator("dcc_rest_execute_webhook_github", '"github"'),
        "github",
    )

    def migrate_future_domain(declarations: Any, definitions: Any, internal: Any) -> None:
        for entry in data["endpoints"]:
            if entry["task"] == 7 and entry["domain"] == "channels":
                synthetic_set_endpoint_state(
                    entry,
                    declarations,
                    definitions,
                    canonical=True,
                )
    expect_state_rejected(
        "stale per-domain progress allowance",
        6,
        migrate_future_domain,
        "task 7 domain channels",
    )

    def complete_task7_under_stale_progress(
        declarations: Any,
        definitions: Any,
        internal: Any,
    ) -> None:
        for entry in data["endpoints"]:
            if entry["task"] == 7:
                synthetic_set_endpoint_state(
                    entry,
                    declarations,
                    definitions,
                    canonical=True,
                )
        for candidate in data["removed_candidates"]:
            for symbol in candidate["owners"]:
                declarations.pop(symbol, None)
                definitions.pop(symbol, None)

    expect_state_rejected(
        "stale progress-through-6 after complete Task 7 migration",
        6,
        complete_task7_under_stale_progress,
        "task 7 domain",
    )

    def stale_task9_composite(declarations: Any, definitions: Any, internal: Any) -> None:
        composite = next(item for item in data["transition_composites"] if item["removal_task"] == 9)
        symbol = composite["symbols"][0]
        owner = composite["owners"][symbol]
        synthetic_add_symbol(
            declarations,
            definitions,
            symbol,
            [owner["header"]],
            owner["source"],
            "dcc_client_t *client",
        )
    expect_state_rejected("stale Task 9 composite", 9, stale_task9_composite, "removal task")

    def stale_task10_helper(declarations: Any, definitions: Any, internal: Any) -> None:
        helper = data["transition_helpers"][0]
        synthetic_add_symbol(
            declarations,
            definitions,
            helper["symbol"],
            [helper["header"]],
            helper["source"],
            "char *json",
        )
    expect_state_rejected("stale Task 10 transition helper", 10, stale_task10_helper, "removal task")

    run_self_tests.rejected = rejected
    return failures


def main() -> int:
    args = parse_args()
    root = args.source.resolve()
    manifest_path = (args.manifest or root / "tools/rest_v2_endpoints.json").resolve()
    try:
        data = load_manifest(manifest_path)
    except (OSError, ValueError) as exc:
        print(f"DCC REST v2 endpoint audit failed: {exc}", file=sys.stderr)
        return 1

    if args.self_test:
        failures = run_self_tests(data, root)
        if failures:
            print("DCC REST v2 endpoint audit self-tests failed:", file=sys.stderr)
            print("- " + "\n- ".join(failures), file=sys.stderr)
            return 1
        rejected = getattr(run_self_tests, "rejected", 0)
        print(
            "DCC REST v2 endpoint audit self-tests passed "
            f"({rejected} malformed manifest/state fixtures rejected)"
        )
        return 0

    errors, debt = audit_state(data, root, args.progress_through)
    if errors:
        print("DCC REST v2 endpoint audit failed:", file=sys.stderr)
        preview = errors[:80]
        print("- " + "\n- ".join(preview), file=sys.stderr)
        if len(errors) > len(preview):
            print(f"- ... {len(errors) - len(preview)} additional violation(s)", file=sys.stderr)
        return 1
    mode = "strict" if args.progress_through is None else f"progress-through-{args.progress_through}"
    counts = Counter(entry["task"] for entry in data["endpoints"])
    summary = ", ".join(f"task {task}={counts[task]}" for task in sorted(counts))
    print(
        f"DCC REST v2 endpoint audit passed ({mode}; {len(data['endpoints'])} endpoints; "
        f"{summary}; deferred violations={sum(debt.values())})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
