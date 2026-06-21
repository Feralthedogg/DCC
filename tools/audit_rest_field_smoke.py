#!/usr/bin/env python3
"""Audit high-risk REST params smoke fixtures at field level."""

from __future__ import annotations

from collections import Counter
import json
import re
import sys
from pathlib import Path
from urllib.parse import quote


ROOT = Path(__file__).resolve().parents[1]
HEADER = ROOT / "include/dcc/rest.h"
HEADER_DIR = ROOT / "include/dcc/rest"
HTTP_SMOKE = ROOT / "tests/http_smoke.c"
HTTP_SMOKE_SUPPORT_GLOB = "http_smoke*.c"


def read_rest_public_headers() -> str:
    parts = [HEADER.read_text(encoding="utf-8")]
    if HEADER_DIR.is_dir():
        parts.extend(path.read_text(encoding="utf-8") for path in sorted(HEADER_DIR.rglob("*.h")))
    return "\n".join(parts)


def read_http_smoke() -> str:
    parts = [HTTP_SMOKE.read_text(encoding="utf-8")]
    support_dir = ROOT / "tests" / "support"
    if support_dir.is_dir():
        parts.extend(
            path.read_text(encoding="utf-8")
            for path in sorted(support_dir.glob(HTTP_SMOKE_SUPPORT_GLOB))
        )
    return "\n".join(parts)


FIELD_WIRE_ALIASES = {
    "attachment_extensions": {"attachment_extension"},
    "attachment_filenames": {"attachment_filename"},
    "author_ids": {"author_id"},
    "author_types": {"author_type"},
    "bitrate_kbps": {"bitrate"},
    "default_reaction_emoji_id": {"default_reaction_emoji"},
    "default_reaction_emoji_name": {"default_reaction_emoji"},
    "description_localizations_json": {"description_localizations"},
    "embed_providers": {"embed_provider"},
    "embed_types": {"embed_type"},
    "flags": {"flags", "single_select", "required", "in_onboarding"},
    "hide_media_download_options": {"flags"},
    "keywords": {"keyword_filter"},
    "link_hostnames": {"link_hostname"},
    "mention_role_ids": {"mentions_role_id"},
    "message_json": {"message"},
    "metadata_fields": {"metadata"},
    "name_localizations_json": {"name_localizations"},
    "positions": {"id", "position", "parent_id", "lock_permissions"},
    "replied_to_message_ids": {"replied_to_message_id"},
    "replied_to_user_ids": {"replied_to_user_id"},
    "require_tag": {"flags"},
    "target_user_id": {"target_user"},
    "trigger_metadata": {"trigger_metadata"},
}

WIRE_TOKEN_SKIP_FIELDS = {
    "application_id",
    "applied_tag_count",
    "attachment_extension_count",
    "attachment_filename_count",
    "author_id_count",
    "author_type_count",
    "available_tag_count",
    "channel_id",
    "channel_id_count",
    "code",
    "command_count",
    "command_id",
    "content_type",
    "creator_id",
    "data",
    "data_len",
    "default_channel_id_count",
    "embed_provider_count",
    "embed_type_count",
    "emoji_id",
    "entity_id",
    "event_id",
    "exempt_channel_count",
    "exempt_role_count",
    "filename",
    "guild_id",
    "has_count",
    "include_role_count",
    "keyword_count",
    "link_hostname_count",
    "mention_count",
    "mention_role_id_count",
    "message_id",
    "metadata_field_count",
    "option_count",
    "owner_id",
    "owner_type",
    "permission_count",
    "permission_overwrite_count",
    "position_count",
    "preset_count",
    "prompt_count",
    "regex_pattern_count",
    "replied_to_message_id_count",
    "replied_to_user_id_count",
    "role_count",
    "role_id",
    "rule_id",
    "sku_id",
    "thread_id",
    "token",
    "user_id",
    "webhook_id",
    "welcome_channel_count",
}

WIRE_TOKEN_SKIP_BY_HELPER = {
    "call_rest_add_guild_member_params": {"flags"},
    "call_rest_modify_guild_member_params": {"flags"},
    "call_rest_modify_thread_params": {"type"},
}

WIRE_VALUE_FIELDS = {
    "application_id",
    "channel_id",
    "code",
    "command_id",
    "creator_id",
    "emoji_id",
    "entity_id",
    "event_id",
    "guild_id",
    "message_id",
    "owner_id",
    "role_id",
    "rule_id",
    "sku_id",
    "thread_id",
    "token",
    "user_id",
    "webhook_id",
}

