#!/usr/bin/env python3
"""Audit DCC's official Discord API surface additions."""

from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

PUBLIC_FILES = [
    ROOT / "include/dcc/rest/official_surface.h",
    ROOT / "include/dcc/oauth2.h",
    ROOT / "include/dcc/webhook_events.h",
    ROOT / "include/dcc/client.h",
]
OBJECT_PUBLIC_FILES = [
    ROOT / "include/dcc/objects/resources/applications.h",
    ROOT / "include/dcc/objects/resources/lobbies.h",
]
EXISTING_PUBLIC_FILES = [
    ROOT / "include/dcc/rest/messages/pins.h",
]
SOURCE_FILES = [
    ROOT / "src/rest/rest_official_surface.c",
    ROOT / "src/rest/rest_users_current.c",
    ROOT / "src/oauth2.c",
    ROOT / "src/webhook_events.c",
    ROOT / "src/gateway/gateway_send_public.c",
]
EXISTING_SOURCE_FILES = [
    ROOT / "src/rest/rest_message_pins.c",
]
SUGAR_FILE = ROOT / "include/dcc/sugar/official_surface.h"
REST_AGGREGATE_FILE = ROOT / "include/dcc/rest.h"
DCC_AGGREGATE_FILE = ROOT / "include/dcc/dcc.h"
SUGAR_AGGREGATE_FILE = ROOT / "include/dcc/sugar.h"
OBJECT_RESOURCES_AGGREGATE_FILE = ROOT / "include/dcc/objects/resources.h"
DOC_FILE = ROOT / "docs/reference/official-api-surface.md"
MKDOCS_FILE = ROOT / "mkdocs.yml"
PACKAGE_CONSUMER_FILE = ROOT / "tests/package_consumer/package_consumer_official_surface.c"
HTTP_SMOKE_FILE = ROOT / "tests/support/http_smoke_official_surface.c"
CMAKE_FILE = ROOT / "CMakeLists.txt"

AGGREGATE_INCLUDES = {
    REST_AGGREGATE_FILE: [
        "#include <dcc/rest/official_surface.h>",
    ],
    DCC_AGGREGATE_FILE: [
        "#include <dcc/oauth2.h>",
        "#include <dcc/webhook_events.h>",
    ],
    SUGAR_AGGREGATE_FILE: [
        "#include <dcc/sugar/official_surface.h>",
    ],
    OBJECT_RESOURCES_AGGREGATE_FILE: [
        "#include <dcc/objects/resources/applications.h>",
        "#include <dcc/objects/resources/lobbies.h>",
    ],
}

DOC_NAV_ENTRIES = [
    "Official Discord Surface: reference/official-api-surface.md",
]

PACKAGE_CONSUMER_INCLUDES = [
    "#include <dcc/oauth2.h>",
    "#include <dcc/objects/resources/applications.h>",
    "#include <dcc/objects/resources/lobbies.h>",
    "#include <dcc/rest/official_surface.h>",
    "#include <dcc/sugar/official_surface.h>",
    "#include <dcc/webhook_events.h>",
]

CMAKE_SOURCE_ENTRIES = [
    "src/gateway/gateway_send_public.c",
    "src/oauth2.c",
    "src/rest/rest_official_surface.c",
    "src/webhook_events.c",
]
CMAKE_TEST_ENTRIES = [
    "dcc_official_surface_headers_smoke",
    "tests/official_surface_headers/applications_header.c",
    "tests/official_surface_headers/lobbies_header.c",
    "tests/official_surface_headers/main.c",
    "tests/official_surface_headers/oauth2_header.c",
    "tests/official_surface_headers/rest_header.c",
    "tests/official_surface_headers/sugar_header.c",
    "tests/official_surface_headers/webhook_events_header.c",
]

