#!/usr/bin/env python3
"""Audit DCC event surface against the DPP 10.1.5 event router snapshot."""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SNAPSHOT = ROOT / "tools/dpp_event_map_10_1_5.txt"
DPP_SOURCE_CANDIDATES = [
    Path(os.environ["DCC_DPP_ROOT"]) / "src/dpp/discordevents.cpp"
    for _ in [0]
    if os.environ.get("DCC_DPP_ROOT")
] + [
    ROOT.parent / "DPP/src/dpp/discordevents.cpp",
]

EVENTS_H = ROOT / "include/dcc/events.h"
EVENTS_HEADER_DIR = ROOT / "include/dcc/events"
EVENT_TYPES_C = ROOT / "src/events/event_types.c"
GATEWAY_EVENT_NAME_FILES = [
    ROOT / "src/gateway/gateway_event_names.c",
    ROOT / "src/gateway/gateway_events.c",
]
EVENT_SURFACE_SMOKE = ROOT / "tests/event_surface_smoke.c"

SYNTHETIC_DCC_EVENTS = {
    "__RAW__",
    "SOCKET_CLOSE",
    "RATE_LIMITED",
    "CHANNEL_INFO",
    "SOUNDBOARD_SOUNDS",
    "SLASH_COMMAND",
    "BUTTON_CLICK",
    "AUTOCOMPLETE",
    "SELECT_CLICK",
    "MESSAGE_CONTEXT_MENU",
    "USER_CONTEXT_MENU",
    "FORM_SUBMIT",
    "VOICE_BUFFER_SEND",
    "VOICE_READY",
    "VOICE_RECEIVE",
    "VOICE_RECEIVE_COMBINED",
    "VOICE_TRACK_MARKER",
    "VOICE_CLIENT_SPEAKING",
    "VOICE_CLIENT_DISCONNECT",
    "VOICE_CLIENT_PLATFORM",
}


def read_snapshot() -> list[str]:
    names = [
        line.strip()
        for line in SNAPSHOT.read_text().splitlines()
        if line.strip() and not line.startswith("#")
    ]
    if len(names) != len(set(names)):
        raise ValueError("duplicate names in DPP event snapshot")
    return names


def parse_dpp_source(path: Path) -> list[str]:
    text = path.read_text()
    match = re.search(
        r"static const std::map<std::string, dpp::events::event\*> event_map = \{(.*?)\n\s*\};",
        text,
        re.S,
    )
    if match is None:
        raise ValueError(f"cannot find DPP event_map in {path}")
    return re.findall(r'\{\s*"([A-Z0-9_]+)"\s*,', match.group(1))


def read_event_public_headers() -> str:
    parts = [EVENTS_H.read_text()]
    if EVENTS_HEADER_DIR.is_dir():
        parts.extend(path.read_text() for path in sorted(EVENTS_HEADER_DIR.rglob("*.h")))
    return "\n".join(parts)


def parse_enum_entries() -> list[str]:
    text = read_event_public_headers()
    match = re.search(r"typedef enum dcc_event_type \{(.*?)\}\s*dcc_event_type_t;", text, re.S)
    if match is None:
        raise ValueError("cannot find dcc_event_type enum")
    names: list[str] = []
    for raw_line in match.group(1).splitlines():
        line = raw_line.split("//", 1)[0].strip().rstrip(",")
        if line.startswith("DCC_EVENT_"):
            names.append(line.split("=", 1)[0].strip())
    return names


def parse_public_names() -> dict[str, str]:
    text = EVENT_TYPES_C.read_text()
    match = re.search(
        r"static const char \*const dcc_event_type_names\[DCC_EVENT_MAX\]\s*=\s*\{(.*?)\};",
        text,
        re.S,
    )
    if match is None:
        raise ValueError("cannot find dcc_event_type_names")
    return dict(re.findall(r'\[(DCC_EVENT_[A-Z0-9_]+)\]\s*=\s*"([^"]+)"', match.group(1)))


def parse_gateway_names() -> dict[str, str]:
    text = "\n".join(path.read_text() for path in GATEWAY_EVENT_NAME_FILES if path.exists())
    return dict(re.findall(r'DCC_GATEWAY_EVENT_NAME\("([A-Z0-9_]+)",\s*(DCC_EVENT_[A-Z0-9_]+)\)', text))


def parse_smoke_router_names() -> set[str]:
    text = EVENT_SURFACE_SMOKE.read_text()
    match = re.search(r"static const char \*const dpp_router_event_names\[\]\s*=\s*\{(.*?)\};", text, re.S)
    if match is None:
        raise ValueError("cannot find dpp_router_event_names")
    return set(re.findall(r'"([A-Z0-9_]+|__LOG__)"', match.group(1)))


def main() -> int:
    errors: list[str] = []

    dpp_snapshot = read_snapshot()
    dpp_set = set(dpp_snapshot)
    public_names = parse_public_names()
    public_name_set = set(public_names.values())
    public_enum_set = set(public_names)
    enum_entries = parse_enum_entries()
    gateway_names = parse_gateway_names()
    smoke_names = parse_smoke_router_names()

    if enum_entries[-1] != "DCC_EVENT_MAX":
        errors.append("DCC_EVENT_MAX is not the final event enum entry")

    missing_public = sorted(dpp_set - public_name_set)
    if missing_public:
        errors.append(f"DPP event(s) missing from dcc_event_type_names: {', '.join(missing_public)}")

    missing_gateway = sorted(dpp_set - {"__LOG__"} - set(gateway_names))
    if missing_gateway:
        errors.append(f"DPP dispatch event(s) missing from gateway name table: {', '.join(missing_gateway)}")

    missing_smoke = sorted(dpp_set - smoke_names)
    if missing_smoke:
        errors.append(f"DPP event(s) missing from event_surface_smoke router list: {', '.join(missing_smoke)}")

    unnamed = sorted(enum for enum in enum_entries if enum != "DCC_EVENT_MAX" and enum not in public_enum_set)
    if unnamed:
        errors.append(f"DCC enum entry/entries without public name: {', '.join(unnamed)}")

    invalid_gateway_targets = sorted(set(gateway_names.values()) - set(enum_entries))
    if invalid_gateway_targets:
        errors.append(f"gateway table maps to unknown enum(s): {', '.join(invalid_gateway_targets)}")

    unexpected_public = sorted(public_name_set - dpp_set - SYNTHETIC_DCC_EVENTS - {"SUBSCRIPTION_CREATE", "SUBSCRIPTION_UPDATE", "SUBSCRIPTION_DELETE"})
    if unexpected_public:
        errors.append(f"unexpected DCC-only public event(s): {', '.join(unexpected_public)}")

    for candidate in DPP_SOURCE_CANDIDATES:
        if candidate.exists():
            live_names = parse_dpp_source(candidate)
            if live_names != dpp_snapshot:
                errors.append(
                    "DPP event snapshot is stale relative to "
                    f"{candidate}; update tools/dpp_event_map_10_1_5.txt deliberately"
                )
            break

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(
        "event surface audit passed: "
        f"{len(dpp_snapshot)} DPP router keys, {len(public_names)} DCC public events, "
        f"{len(gateway_names)} gateway dispatch names"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