EXPECTED_MULTIPART_REQUESTS = Counter(
    {
        (
            "POST",
            "/multipart-overload",
            "NULL",
            'name="payload_json"\r\n\r\n{"content":"overload"}',
            'name="files[0]"; filename="overload.txt"\r\nContent-Type: text/plain\r\n\r\noverload-file',
        ): 1,
        (
            "PATCH",
            "/multipart-method",
            "NULL",
            'name="payload_json"\r\n\r\n{"content":"overload"}',
            'name="files[0]"; filename="overload.txt"',
        ): 1,
        (
            "POST",
            "/channels/222/messages",
            '{"content":"hi"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"\r\nContent-Type: text/plain\r\n\r\nhello-file',
        ): 1,
        (
            "PATCH",
            "/channels/222/messages/333",
            '{"content":"edited"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "POST",
            "/webhooks/666/tok%2Fen?wait=true",
            '{"content":"hook"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "POST",
            "/webhooks/666/tok%2Fen?wait=true&thread_id=888&with_components=true",
            '{"content":"hook"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "PATCH",
            "/webhooks/666/tok%2Fen/messages/777?thread_id=888",
            '{"content":"hook-edited"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 2,
        (
            "POST",
            "/webhooks/123/tok%2Fen",
            '{"content":"follow"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/@original",
            '{"content":"original"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/999",
            '{"content":"follow-edited"}',
            'name="payload_json"',
            'name="files[0]"; filename="hello.txt"',
        ): 1,
        (
            "POST",
            "/guilds/333/stickers",
            "NULL",
            'name="tags"\r\n\r\nslap',
            'name="file"; filename="slap.png"\r\nContent-Type: image/png\r\n\r\npng-data',
        ): 2,
        (
            "POST",
            "/channels/222/messages",
            "expected_body",
            'name="payload_json"',
            'name="files[0]"; filename="builder.txt"\r\nContent-Type: text/plain\r\n\r\nbuilder-file',
        ): 1,
        (
            "PATCH",
            "/channels/222/messages/333",
            "expected_body",
            'name="payload_json"',
            'name="files[0]"; filename="builder.txt"\r\nContent-Type: text/plain\r\n\r\nbuilder-file',
        ): 1,
    }
)

EXPECTED_INTERACTION_MULTIPART_REQUESTS = Counter(
    {
        (
            "interaction convenience original multipart edit",
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/@original",
            '{"content":"original file"}',
        ): 1,
        (
            "interaction convenience followup create multipart",
            "POST",
            "/webhooks/123/tok%2Fen",
            '{"content":"new followup file"}',
        ): 1,
        (
            "interaction convenience followup edit multipart",
            "PATCH",
            "/webhooks/123/tok%2Fen/messages/999",
            '{"content":"followup file edited"}',
        ): 1,
    }
)

EXPECTED_REST_GAP_LABELS = {
    "bulk_edit_guild_command_permissions_params",
    "bulk_overwrite_application_commands_default_global",
    "bulk_overwrite_application_commands_specific_guild",
    "bulk_overwrite_global_commands",
    "bulk_overwrite_guild_commands",
    "create_application_command_default_global",
    "create_application_command_specific_guild",
    "create_global_command",
    "create_guild_command",
    "delete_channel",
    "delete_all_application_commands_default_global",
    "delete_all_application_commands_specific_guild",
    "delete_global_command",
    "delete_guild_command",
    "delete_message",
    "edit_global_command",
    "edit_guild_command",
    "edit_guild_command_permissions_params",
    "edit_message",
    "get_all_guild_command_permissions",
    "get_channel_messages",
    "get_channel_messages_page",
    "get_global_command",
    "get_global_commands",
    "get_global_commands_with_localizations",
    "get_guild",
    "get_guild_command",
    "get_guild_command_permissions",
    "get_guild_commands",
    "get_guild_commands_with_localizations",
    "interaction_followup_create",
    "interaction_followup_create_builder",
    "interaction_followup_delete",
    "interaction_followup_edit",
    "interaction_followup_edit_builder",
    "interaction_followup_get",
    "interaction_original_response_delete",
    "interaction_original_response_edit",
    "interaction_original_response_edit_builder",
    "interaction_original_response_get",
    "modify_channel",
    "modify_guild",
}

EXPECTED_INTERACTION_CONVENIENCE_LABELS = {
    "direct interaction ephemeral text",
    "direct interaction error embed",
    "interaction convenience autocomplete callback",
    "interaction convenience deferred message callback",
    "interaction convenience followup create",
    "interaction convenience followup edit",
    "interaction convenience followup get",
    "interaction convenience modal callback",
    "interaction convenience original delete",
    "interaction convenience original get",
    "interaction convenience type callback",
}

DIRECT_REQUEST_REQUIRED_TOKENS = {
    "/channels/222/messages?after=666&around=888&before=777&limit=100",
    "/applications/123/guilds/333/commands/permissions",
    "/applications/123/guilds/333/commands?with_localizations=true",
    "/webhooks/123/tok/messages/@original",
    '"permissions":[{"id":"555","type":1,"permission":true}]',
    '"content":"builder"',
}

INTERACTION_CONVENIENCE_REQUIRED_TOKENS = {
    "/interactions/558/tok%2Fen/callback",
    "/webhooks/123/tok%2Fen/messages/@original",
    "/webhooks/123/tok%2Fen/messages/999",
    '"type":9',
    '"type":8',
    '"type":6',
    '"type":5',
    '"flags":64',
    '"color":15548997',
    '"name_localizations":{"ko":"red-ko"}',
}

MESSAGE_BUILDER_BODY_TOKENS = {
    '"content":"hello \\"json\\"\\nnext"',
    '"tts":true',
    '"flags":64',
    '"nonce":"nonce-1"',
    '"sticker_ids":["777","778"]',
    '"message_reference":{"message_id":"111"}',
    '"allowed_mentions":{"parse":[]}',
    '"embeds":[{',
    '"title":"embed \\"title\\""',
    '"description":"line one\\nline two"',
    '"footer":{"text":"footer","icon_url":"https://example.com/footer.png"}',
    '"image":{"url":"https://example.com/image.png"}',
    '"thumbnail":{"url":"https://example.com/thumb.png"}',
    '"author":{"name":"author","url":"https://example.com/author","icon_url":"https://example.com/author.png"}',
    '"fields":[{"name":"field one","value":"value \\"one\\"","inline":true}',
    '{"type":2,"style":1,"label":"Launch","emoji":{"id":"888","name":"wave"},"custom_id":"launch","disabled":false}',
    '{"type":2,"style":5,"label":"Docs","url":"https://example.com/docs"}',
    '{"type":3,"custom_id":"select-color","options":[',
    '{"label":"Red","value":"red","description":"Pick red","default":true}',
    '"placeholder":"Pick","min_values":1,"max_values":1',
    '"attachments":[{"id":"0","filename":"hello.txt"}]',
    '"poll":{"question":{"text":"Pick \\"one\\""}',
    '{"poll_media":{"text":"Alpha \\"A\\"","emoji":{"name":"one"}}}',
    '{"poll_media":{"text":"Beta","emoji":{"id":"889","name":"two","animated":true}}}',
    '"duration":48',
    '"allow_multiselect":true',
    '"layout_type":1',
}

COMMAND_BUILDER_BODY_TOKENS = {
    '"name":"ping"',
    '"name_localizations":{"ko":"ping-ko"}',
    '"description":"say \\"pong\\"\\nnow"',
    '"description_localizations":{"ko":"pong-ko"}',
    '"type":1',
    '"options":[{"type":3,"name":"text","description":"input","required":true}]',
    '"default_member_permissions":"2048"',
    '"dm_permission":false',
    '"nsfw":true',
    '"integration_types":[0]',
    '"contexts":[0,1]',
}

COMMAND_BUILDER_ARRAY_TOKENS = {
    'snprintf(expected_array, sizeof(expected_array)',
    '{"name":"inspect","type":2,"default_member_permissions":null}',
    "dcc_rest_bulk_overwrite_global_commands_builder",
    "dcc_rest_bulk_overwrite_guild_commands_builder",
    "dcc_rest_bulk_overwrite_application_commands_builder",
    "expected_array",
}

INTERACTION_BUILDER_BODY_TOKENS = {
    '"type":4,"data":{"content":"pong builder"}',
    '"type":9,"data":{"custom_id":"feedback-modal","title":"Feedback"',
    '"type":4,"custom_id":"feedback","style":2,"label":"Feedback"',
    '"min_length":3,"max_length":1000',
    '"required":true,"value":"seed","placeholder":"Tell us"',
    '"type":8,"data":{"choices":[',
    '{"name":"Red","name_localizations":{"ko":"red-ko"},"value":"red"}',
    '{"name":"Limit","value":42}',
    '{"name":"Ratio","value":3.5}',
}


def find_braced(text: str, open_brace: int) -> str:
    depth = 0
    cursor = open_brace
    in_string = False
    in_char = False
    escaped = False
    while cursor < len(text):
        char = text[cursor]
        if escaped:
            escaped = False
        elif char == "\\" and (in_string or in_char):
            escaped = True
        elif in_string:
            if char == '"':
                in_string = False
        elif in_char:
            if char == "'":
                in_char = False
        elif char == '"':
            in_string = True
        elif char == "'":
            in_char = True
        elif char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace + 1 : cursor]
        cursor += 1
    raise ValueError("unterminated braced block")