PUBLIC_SYMBOLS = [
    "dcc_rest_get_gateway",
    "dcc_rest_get_current_application",
    "dcc_rest_modify_current_application",
    "dcc_rest_build_application_modify_body",
    "dcc_rest_modify_current_application_params",
    "dcc_rest_get_activity_instance",
    "dcc_rest_build_soundboard_send_body",
    "dcc_rest_send_soundboard_sound",
    "dcc_rest_send_soundboard_sound_params",
    "dcc_rest_get_default_soundboard_sounds",
    "dcc_rest_get_guild_soundboard_sounds",
    "dcc_rest_get_guild_soundboard_sound",
    "dcc_rest_create_guild_soundboard_sound",
    "dcc_rest_build_guild_soundboard_sound_create_body",
    "dcc_rest_create_guild_soundboard_sound_params",
    "dcc_rest_modify_guild_soundboard_sound",
    "dcc_rest_build_guild_soundboard_sound_modify_body",
    "dcc_rest_modify_guild_soundboard_sound_params",
    "dcc_rest_delete_guild_soundboard_sound",
    "dcc_rest_get_sku_subscriptions",
    "dcc_rest_get_sku_subscription",
    "dcc_rest_build_bulk_ban_body",
    "dcc_rest_bulk_ban_guild_members",
    "dcc_rest_bulk_ban_guild_members_params",
    "dcc_rest_get_guild_role",
    "dcc_rest_get_guild_role_member_counts",
    "dcc_rest_get_guild_widget_json",
    "dcc_rest_get_guild_widget_png",
    "dcc_rest_build_guild_incident_actions_body",
    "dcc_rest_modify_guild_incident_actions",
    "dcc_rest_modify_guild_incident_actions_params",
    "dcc_rest_get_invite_target_users",
    "dcc_rest_put_invite_target_users",
    "dcc_rest_get_invite_target_users_job_status",
    "dcc_rest_get_entitlement",
    "dcc_rest_get_sticker_pack",
    "dcc_rest_get_current_user_guild_member",
    "dcc_rest_delete_current_user_application_role_connection",
    "dcc_rest_execute_webhook_slack",
    "dcc_rest_execute_webhook_github",
    "dcc_rest_create_lobby",
    "dcc_rest_build_lobby_body",
    "dcc_rest_create_lobby_params",
    "dcc_rest_create_or_join_lobby",
    "dcc_rest_build_lobby_create_or_join_body",
    "dcc_rest_create_or_join_lobby_params",
    "dcc_rest_get_lobby",
    "dcc_rest_modify_lobby",
    "dcc_rest_modify_lobby_params",
    "dcc_rest_delete_lobby",
    "dcc_rest_add_lobby_member",
    "dcc_rest_build_lobby_member_body",
    "dcc_rest_add_lobby_member_params",
    "dcc_rest_bulk_update_lobby_members",
    "dcc_rest_build_lobby_member_bulk_body",
    "dcc_rest_bulk_update_lobby_members_params",
    "dcc_rest_remove_lobby_member",
    "dcc_rest_leave_lobby",
    "dcc_rest_link_lobby_channel",
    "dcc_rest_build_lobby_channel_link_body",
    "dcc_rest_link_lobby_channel_params",
    "dcc_rest_unlink_lobby_channel",
    "dcc_rest_send_lobby_message",
    "dcc_rest_build_lobby_message_body",
    "dcc_rest_send_lobby_message_params",
    "dcc_rest_get_lobby_messages",
    "dcc_rest_update_lobby_message_moderation_metadata",
    "dcc_rest_build_lobby_message_moderation_metadata_body",
    "dcc_rest_update_lobby_message_moderation_metadata_params",
    "dcc_rest_create_lobby_channel_invite_for_self",
    "dcc_rest_create_lobby_channel_invite_for_user",
    "dcc_rest_official_body_json_free",
    "dcc_oauth2_get_current_authorization",
    "dcc_oauth2_get_current_bot_application",
    "dcc_oauth2_exchange_code",
    "dcc_oauth2_refresh_token",
    "dcc_oauth2_revoke_token",
    "dcc_client_request_guild_members",
    "dcc_client_request_soundboard_sounds",
    "dcc_client_request_channel_info",
    "dcc_client_update_presence",
    "dcc_client_update_presence_raw",
    "dcc_webhook_event_type_name",
    "dcc_webhook_event_type_from_string",
    "dcc_webhook_event_parse",
    "dcc_webhook_events_add_route",
]

OBJECT_SYMBOLS = [
    "dcc_activity_location_kind_t",
    "DCC_ACTIVITY_LOCATION_GUILD_CHANNEL",
    "DCC_ACTIVITY_LOCATION_PRIVATE_CHANNEL",
    "dcc_activity_location_t",
    "dcc_activity_instance_t",
    "dcc_lobby_member_flag_t",
    "DCC_LOBBY_MEMBER_CAN_LINK_LOBBY",
    "dcc_lobby_member_t",
    "dcc_lobby_t",
]

EXISTING_OFFICIAL_SYMBOLS = [
    "dcc_rest_legacy_pin_message",
    "dcc_rest_legacy_unpin_message",
    "dcc_rest_get_legacy_channel_pins",
]

