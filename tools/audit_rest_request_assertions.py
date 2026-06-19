#!/usr/bin/env python3
"""Audit that DPP-mapped REST wrappers have request-level smoke assertions."""

from __future__ import annotations

import importlib.util
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
HTTP_SMOKE = ROOT / "tests/http_smoke.c"
HTTP_SMOKE_SUPPORT_GLOB = "http_smoke*.c"
SURFACE_AUDIT = ROOT / "tools/audit_rest_surface.py"

# These wrappers are covered by bespoke smoke blocks rather than the generic
# run_public_rest_wrapper_expect()/EXPECT_* request assertion helpers.
MANUAL_REQUEST_ASSERTED = {
    "dcc_rest_bulk_overwrite_global_commands_builder",
    "dcc_rest_bulk_overwrite_guild_commands_builder",
    "dcc_rest_create_global_command_builder",
    "dcc_rest_create_guild_command_builder",
    "dcc_rest_create_guild_sticker_multipart",
    "dcc_rest_create_guild_sticker_params",
    "dcc_rest_create_message",
    "dcc_rest_create_message_builder",
    "dcc_rest_create_message_builder_multipart",
    "dcc_rest_edit_global_command_builder",
    "dcc_rest_edit_guild_command_builder",
    "dcc_rest_edit_guild_command_permissions",
    "dcc_rest_edit_message_builder",
    "dcc_rest_edit_message_builder_multipart",
    "dcc_rest_execute_webhook_multipart",
    "dcc_rest_execute_webhook_multipart_options",
    "dcc_rest_get_channel",
    "dcc_rest_get_message",
    "dcc_rest_guild_message_search",
    "dcc_rest_guild_message_search_params",
    "dcc_rest_interaction_response_create",
    "dcc_rest_interaction_response_create_message_builder",
    "dcc_rest_modify_webhook_message_multipart",
    "dcc_rest_modify_webhook_message_multipart_thread",
}


def load_surface_mapping() -> set[str]:
    spec = importlib.util.spec_from_file_location("dcc_rest_surface_audit", SURFACE_AUDIT)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"failed to load {SURFACE_AUDIT}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return {wrapper for wrappers in module.DPP_TO_DCC.values() for wrapper in wrappers}


def read_http_smoke() -> str:
    parts = [HTTP_SMOKE.read_text()]
    support_dir = ROOT / "tests" / "support"
    if support_dir.is_dir():
        parts.extend(
            path.read_text()
            for path in sorted(support_dir.glob(HTTP_SMOKE_SUPPORT_GLOB))
        )
    return "\n".join(parts)


def find_function_body(text: str, name: str) -> str:
    signature = re.search(
        rf"\b(?:static\s+)?dcc_status_t\s+{re.escape(name)}\s*\([^)]*\)\s*\{{",
        text,
        re.S,
    )
    if signature is None:
        return ""

    cursor = signature.end()
    depth = 1
    while cursor < len(text) and depth != 0:
        if text[cursor] == "{":
            depth += 1
        elif text[cursor] == "}":
            depth -= 1
        cursor += 1
    if depth != 0:
        return ""
    return text[signature.end() : cursor - 1]


def split_c_arguments(arg_text: str) -> list[str]:
    args: list[str] = []
    start = 0
    paren = 0
    brace = 0
    bracket = 0
    in_string = False
    escaped = False
    for index, char in enumerate(arg_text):
        if in_string:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                in_string = False
            continue
        if char == '"':
            in_string = True
        elif char == "(":
            paren += 1
        elif char == ")":
            paren -= 1
        elif char == "{":
            brace += 1
        elif char == "}":
            brace -= 1
        elif char == "[":
            bracket += 1
        elif char == "]":
            bracket -= 1
        elif char == "," and paren == 0 and brace == 0 and bracket == 0:
            args.append(arg_text[start:index].strip())
            start = index + 1
    tail = arg_text[start:].strip()
    if tail:
        args.append(tail)
    return args


def macro_calls(text: str, macro: str) -> list[str]:
    calls: list[str] = []
    pattern = re.compile(rf"\b{re.escape(macro)}\s*\(")
    for match in pattern.finditer(text):
        cursor = match.end()
        depth = 1
        in_string = False
        escaped = False
        while cursor < len(text) and depth != 0:
            char = text[cursor]
            if in_string:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == '"':
                    in_string = False
            elif char == '"':
                in_string = True
            elif char == "(":
                depth += 1
            elif char == ")":
                depth -= 1
            cursor += 1
        if depth == 0:
            calls.append(text[match.end() : cursor - 1])
    return calls


def request_asserted_wrappers(text: str) -> set[str]:
    asserted: set[str] = set()

    for call in macro_calls(text, "run_public_rest_wrapper_expect"):
        args = split_c_arguments(call)
        if len(args) >= 3 and re.fullmatch(r"call_rest_[A-Za-z0-9_]+", args[2]):
            body = find_function_body(text, args[2])
            asserted.update(re.findall(r"\b(dcc_rest_[A-Za-z0-9_]+)\s*\(", body))

    for macro in (
        "EXPECT_REST_GAP",
        "EXPECT_INTERACTION_CONVENIENCE",
        "EXPECT_INTERACTION_MULTIPART",
    ):
        for call in macro_calls(text, macro):
            asserted.update(re.findall(r"\b(dcc_rest_[A-Za-z0-9_]+)\s*\(", call))

    return asserted


def main() -> int:
    expected = load_surface_mapping()
    text = read_http_smoke()
    asserted = request_asserted_wrappers(text)
    all_smoke_calls = set(re.findall(r"\b(dcc_rest_[A-Za-z0-9_]+)\s*\(", text))

    errors: list[str] = []
    unknown_manual = sorted(MANUAL_REQUEST_ASSERTED - expected)
    if unknown_manual:
        errors.append(
            "manual request assertion classification contains non-DPP wrapper(s): "
            + ", ".join(unknown_manual)
        )

    missing_manual_calls = sorted(MANUAL_REQUEST_ASSERTED - all_smoke_calls)
    if missing_manual_calls:
        errors.append(
            "manual request assertion wrapper(s) no longer called in HTTP smoke corpus: "
            + ", ".join(missing_manual_calls)
        )

    missing = sorted(expected - asserted - MANUAL_REQUEST_ASSERTED)
    if missing:
        errors.append(
            "DPP-mapped REST wrapper(s) lack request-level smoke assertions: "
            + ", ".join(missing)
        )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(
        "rest request assertion audit passed: "
        f"{len(expected)} DPP-mapped wrappers; "
        f"{len(asserted)} parsed request assertions; "
        f"{len(MANUAL_REQUEST_ASSERTED)} bespoke request assertions"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