def find_function_body(text: str, name: str) -> str:
    match = re.search(
        rf"\b(?:static\s+)?(?:int|dcc_status_t)\s+{re.escape(name)}\s*\([^)]*\)\s*\{{",
        text,
        re.S,
    )
    if match is None:
        raise ValueError(f"cannot find function {name}")
    return find_braced(text, match.end() - 1)


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


def c_string_literal_value(expr: str) -> str:
    if expr.strip() == "NULL":
        return ""
    parts = re.findall(r'"((?:\\.|[^"\\])*)"', expr, re.S)
    return "".join(bytes(part, "utf-8").decode("unicode_escape") for part in parts)


def c_expected_value(expr: str) -> str:
    normalized = re.sub(r"\s+", " ", expr.replace("\\\n", " ")).strip()
    if normalized == "NULL":
        return "NULL"
    if '"' in normalized:
        return c_string_literal_value(normalized)
    return normalized.strip("() ")


def find_initializer(text: str, typedef_name: str, variable: str) -> str:
    match = re.search(
        rf"\b(?:const\s+)?{re.escape(typedef_name)}\s+{re.escape(variable)}(?:\s*\[[^]]*\])?\s*=\s*\{{",
        text,
        re.S,
    )
    if match is None:
        raise ValueError(f"cannot find initializer {typedef_name} {variable}")
    return find_braced(text, match.end() - 1)


