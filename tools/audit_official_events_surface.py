#!/usr/bin/env python3
"""Audit DCC Gateway send and outgoing Webhook Events coverage."""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DOCS_ROOT = Path(os.environ.get("DCC_DISCORD_API_DOCS_ROOT", "/tmp/discord-api-docs"))

GATEWAY_DOC = DOCS_ROOT / "developers/events/gateway-events.mdx"
OPCODES_DOC = DOCS_ROOT / "developers/topics/opcodes-and-status-codes.mdx"
WEBHOOK_DOC = DOCS_ROOT / "developers/events/webhook-events.mdx"

PUBLIC_HEADER_FILES = [
    ROOT / "include/dcc/client.h",
    ROOT / "include/dcc/webhook_events.h",
]
SOURCE_FILES = [
    ROOT / "src/gateway/gateway_heartbeat.c",
    ROOT / "src/gateway/gateway_identify.c",
    ROOT / "src/gateway/gateway_payload.c",
    ROOT / "src/gateway/gateway_resume.c",
    ROOT / "src/gateway/gateway_send_public.c",
    ROOT / "src/gateway/gateway_voice_state_update.c",
    ROOT / "src/client/client_gateway_control.c",
    ROOT / "src/webhook_events.c",
]
SMOKE_FILES = [
    ROOT / "tests/support/http_smoke_official_surface.c",
    ROOT / "tests/package_consumer/package_consumer_official_surface.c",
]
DOC_FILE = ROOT / "docs/reference/official-api-surface.md"

GATEWAY_EVENT_COVERAGE = {
    "Identify": ["dcc_gateway_send_identify"],
    "Resume": ["dcc_gateway_send_resume"],
    "Heartbeat": ["dcc_gateway_send_heartbeat"],
    "Request Guild Members": ["dcc_client_request_guild_members"],
    "Request Soundboard Sounds": ["dcc_client_request_soundboard_sounds"],
    "Request Channel Info": ["dcc_client_request_channel_info"],
    "Update Voice State": ["dcc_client_update_voice_state"],
    "Update Presence": ["dcc_client_update_presence", "dcc_client_update_presence_raw"],
}

GATEWAY_EVENT_OPCODE_NAMES = {
    "Identify": "Identify",
    "Resume": "Resume",
    "Heartbeat": "Heartbeat",
    "Request Guild Members": "Request Guild Members",
    "Request Soundboard Sounds": "Request Soundboard Sounds",
    "Request Channel Info": "Request Channel Info",
    "Update Voice State": "Voice State Update",
    "Update Presence": "Presence Update",
}

HEADING_RE = re.compile(r"^(#{2,6})\s+(.+?)\s*$", re.M)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_all(paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths)


def markdown_section(text: str, heading: str, level: str = "##") -> str:
    pattern = re.compile(rf"^{re.escape(level)}\s+{re.escape(heading)}\s*$", re.M)
    match = pattern.search(text)
    if not match:
        raise RuntimeError(f"could not locate {heading} section")

    next_heading = re.search(rf"^{re.escape(level)}\s+.+$", text[match.end():], re.M)
    end = match.end() + next_heading.start() if next_heading else len(text)
    return text[match.end():end]


def append_unique(values: list[str], value: str) -> None:
    if value not in values:
        values.append(value)


def parse_gateway_send_events(text: str) -> list[str]:
    body = markdown_section(text, "Send Events")
    events: list[str] = []
    first_detail = re.search(r"^#{3,4}\s+.+$", body, re.M)
    table_body = body[:first_detail.start()] if first_detail else body
    for value in re.findall(r"\| \[([A-Za-z ]+)\]\(/developers/events/gateway-events#[^)]+\)", table_body):
        append_unique(events, value)
    for match in HEADING_RE.finditer(body):
        level = match.group(1)
        value = match.group(2).strip()
        if level in {"###", "####"} and value in GATEWAY_EVENT_COVERAGE:
            append_unique(events, value)
    if not events:
        raise RuntimeError("could not locate Gateway Send Events table or headings")
    return events