RAW_BODY_TYPED_COMPANIONS = {
    "dcc_rest_modify_current_application": "dcc_rest_modify_current_application_params",
    "dcc_rest_send_soundboard_sound": "dcc_rest_send_soundboard_sound_params",
    "dcc_rest_create_guild_soundboard_sound": "dcc_rest_create_guild_soundboard_sound_params",
    "dcc_rest_modify_guild_soundboard_sound": "dcc_rest_modify_guild_soundboard_sound_params",
    "dcc_rest_bulk_ban_guild_members": "dcc_rest_bulk_ban_guild_members_params",
    "dcc_rest_modify_guild_incident_actions": "dcc_rest_modify_guild_incident_actions_params",
    "dcc_rest_create_lobby": "dcc_rest_create_lobby_params",
    "dcc_rest_create_or_join_lobby": "dcc_rest_create_or_join_lobby_params",
    "dcc_rest_modify_lobby": "dcc_rest_modify_lobby_params",
    "dcc_rest_add_lobby_member": "dcc_rest_add_lobby_member_params",
    "dcc_rest_bulk_update_lobby_members": "dcc_rest_bulk_update_lobby_members_params",
    "dcc_rest_link_lobby_channel": "dcc_rest_link_lobby_channel_params",
    "dcc_rest_send_lobby_message": "dcc_rest_send_lobby_message_params",
    "dcc_rest_update_lobby_message_moderation_metadata":
        "dcc_rest_update_lobby_message_moderation_metadata_params",
}

RAW_BODY_ESCAPE_HATCHES = [
    "dcc_rest_execute_webhook_slack",
    "dcc_rest_execute_webhook_github",
]

BODY_BUILDER_EDGE_SMOKE_TOKENS = [
    "run_public_rest_official_body_builder_edges",
    "application_modify_escape_edges",
    "incident_nullable_edges",
    "lobby_message_raw_metadata_edges",
    "lobby_moderation_metadata_escape_edges",
    "DCC \\\"runtime\\\" \\\\ core",
    "decision\\\\\\\"key",
    "allow\\\\\\\\value",
]