def parse_struct_fields(tag: str, body: str) -> list[str]:
    fields: list[str] = []
    for declaration in body.split(";"):
        declaration = declaration.strip()
        if not declaration:
            continue
        field = re.search(r"([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^]]*\])?$", declaration)
        if field is None:
            raise ValueError(f"cannot parse field declaration in {tag}: {declaration}")
        fields.append(field.group(1))
    return fields


def rest_param_structs(header: str) -> dict[str, list[str]]:
    structs: dict[str, list[str]] = {}
    for match in re.finditer(
        r"typedef\s+struct\s+(dcc_[A-Za-z0-9_]*params)\s*\{(?P<body>.*?)\}\s*(dcc_[A-Za-z0-9_]*params_t)\s*;",
        header,
        re.S,
    ):
        structs[match.group(3)] = parse_struct_fields(match.group(1), match.group("body"))
    return structs


def public_multipart_functions(header: str) -> list[str]:
    return re.findall(
        r"\bDCC_API\s+dcc_status_t\s+(dcc_rest_[A-Za-z0-9_]*multipart[A-Za-z0-9_]*)\s*\(",
        header,
    )


def struct_fields(header: str, tag: str) -> list[str]:
    typedef_name = f"{tag}_t"
    structs = rest_param_structs(header)
    if typedef_name not in structs:
        raise ValueError(f"cannot find struct {tag}")
    return structs[typedef_name]


def find_initializers(text: str, typedef_name: str) -> list[str]:
    initializers: list[str] = []
    pattern = re.compile(
        rf"\b(?:const\s+)?{re.escape(typedef_name)}\s+[A-Za-z_][A-Za-z0-9_]*(?:\s*\[[^]]*\])?\s*=\s*\{{",
        re.S,
    )
    for match in pattern.finditer(text):
        initializers.append(find_braced(text, match.end() - 1))
    return initializers


def assigned_fields(source: str) -> set[str]:
    return set(re.findall(r"\.([A-Za-z_][A-Za-z0-9_]*)\s*=", source))


def read_initializer_expr(source: str, start: int) -> tuple[str, int]:
    cursor = start
    paren = 0
    brace = 0
    bracket = 0
    in_string = False
    in_char = False
    escaped = False
    while cursor < len(source):
        char = source[cursor]
        if escaped:
            escaped = False
        elif char == "\\" and (in_string or in_char):
            escaped = True
        elif in_string:
            if char == '"':
                in_string = False
        elif in_char:
            if char == "'":
                in_char = False
        elif char == '"':
            in_string = True
        elif char == "'":
            in_char = True
        elif char == "(":
            paren += 1
        elif char == ")":
            paren -= 1
        elif char == "{":
            brace += 1
        elif char == "}":
            if paren == 0 and brace == 0 and bracket == 0:
                break
            brace -= 1
        elif char == "[":
            bracket += 1
        elif char == "]":
            bracket -= 1
        elif char == "," and paren == 0 and brace == 0 and bracket == 0:
            break
        cursor += 1
    return source[start:cursor].strip(), cursor


def find_char_string_assignment(text: str, variable: str) -> str:
    match = re.search(
        rf"\b(?:const\s+)?char\s+{re.escape(variable)}\s*(?:\[[^]]*\])?\s*=\s*",
        text,
        re.S,
    )
    if match is None:
        raise ValueError(f"cannot find char string assignment {variable}")

    cursor = match.end()
    in_string = False
    in_char = False
    escaped = False
    while cursor < len(text):
        char = text[cursor]
        if escaped:
            escaped = False
        elif char == "\\" and (in_string or in_char):
            escaped = True
        elif in_string:
            if char == '"':
                in_string = False
        elif in_char:
            if char == "'":
                in_char = False
        elif char == '"':
            in_string = True
        elif char == "'":
            in_char = True
        elif char == ";":
            return text[match.end() : cursor].strip()
        cursor += 1
    raise ValueError(f"unterminated char string assignment {variable}")


def assigned_scalar_values(source: str) -> dict[str, set[str]]:
    values: dict[str, set[str]] = {}
    pattern = re.compile(r"\.([A-Za-z_][A-Za-z0-9_]*)\s*=")
    cursor = 0
    while True:
        match = pattern.search(source, cursor)
        if match is None:
            break
        expr, cursor = read_initializer_expr(source, match.end())
        field = match.group(1)
        value = scalar_initializer_value(expr)
        if value is not None:
            values.setdefault(field, set()).add(value)
    return values


def scalar_initializer_value(expr: str) -> str | None:
    expr = expr.strip()
    if not expr:
        return None
    if expr == "NULL":
        return "null"
    if expr.startswith('"'):
        value = c_string_literal_value(expr)
        return value if value != "" else None
    if re.fullmatch(r"[0-9]+(?:[uUlL]*)", expr):
        return re.sub(r"[uUlL]+$", "", expr)
    return None