def parse_webhook_event_values(text: str) -> list[str]:
    body = markdown_section(text, "Event Types")
    values = re.findall(
        r"\|\s*\[[^\]]+\]\(/developers/events/webhook-events#[^)]+\)\s*\|\s*`([A-Z0-9_]+)`",
        body
    )
    if not values:
        raise RuntimeError("could not locate Webhook Events table")
    return values


def parse_gateway_opcodes(text: str) -> dict[str, int]:
    body = markdown_section(text, "Gateway")
    opcodes: dict[str, int] = {}
    for match in re.finditer(r"\|\s*(\d+)\s*\|\s*([^|]+?)\s*\|\s*([^|]+?)\s*\|", body):
        action = match.group(3).strip()
        if "Send" not in action:
            continue
        name = match.group(2).strip()
        opcodes[name] = int(match.group(1))
    if not opcodes:
        raise RuntimeError("could not locate Gateway opcode table")
    return opcodes


def source_has_gateway_opcode(source_text: str, opcode: int) -> bool:
    return re.search(rf'\\?"op\\?"\s*:\s*{opcode}\b', source_text) is not None


def main() -> int:
    if not GATEWAY_DOC.exists() or not OPCODES_DOC.exists() or not WEBHOOK_DOC.exists():
        print(
            "official events audit failed: set DCC_DISCORD_API_DOCS_ROOT to a discord-api-docs checkout",
            file=sys.stderr,
        )
        return 1

    public_text = read_all(PUBLIC_HEADER_FILES)
    source_text = read_all(SOURCE_FILES)
    smoke_text = read_all(SMOKE_FILES)
    doc_text = read(DOC_FILE)
    gateway_events = parse_gateway_send_events(read(GATEWAY_DOC))
    gateway_opcodes = parse_gateway_opcodes(read(OPCODES_DOC))
    webhook_values = parse_webhook_event_values(read(WEBHOOK_DOC))

    missing: list[str] = []

    for event in gateway_events:
        symbols = GATEWAY_EVENT_COVERAGE.get(event)
        if symbols is None:
            missing.append(f"gateway send event unclassified: {event}")
            continue
        for symbol in symbols:
            if symbol.startswith("dcc_client_") and symbol not in public_text:
                missing.append(f"gateway public symbol missing: {event}: {symbol}")
            if symbol not in source_text:
                missing.append(f"gateway source coverage missing: {event}: {symbol}")
        opcode_name = GATEWAY_EVENT_OPCODE_NAMES.get(event)
        if opcode_name is None:
            missing.append(f"gateway opcode event unclassified: {event}")
        elif opcode_name not in gateway_opcodes:
            missing.append(f"gateway opcode missing from official docs: {event}: {opcode_name}")
        elif not source_has_gateway_opcode(source_text, gateway_opcodes[opcode_name]):
            missing.append(
                f"gateway opcode source literal missing: {event}: op {gateway_opcodes[opcode_name]}"
            )
        if event not in doc_text:
            missing.append(f"gateway docs mention missing: {event}")

    for value in webhook_values:
        enum_name = f"DCC_WEBHOOK_EVENT_{value}"
        if enum_name not in public_text:
            missing.append(f"webhook enum missing: {enum_name}")
        if value not in source_text:
            missing.append(f"webhook string map missing: {value}")
        if value not in doc_text:
            missing.append(f"webhook docs mention missing: {value}")

    for required in [
        "timestamp",
        "timestamp_len",
        "event_json",
        "event_json_len",
        "data_json",
        "data_json_len",
        "dcc_webhook_event_parse",
    ]:
        if required not in public_text + source_text:
            missing.append(f"webhook parser field/symbol missing: {required}")

    for required in [
        "top-level fields",
        "Top-level `type`, `application_id`, and `event`",
    ]:
        if required not in doc_text:
            missing.append(f"webhook parser contract docs missing: {required}")

    for required in [
        'dcc_webhook_event_type_from_string("PING")',
        "DCC_WEBHOOK_EVENT_PING",
    ]:
        if required not in smoke_text:
            missing.append(f"webhook PING round-trip smoke missing: {required}")

    if missing:
        print("official events audit failed:", file=sys.stderr)
        for item in missing:
            print(f"  - {item}", file=sys.stderr)
        return 1

    print(
        "official events audit passed: "
        f"{len(gateway_events)} gateway send events, {len(webhook_values)} webhook events"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