SUGAR_MACROS = [
    "DCC_GATEWAY_MEMBERS",
    "DCC_GATEWAY_PRESENCE",
    "DCC_OAUTH2_EXCHANGE_CODE_OPTIONS",
    "DCC_OAUTH2_REFRESH_TOKEN_OPTIONS",
    "DCC_OAUTH2_REVOKE_TOKEN_OPTIONS",
    "DCC_APPLICATION_INSTALL_PARAMS",
    "DCC_APPLICATION_INTEGRATION_CONFIG",
    "DCC_APPLICATION_MODIFY",
    "DCC_SOUNDBOARD_SEND",
    "DCC_GUILD_SOUNDBOARD_SOUND",
    "DCC_BULK_BAN_BODY",
    "DCC_INCIDENT_ACTIONS_BODY",
    "DCC_LOBBY_CREATE_OR_JOIN",
    "DCC_LOBBY_MEMBER_SPEC",
    "DCC_LOBBY",
    "DCC_LOBBY_LINK_CHANNEL",
    "DCC_LOBBY_MEMBER",
    "DCC_LOBBY_BULK_MEMBER",
    "DCC_LOBBY_BULK_MEMBERS",
    "DCC_LOBBY_MESSAGE",
    "DCC_LOBBY_MODERATION_METADATA_ITEM",
    "DCC_LOBBY_MODERATION_METADATA",
    "DCC_WEBHOOK_EVENT_ROUTE",
    "DCC_WEBHOOK_EVENT_ROUTE_UNSIGNED",
    "DCC_REST_GATEWAY",
    "DCC_REST_GATEWAY_BOT",
    "DCC_REST_CURRENT_APPLICATION",
    "DCC_REST_MODIFY_CURRENT_APPLICATION",
    "DCC_REST_MODIFY_CURRENT_APPLICATION_PARAMS",
    "DCC_REST_ACTIVITY_INSTANCE",
    "DCC_REST_SEND_SOUNDBOARD",
    "DCC_REST_SEND_SOUNDBOARD_PARAMS",
    "DCC_REST_DEFAULT_SOUNDBOARD_SOUNDS",
    "DCC_REST_GUILD_SOUNDBOARD_SOUNDS",
    "DCC_REST_GUILD_SOUNDBOARD_SOUND",
    "DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND",
    "DCC_REST_CREATE_GUILD_SOUNDBOARD_SOUND_PARAMS",
    "DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND",
    "DCC_REST_MODIFY_GUILD_SOUNDBOARD_SOUND_PARAMS",
    "DCC_REST_DELETE_GUILD_SOUNDBOARD_SOUND",
    "DCC_REST_SKU_SUBSCRIPTIONS",
    "DCC_REST_SKU_SUBSCRIPTION",
    "DCC_REST_BULK_BAN",
    "DCC_REST_BULK_BAN_PARAMS",
    "DCC_REST_GUILD_ROLE",
    "DCC_REST_ROLE_MEMBER_COUNTS",
    "DCC_REST_WIDGET_JSON",
    "DCC_REST_WIDGET_PNG",
    "DCC_REST_INCIDENT_ACTIONS",
    "DCC_REST_INCIDENT_ACTIONS_PARAMS",
    "DCC_REST_INVITE_TARGET_USERS",
    "DCC_REST_PUT_INVITE_TARGET_USERS",
    "DCC_REST_INVITE_TARGET_USERS_JOB",
    "DCC_REST_ENTITLEMENT",
    "DCC_REST_STICKER_PACK",
    "DCC_REST_CURRENT_USER_GUILD_MEMBER",
    "DCC_REST_DELETE_USER_ROLE_CONNECTION",
    "DCC_REST_WEBHOOK_SLACK",
    "DCC_REST_WEBHOOK_GITHUB",
    "DCC_REST_LEGACY_PIN_MESSAGE",
    "DCC_REST_LEGACY_UNPIN_MESSAGE",
    "DCC_REST_LEGACY_CHANNEL_PINS",
    "DCC_REST_CREATE_LOBBY",
    "DCC_REST_CREATE_LOBBY_PARAMS",
    "DCC_REST_CREATE_OR_JOIN_LOBBY",
    "DCC_REST_CREATE_OR_JOIN_LOBBY_PARAMS",
    "DCC_REST_LOBBY",
    "DCC_REST_MODIFY_LOBBY",
    "DCC_REST_MODIFY_LOBBY_PARAMS",
    "DCC_REST_DELETE_LOBBY",
    "DCC_REST_ADD_LOBBY_MEMBER",
    "DCC_REST_ADD_LOBBY_MEMBER_PARAMS",
    "DCC_REST_BULK_UPDATE_LOBBY_MEMBERS",
    "DCC_REST_BULK_UPDATE_LOBBY_MEMBERS_PARAMS",
    "DCC_REST_REMOVE_LOBBY_MEMBER",
    "DCC_REST_LEAVE_LOBBY",
    "DCC_REST_LINK_LOBBY_CHANNEL",
    "DCC_REST_LINK_LOBBY_CHANNEL_PARAMS",
    "DCC_REST_UNLINK_LOBBY_CHANNEL",
    "DCC_REST_SEND_LOBBY_MESSAGE",
    "DCC_REST_SEND_LOBBY_MESSAGE_PARAMS",
    "DCC_REST_LOBBY_MESSAGES",
    "DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION",
    "DCC_REST_UPDATE_LOBBY_MESSAGE_MODERATION_PARAMS",
    "DCC_REST_CREATE_LOBBY_INVITE_SELF",
    "DCC_REST_CREATE_LOBBY_INVITE_USER",
    "DCC_GATEWAY_REQUEST_MEMBERS",
    "DCC_GATEWAY_REQUEST_SOUNDBOARD",
    "DCC_GATEWAY_REQUEST_CHANNEL_INFO",
    "DCC_GATEWAY_UPDATE_PRESENCE",
    "DCC_GATEWAY_UPDATE_PRESENCE_RAW",
    "DCC_OAUTH2_ME",
    "DCC_OAUTH2_CURRENT_BOT_APPLICATION",
    "DCC_OAUTH2_EXCHANGE_CODE",
    "DCC_OAUTH2_REFRESH_TOKEN",
    "DCC_OAUTH2_REVOKE_TOKEN",
    "DCC_WEBHOOK_EVENTS_ADD_ROUTE",
]


def read_all(paths: list[Path]) -> str:
    return "\n".join(path.read_text(encoding="utf-8") for path in paths)