def require_fields(
    errors: list[str],
    header: str,
    label: str,
    struct_tag: str,
    source: str,
    ignored: set[str] | None = None,
) -> None:
    ignored = {"size"} | (ignored or set())
    expected = set(struct_fields(header, struct_tag)) - ignored
    actual = assigned_fields(source)
    missing = sorted(expected - actual)
    if missing:
        errors.append(f"{label} smoke initializer misses field(s): {', '.join(missing)}")


def require_literals(errors: list[str], text: str, label: str, literals: list[str]) -> None:
    missing = [literal for literal in literals if literal not in text]
    if missing:
        errors.append(f"{label} smoke expected request misses literal(s): {', '.join(missing)}")


def wrapper_expectations(smoke: str) -> dict[str, str]:
    expectations: dict[str, str] = {}
    for call in macro_calls(smoke, "run_public_rest_wrapper_expect"):
        args = split_c_arguments(call)
        if len(args) < 6 or not re.fullmatch(r"call_rest_[A-Za-z0-9_]+", args[2]):
            continue
        expected_path = c_string_literal_value(args[4])
        expected_body = c_string_literal_value(args[5])
        expectations[args[2]] = f"{expected_path}\n{expected_body}"
    return expectations


def wire_tokens_for_field(field: str) -> set[str]:
    return FIELD_WIRE_ALIASES.get(field, {field})


def wire_token_present(wire: str, token: str) -> bool:
    return f'"{token}"' in wire or f"{token}=" in wire or token in wire


def wire_value_present(wire: str, value: str) -> bool:
    if value == "":
        return False
    return value in wire or quote(value, safe="") in wire


def audit_helper_wire_tokens(
    errors: list[str],
    structs: dict[str, list[str]],
    smoke: str,
) -> tuple[int, int]:
    helpers = wrapper_expectations(smoke)
    audited_helpers = 0
    audited_fields = 0
    for helper, wire in sorted(helpers.items()):
        body = find_function_body(smoke, helper)
        helper_fields = 0
        missing: list[str] = []
        for typedef_name in structs:
            for initializer in find_initializers(body, typedef_name):
                for field in sorted(assigned_fields(initializer)):
                    if (
                        field == "size" or
                        field in WIRE_TOKEN_SKIP_FIELDS or
                        field.startswith("has_") or
                        field.endswith("_count") or
                        field in WIRE_TOKEN_SKIP_BY_HELPER.get(helper, set())
                    ):
                        continue
                    helper_fields += 1
                    tokens = wire_tokens_for_field(field)
                    if not any(wire_token_present(wire, token) for token in tokens):
                        missing.append(f"{typedef_name}.{field}")
        if helper_fields == 0:
            continue
        audited_helpers += 1
        audited_fields += helper_fields
        if missing:
            errors.append(
                f"{helper} expected request misses wire token(s) for field(s): "
                + ", ".join(missing)
            )
    return audited_helpers, audited_fields


def audit_helper_wire_values(
    errors: list[str],
    structs: dict[str, list[str]],
    smoke: str,
) -> tuple[int, int]:
    helpers = wrapper_expectations(smoke)
    audited_helpers = 0
    audited_fields = 0
    for helper, wire in sorted(helpers.items()):
        body = find_function_body(smoke, helper)
        helper_fields = 0
        missing: list[str] = []
        for typedef_name in structs:
            for initializer in find_initializers(body, typedef_name):
                values = assigned_scalar_values(initializer)
                for field in sorted(WIRE_VALUE_FIELDS & values.keys()):
                    for value in sorted(values[field]):
                        if value == "0":
                            continue
                        helper_fields += 1
                        if not wire_value_present(wire, value):
                            missing.append(f"{typedef_name}.{field}={value}")
        if helper_fields == 0:
            continue
        audited_helpers += 1
        audited_fields += helper_fields
        if missing:
            errors.append(
                f"{helper} expected request misses wire value(s) for field(s): "
                + ", ".join(missing)
            )
    return audited_helpers, audited_fields


def audit_multipart_expectations(errors: list[str], header: str, smoke: str) -> tuple[int, int, int]:
    multipart_functions = public_multipart_functions(header)
    missing_functions = sorted(name for name in multipart_functions if name not in smoke)
    if missing_functions:
        errors.append(
            "public multipart REST function(s) missing HTTP smoke coverage: "
            + ", ".join(missing_functions)
        )

    actual_direct: Counter[tuple[str, str, str, str, str]] = Counter()
    for call in macro_calls(smoke, "expect_multipart_request"):
        args = split_c_arguments(call)
        if len(args) != 6:
            continue
        method = c_expected_value(args[1])
        path = c_expected_value(args[2])
        if method not in {"POST", "PATCH"} or not path.startswith("/"):
            continue
        actual_direct[
            (
                method,
                path,
                c_expected_value(args[3]),
                c_expected_value(args[4]),
                c_expected_value(args[5]),
            )
        ] += 1

    if actual_direct != EXPECTED_MULTIPART_REQUESTS:
        missing = EXPECTED_MULTIPART_REQUESTS - actual_direct
        unexpected = actual_direct - EXPECTED_MULTIPART_REQUESTS
        if missing:
            errors.append(
                "HTTP smoke missing multipart request expectation(s): "
                + "; ".join(f"{count}x {entry!r}" for entry, count in sorted(missing.items()))
            )
        if unexpected:
            errors.append(
                "HTTP smoke has unexpected multipart request expectation(s): "
                + "; ".join(f"{count}x {entry!r}" for entry, count in sorted(unexpected.items()))
            )

    for entry, count in actual_direct.items():
        method, path, payload, field_fragment, file_fragment = entry
        if "filename=" not in file_fragment or 'name="' not in file_fragment:
            errors.append(f"multipart request {method} {path} does not pin file field and filename")
        if payload != "NULL" and "payload_json" not in field_fragment:
            errors.append(f"multipart request {method} {path} does not pin payload_json field")

    actual_interaction: Counter[tuple[str, str, str, str]] = Counter()
    for call in macro_calls(smoke, "EXPECT_INTERACTION_MULTIPART"):
        args = split_c_arguments(call)
        if len(args) != 5:
            continue
        method = c_expected_value(args[1])
        path = c_expected_value(args[2])
        if method not in {"POST", "PATCH"} or not path.startswith("/"):
            continue
        actual_interaction[
            (
                c_expected_value(args[0]),
                method,
                path,
                c_expected_value(args[3]),
            )
        ] += 1

    if actual_interaction != EXPECTED_INTERACTION_MULTIPART_REQUESTS:
        missing = EXPECTED_INTERACTION_MULTIPART_REQUESTS - actual_interaction
        unexpected = actual_interaction - EXPECTED_INTERACTION_MULTIPART_REQUESTS
        if missing:
            errors.append(
                "HTTP smoke missing interaction convenience multipart expectation(s): "
                + "; ".join(f"{count}x {entry!r}" for entry, count in sorted(missing.items()))
            )
        if unexpected:
            errors.append(
                "HTTP smoke has unexpected interaction convenience multipart expectation(s): "
                + "; ".join(f"{count}x {entry!r}" for entry, count in sorted(unexpected.items()))
            )

    required_macro_fragment = (
        'name=\\"files[0]\\"; filename=\\"interaction.txt\\"\\r\\n'
        "Content-Type: text/plain\\r\\n\\r\\ninteraction-file"
    )
    if required_macro_fragment not in smoke:
        errors.append("interaction convenience multipart macro no longer pins interaction file body")

    return len(multipart_functions), sum(actual_direct.values()), sum(actual_interaction.values())


def macro_request_expectations(smoke: str, macro: str) -> list[tuple[str, str, str, str]]:
    expectations: list[tuple[str, str, str, str]] = []
    for call in macro_calls(smoke, macro):
        args = split_c_arguments(call)
        if len(args) < 4:
            continue
        label = c_expected_value(args[0])
        method = c_expected_value(args[1])
        path = c_expected_value(args[2])
        body = c_expected_value(args[3])
        if method not in {"DELETE", "GET", "PATCH", "POST", "PUT"} or not path.startswith("/"):
            continue
        expectations.append((label, method, path, body))
    return expectations


def audit_direct_request_expectations(errors: list[str], smoke: str) -> tuple[int, int]:
    gap_expectations = macro_request_expectations(smoke, "EXPECT_REST_GAP")
    gap_labels = {label for label, _, _, _ in gap_expectations}
    if gap_labels != EXPECTED_REST_GAP_LABELS:
        missing = sorted(EXPECTED_REST_GAP_LABELS - gap_labels)
        unexpected = sorted(gap_labels - EXPECTED_REST_GAP_LABELS)
        if missing:
            errors.append(f"EXPECT_REST_GAP missing direct request label(s): {', '.join(missing)}")
        if unexpected:
            errors.append(f"EXPECT_REST_GAP has unexpected direct request label(s): {', '.join(unexpected)}")

    interaction_expectations = macro_request_expectations(smoke, "EXPECT_INTERACTION_CONVENIENCE")
    interaction_labels = {label for label, _, _, _ in interaction_expectations}
    if interaction_labels != EXPECTED_INTERACTION_CONVENIENCE_LABELS:
        missing = sorted(EXPECTED_INTERACTION_CONVENIENCE_LABELS - interaction_labels)
        unexpected = sorted(interaction_labels - EXPECTED_INTERACTION_CONVENIENCE_LABELS)
        if missing:
            errors.append(
                f"EXPECT_INTERACTION_CONVENIENCE missing request label(s): {', '.join(missing)}"
            )
        if unexpected:
            errors.append(
                f"EXPECT_INTERACTION_CONVENIENCE has unexpected request label(s): {', '.join(unexpected)}"
            )

    for label, method, path, body in gap_expectations + interaction_expectations:
        if " " in path:
            errors.append(f"{label} request path contains whitespace: {path!r}")
        if body != "NULL" and not (body.startswith("{") or body.startswith("[")):
            errors.append(f"{label} expected body is not JSON object/array or NULL: {body!r}")
        if method in {"DELETE", "GET"} and body != "NULL":
            errors.append(f"{label} {method} request unexpectedly pins a body: {body!r}")

    gap_wire = "\n".join(f"{label}\n{method}\n{path}\n{body}" for label, method, path, body in gap_expectations)
    missing_gap_tokens = sorted(token for token in DIRECT_REQUEST_REQUIRED_TOKENS if token not in gap_wire)
    if missing_gap_tokens:
        errors.append(
            "EXPECT_REST_GAP direct request corpus misses high-risk token(s): "
            + ", ".join(missing_gap_tokens)
        )

    interaction_wire = "\n".join(
        f"{label}\n{method}\n{path}\n{body}" for label, method, path, body in interaction_expectations
    )
    missing_interaction_tokens = sorted(
        token for token in INTERACTION_CONVENIENCE_REQUIRED_TOKENS if token not in interaction_wire
    )
    if missing_interaction_tokens:
        errors.append(
            "EXPECT_INTERACTION_CONVENIENCE request corpus misses high-risk token(s): "
            + ", ".join(missing_interaction_tokens)
        )

    return len(gap_expectations), len(interaction_expectations)