def main() -> int:
    public_text = read_all(PUBLIC_FILES)
    object_public_text = read_all(OBJECT_PUBLIC_FILES)
    existing_public_text = read_all(EXISTING_PUBLIC_FILES)
    source_text = read_all(SOURCE_FILES)
    existing_source_text = read_all(EXISTING_SOURCE_FILES)
    sugar_text = SUGAR_FILE.read_text(encoding="utf-8")
    doc_text = DOC_FILE.read_text(encoding="utf-8")
    mkdocs_text = MKDOCS_FILE.read_text(encoding="utf-8")
    package_consumer_text = PACKAGE_CONSUMER_FILE.read_text(encoding="utf-8")
    http_smoke_text = HTTP_SMOKE_FILE.read_text(encoding="utf-8")
    cmake_text = CMAKE_FILE.read_text(encoding="utf-8")

    missing: list[str] = []
    for aggregate_file, includes in AGGREGATE_INCLUDES.items():
        aggregate_text = aggregate_file.read_text(encoding="utf-8")
        for include in includes:
            if include not in aggregate_text:
                missing.append(f"aggregate include missing in {aggregate_file.relative_to(ROOT)}: {include}")

    for source_entry in CMAKE_SOURCE_ENTRIES:
        if source_entry not in cmake_text:
            missing.append(f"CMake source entry missing: {source_entry}")

    for test_entry in CMAKE_TEST_ENTRIES:
        if test_entry not in cmake_text:
            missing.append(f"CMake official surface header smoke entry missing: {test_entry}")

    for include in PACKAGE_CONSUMER_INCLUDES:
        if include not in package_consumer_text:
            missing.append(f"package consumer direct include missing: {include}")

    for symbol in PUBLIC_SYMBOLS:
        if symbol not in public_text:
            missing.append(f"public declaration missing: {symbol}")
        if symbol not in source_text:
            missing.append(f"source implementation missing: {symbol}")
        if symbol not in doc_text:
            missing.append(f"docs mention missing: {symbol}")
        if symbol not in package_consumer_text:
            missing.append(f"package consumer official-surface mention missing: {symbol}")

    for symbol in OBJECT_SYMBOLS:
        if symbol not in object_public_text:
            missing.append(f"public object declaration missing: {symbol}")
        if symbol not in doc_text:
            missing.append(f"docs mention missing for public object: {symbol}")
        if symbol not in package_consumer_text:
            missing.append(f"package consumer public object mention missing: {symbol}")

    for symbol in EXISTING_OFFICIAL_SYMBOLS:
        if symbol not in existing_public_text:
            missing.append(f"existing public declaration missing: {symbol}")
        if symbol not in existing_source_text:
            missing.append(f"existing source implementation missing: {symbol}")
        if symbol not in doc_text:
            missing.append(f"docs mention missing for existing official wrapper: {symbol}")
        if symbol not in package_consumer_text:
            missing.append(f"package consumer existing official wrapper missing: {symbol}")

    for raw_symbol, typed_symbol in RAW_BODY_TYPED_COMPANIONS.items():
        if raw_symbol not in public_text:
            missing.append(f"raw body wrapper missing: {raw_symbol}")
        if typed_symbol not in public_text:
            missing.append(f"typed companion missing for {raw_symbol}: {typed_symbol}")
        if typed_symbol not in doc_text:
            missing.append(f"docs typed companion mention missing: {typed_symbol}")

    for symbol in RAW_BODY_ESCAPE_HATCHES:
        if symbol not in public_text:
            missing.append(f"raw escape hatch missing: {symbol}")
        if symbol not in doc_text:
            missing.append(f"docs raw escape hatch mention missing: {symbol}")

    for token in BODY_BUILDER_EDGE_SMOKE_TOKENS:
        if token not in http_smoke_text:
            missing.append(f"official surface body builder edge smoke missing: {token}")

    for macro in SUGAR_MACROS:
        if macro not in sugar_text:
            missing.append(f"sugar macro missing: {macro}")
        if macro not in doc_text:
            missing.append(f"docs sugar mention missing: {macro}")
        if macro not in package_consumer_text:
            missing.append(f"package consumer sugar mention missing: {macro}")

    for nav_entry in DOC_NAV_ENTRIES:
        if nav_entry not in mkdocs_text:
            missing.append(f"mkdocs nav mention missing: {nav_entry}")

    if missing:
        print("official surface audit failed:", file=sys.stderr)
        for item in missing:
            print(f"  - {item}", file=sys.stderr)
        return 1
    print(
        "official surface audit passed: "
        f"{len(PUBLIC_SYMBOLS)} new symbols, "
        f"{len(EXISTING_OFFICIAL_SYMBOLS)} existing symbols, "
        f"{len(OBJECT_SYMBOLS)} object symbols, "
        f"{len(SUGAR_MACROS)} sugar macros"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