def compact_json_body(expr: str, label: str) -> str:
    value = c_string_literal_value(expr)
    try:
        parsed = json.loads(value)
    except json.JSONDecodeError as exc:
        raise ValueError(f"{label} is not valid JSON: {exc}") from exc
    return json.dumps(parsed, separators=(",", ":"), ensure_ascii=False)


def require_token_set(errors: list[str], label: str, corpus: str, tokens: set[str]) -> int:
    missing = sorted(token for token in tokens if token not in corpus)
    if missing:
        errors.append(f"{label} misses builder payload token(s): {', '.join(missing)}")
    return len(tokens)


def audit_builder_payload_tokens(errors: list[str], smoke: str) -> tuple[int, int, int]:
    message_body = find_function_body(smoke, "run_public_rest_message_builder_smoke")
    message_expected = compact_json_body(
        find_char_string_assignment(message_body, "expected_body"),
        "message builder expected body",
    )
    message_tokens = require_token_set(
        errors,
        "message builder expected body",
        message_expected,
        MESSAGE_BUILDER_BODY_TOKENS,
    )

    command_body = find_function_body(smoke, "run_public_rest_application_command_builder_smoke")
    command_expected = compact_json_body(
        find_char_string_assignment(command_body, "expected_body"),
        "application command builder expected body",
    )
    command_tokens = require_token_set(
        errors,
        "application command builder expected body",
        command_expected,
        COMMAND_BUILDER_BODY_TOKENS,
    )
    command_array_corpus = c_string_literal_value(command_body) + "\n" + command_body
    command_tokens += require_token_set(
        errors,
        "application command builder array body",
        command_array_corpus,
        COMMAND_BUILDER_ARRAY_TOKENS,
    )

    interaction_body = (
        find_function_body(smoke, "run_public_rest_wrapper_smoke")
        + "\n"
        + find_function_body(smoke, "run_public_rest_wrapper_interactions_smoke")
    )
    interaction_corpus = c_string_literal_value(interaction_body)
    interaction_tokens = require_token_set(
        errors,
        "interaction builder expected bodies",
        interaction_corpus,
        INTERACTION_BUILDER_BODY_TOKENS,
    )

    return message_tokens, command_tokens, interaction_tokens


def audit_multipart_direct_smoke(errors: list[str], header: str, smoke: str) -> int:
    body = find_function_body(smoke, "run_public_rest_multipart_smoke")
    sticker_params = find_initializer(body, "dcc_guild_sticker_params_t", "sticker_params")
    require_fields(
        errors,
        header,
        "guild sticker multipart params",
        "dcc_guild_sticker_params",
        sticker_params,
    )
    required_literals = [
        'dcc_rest_create_guild_sticker_params(client, &sticker_params',
        '"/guilds/333/stickers"',
        '"name=\\"name\\"\\r\\n\\r\\nslap"',
        '"name=\\"description\\"\\r\\n\\r\\nslap desc"',
        '"name=\\"tags\\"\\r\\n\\r\\nslap"',
        '"name=\\"file\\"; filename=\\"slap.png\\"\\r\\nContent-Type: image/png\\r\\n\\r\\npng-data"',
    ]
    missing = [literal for literal in required_literals if literal not in body]
    if missing:
        errors.append(
            "guild sticker params multipart smoke misses expected literal(s): "
            + ", ".join(missing)
        )
    return len(required_literals)


def audit_all_params_structs(errors: list[str], header: str, smoke: str) -> None:
    structs = rest_param_structs(header)
    special_sources = {
        "dcc_auto_moderation_metadata_params_t": find_function_body(
            smoke,
            "call_rest_create_auto_moderation_rule_params",
        ),
    }

    audited = 0
    for typedef_name, fields in sorted(structs.items()):
        sources = find_initializers(smoke, typedef_name)
        if typedef_name in special_sources:
            sources.append(special_sources[typedef_name])
        if not sources:
            errors.append(f"{typedef_name} has no typed HTTP smoke initializer")
            continue

        expected = set(fields) - {"size"}
        actual: set[str] = set()
        for source in sources:
            actual.update(assigned_fields(source))
        missing = sorted(expected - actual)
        if missing:
            errors.append(f"{typedef_name} HTTP smoke misses field(s): {', '.join(missing)}")
        audited += 1

    if audited != len(structs):
        errors.append(f"REST params field audit covered {audited} of {len(structs)} params structs")


def main() -> int:
    header = read_rest_public_headers()
    smoke = read_http_smoke()
    errors: list[str] = []

    structs = rest_param_structs(header)
    audit_all_params_structs(errors, header, smoke)
    audited_helpers, audited_wire_fields = audit_helper_wire_tokens(errors, structs, smoke)
    audited_value_helpers, audited_wire_values = audit_helper_wire_values(errors, structs, smoke)
    audited_multipart_functions, audited_multipart_requests, audited_interaction_multipart_requests = (
        audit_multipart_expectations(errors, header, smoke)
    )
    audited_direct_gap_requests, audited_interaction_convenience_requests = audit_direct_request_expectations(
        errors,
        smoke,
    )
    audited_message_builder_tokens, audited_command_builder_tokens, audited_interaction_builder_tokens = (
        audit_builder_payload_tokens(errors, smoke)
    )
    audited_multipart_literals = audit_multipart_direct_smoke(errors, header, smoke)

    onboarding = find_function_body(smoke, "call_rest_modify_guild_onboarding_params")
    onboarding_emoji = find_initializer(onboarding, "dcc_onboarding_emoji_params_t", "emoji")
    require_fields(
        errors,
        header,
        "onboarding emoji params",
        "dcc_onboarding_emoji_params",
        onboarding_emoji,
    )

    role_connection = find_function_body(
        smoke,
        "call_rest_update_application_role_connection_metadata_params",
    )
    role_connection_metadata = find_initializer(
        role_connection,
        "dcc_application_role_connection_metadata_params_t",
        "metadata",
    )
    require_fields(
        errors,
        header,
        "application role connection metadata params",
        "dcc_application_role_connection_metadata_params",
        role_connection_metadata,
    )

    automod_create = find_function_body(smoke, "call_rest_create_auto_moderation_rule_params")
    automod_modify = find_function_body(smoke, "call_rest_modify_auto_moderation_rule_params")
    automod_actions = find_initializer(
        automod_create,
        "dcc_auto_moderation_action_params_t",
        "actions",
    )
    require_fields(
        errors,
        header,
        "auto moderation metadata params",
        "dcc_auto_moderation_metadata_params",
        automod_create,
    )
    require_fields(
        errors,
        header,
        "auto moderation action params",
        "dcc_auto_moderation_action_params",
        automod_actions,
    )
    require_fields(
        errors,
        header,
        "auto moderation rule params",
        "dcc_auto_moderation_rule_params",
        automod_create + automod_modify,
    )

    scheduled_create = find_function_body(smoke, "call_rest_create_guild_scheduled_event_params")
    scheduled_modify = find_function_body(smoke, "call_rest_modify_guild_scheduled_event_params")
    require_fields(
        errors,
        header,
        "scheduled event params",
        "dcc_scheduled_event_params",
        scheduled_create + scheduled_modify,
    )

    wrapper_smoke = find_function_body(smoke, "run_public_rest_wrapper_smoke")
    message_search = find_initializer(
        wrapper_smoke,
        "dcc_message_search_params_t",
        "search_params",
    )
    require_fields(
        errors,
        header,
        "guild message search params",
        "dcc_message_search_params",
        message_search,
    )

    require_literals(
        errors,
        smoke,
        "field-level REST request assertions",
        [
            '\\"image\\":\\"data:image/png;base64,AA==\\"',
            '\\"channel_id\\":\\"222\\"',
            "content=hello%20ops",
            "mentions_role_id=777",
            "replied_to_user_id=888",
            "replied_to_message_id=9999",
            "embed_provider=youtube",
            "attachment_filename=report.pdf",
            "mention_everyone=true",
            "offset=3",
            "slop=4",
            '\\"description_localizations\\":{\\"ko\\":\\"Rank score KO\\"}',
            '\\"keyword_filter\\":[\\"bad\\",\\"evil*\\"]',
            '\\"duration_seconds\\":60',
        ],
    )

    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1

    print(
        "rest field smoke audit passed: "
        f"{len(structs)} REST params structs covered; "
        f"{audited_wire_fields} non-path/control field wire tokens checked across "
        f"{audited_helpers} typed wrapper helper(s); "
        f"{audited_wire_values} path/control scalar wire values checked across "
        f"{audited_value_helpers} typed wrapper helper(s); "
        f"{audited_multipart_functions} public multipart function(s), "
        f"{audited_multipart_requests} direct multipart request expectation(s), "
        f"{audited_interaction_multipart_requests} interaction multipart expectation(s); "
        f"{audited_direct_gap_requests} direct gap request expectation(s), "
        f"{audited_interaction_convenience_requests} interaction convenience request expectation(s); "
        f"{audited_message_builder_tokens} message builder token(s), "
        f"{audited_command_builder_tokens} command builder token(s), "
        f"{audited_interaction_builder_tokens} interaction builder token(s); "
        f"{audited_multipart_literals} multipart/direct literals pinned; "
        "high-risk wire literals pinned"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
