#!/usr/bin/env python3
"""Audit DCC route coverage against official discord-api-docs Route tags."""

from __future__ import annotations

import os
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

DOC_ROUTE_SOURCES = [
    "developers/resources",
    "developers/interactions",
    "developers/events/gateway.mdx",
    "developers/topics/oauth2.mdx",
]

IGNORED_DOC_ROUTE_SOURCES = {
    "developers/change-log.mdx",
}

SUPPLEMENTAL_OFFICIAL_FLOW_ROUTES = [
    ("POST", "/oauth2/token"),
    ("POST", "/oauth2/token/revoke"),
]

OFFICIAL_ROUTE_OVERRIDES = {
    ("GET", "/channels/{}/messages/{}/reactions/{}"): {
        "dcc_rest_get_message_reactions",
        "dcc_rest_get_message_reactions_page",
    },
    ("PUT", "/channels/{}/messages/{}/reactions/{}/@me"): {
        "dcc_rest_add_message_reaction",
        "dcc_rest_add_message_reaction_params",
    },
    ("DELETE", "/channels/{}/messages/{}/reactions/{}/@me"): {
        "dcc_rest_delete_own_message_reaction",
    },
    ("DELETE", "/channels/{}/messages/{}/reactions/{}/{}"): {
        "dcc_rest_delete_user_message_reaction",
    },
    ("DELETE", "/channels/{}/messages/{}/reactions/{}"): {
        "dcc_rest_delete_all_message_reactions_for_emoji",
    },
    ("GET", "/guilds/templates/{}"): {
        "dcc_rest_get_template",
    },
    ("PUT", "/guilds/{}/templates/{}"): {
        "dcc_rest_sync_guild_template",
    },
    ("PATCH", "/guilds/{}/templates/{}"): {
        "dcc_rest_modify_guild_template",
        "dcc_rest_modify_guild_template_params",
    },
    ("DELETE", "/guilds/{}/templates/{}"): {
        "dcc_rest_delete_guild_template",
    },
    ("POST", "/webhooks/{}/{}"): {
        "dcc_rest_execute_webhook",
        "dcc_rest_execute_webhook_options",
        "dcc_rest_interaction_followup_create",
        "dcc_rest_interaction_followup_create_builder",
        "dcc_rest_interaction_followup_create_multipart",
    },
    ("GET", "/webhooks/{}/{}"): {
        "dcc_rest_get_webhook_with_token",
    },
    ("PATCH", "/webhooks/{}/{}"): {
        "dcc_rest_modify_webhook_with_token",
        "dcc_rest_modify_webhook_with_token_params",
    },
    ("DELETE", "/webhooks/{}/{}"): {
        "dcc_rest_delete_webhook_with_token",
    },
    ("POST", "/webhooks/{}/{}/slack"): {
        "dcc_rest_execute_webhook_slack",
    },
    ("POST", "/webhooks/{}/{}/github"): {
        "dcc_rest_execute_webhook_github",
    },
    ("GET", "/webhooks/{}/{}/messages/@original"): {
        "dcc_rest_interaction_original_response_get",
    },
    ("PATCH", "/webhooks/{}/{}/messages/@original"): {
        "dcc_rest_interaction_original_response_edit",
        "dcc_rest_interaction_original_response_edit_builder",
        "dcc_rest_interaction_original_response_edit_multipart",
    },
    ("DELETE", "/webhooks/{}/{}/messages/@original"): {
        "dcc_rest_interaction_original_response_delete",
    },
    ("GET", "/webhooks/{}/{}/messages/{}"): {
        "dcc_rest_get_webhook_message",
        "dcc_rest_interaction_followup_get",
    },
    ("PATCH", "/webhooks/{}/{}/messages/{}"): {
        "dcc_rest_modify_webhook_message",
        "dcc_rest_modify_webhook_message_builder",
        "dcc_rest_modify_webhook_message_multipart",
        "dcc_rest_interaction_followup_edit",
        "dcc_rest_interaction_followup_edit_builder",
        "dcc_rest_interaction_followup_edit_multipart",
    },
    ("DELETE", "/webhooks/{}/{}/messages/{}"): {
        "dcc_rest_delete_webhook_message",
        "dcc_rest_interaction_followup_delete",
    },
}

REST_PUBLIC_ROOTS = [
    ROOT / "include/dcc/rest.h",
    ROOT / "include/dcc/rest",
    ROOT / "include/dcc/oauth2.h",
]

REST_SOURCE_ROOTS = [
    ROOT / "src/rest",
    ROOT / "src/oauth2.c",
]

STRING_RE = re.compile(r'"((?:\\.|[^"\\])*)"', re.S)
ROUTE_RE = re.compile(r'<Route\s+method="([A-Z]+)">\s*(.*?)\s*</Route>', re.S)
LINK_RE = re.compile(r"\[([^\]]+)\]\([^)]*\)")
PUBLIC_SYMBOL_RE = re.compile(r"DCC_API\s+dcc_status_t\s+(dcc_(?:rest|oauth2)_[A-Za-z0-9_]+)\s*\(")
SOURCE_SYMBOL_RE = re.compile(r"^dcc_status_t\s+(dcc_(?:rest|oauth2)_[A-Za-z0-9_]+)\s*\(", re.M)
METHOD_TOKEN_RE = re.compile(r"\bDCC_REST_(GET|POST|PUT|PATCH|DELETE)\b")
QUOTED_METHODS = {"GET", "POST", "PUT", "PATCH", "DELETE"}


def docs_root() -> Path:
    configured = os.environ.get("DCC_DISCORD_API_DOCS_ROOT")
    root = Path(configured) if configured else Path("/tmp/discord-api-docs")
    return root / "developers" if (root / "developers").is_dir() else root


def docs_repo_root(root: Path) -> Path:
    return root.parent if root.name == "developers" else root


def route_source_paths(root: Path) -> list[Path]:
    paths: list[Path] = []
    repo_root = docs_repo_root(root)
    for rel in DOC_ROUTE_SOURCES:
        path = repo_root / rel
        if path.is_dir():
            paths.extend(sorted(path.rglob("*.mdx")))
        elif path.is_file():
            paths.append(path)
    return paths


def route_tag_source_relatives(root: Path) -> set[str]:
    repo_root = docs_repo_root(root)
    developers = repo_root / "developers"
    if not developers.is_dir():
        developers = root

    sources: set[str] = set()
    for path in developers.rglob("*.mdx"):
        text = path.read_text(encoding="utf-8", errors="ignore")
        if ROUTE_RE.search(text):
            sources.add(path.relative_to(repo_root).as_posix())
    return sources


def configured_route_source_relatives(root: Path) -> set[str]:
    repo_root = docs_repo_root(root)
    return {path.relative_to(repo_root).as_posix() for path in route_source_paths(root)}


def normalize_doc_route(route: str) -> str:
    route = LINK_RE.sub(lambda match: match.group(1), route)
    route = route.replace(r"\{", "{").replace(r"\}", "}")
    route = route.replace("&lt;", "<").replace("&gt;", ">")
    return "".join(route.split())


def canonical_path(path: str) -> str:
    path = path.split("?", 1)[0]
    path = re.sub(r"%\.\*s|%ll[ud]|%zu|%u|%d|%s", "{}", path)
    path = re.sub(r"\{[^}/]+\}", "{}", path)
    path = re.sub(r"/[0-9]{2,}(?=/|$)", "/{}", path)
    path = re.sub(r"/[^/]*%2F[^/]*(?=/|$)", "/{}", path)
    return path


def decode_c_string(value: str) -> str:
    try:
        return bytes(value, "utf-8").decode("unicode_escape")
    except UnicodeDecodeError:
        return value


def official_routes(root: Path) -> tuple[set[tuple[str, str]], list[str]]:
    sources = route_source_paths(root)
    if not sources:
        raise FileNotFoundError(
            f"discord-api-docs route sources not found under {root}; "
            "set DCC_DISCORD_API_DOCS_ROOT to a checkout of discord-api-docs"
        )

    routes: set[tuple[str, str]] = set()
    route_labels: list[str] = []
    for source in sources:
        text = source.read_text(encoding="utf-8")
        for match in ROUTE_RE.finditer(text):
            method = match.group(1)
            route = normalize_doc_route(match.group(2))
            routes.add((method, route))
            route_labels.append(f"{method} {route}")
    for method, path in SUPPLEMENTAL_OFFICIAL_FLOW_ROUTES:
        routes.add((method, path))
        route_labels.append(f"{method} {path}")
    return routes, route_labels


def text_from_roots(roots: list[Path]) -> str:
    parts: list[str] = []
    for root in roots:
        if root.is_file():
            parts.append(root.read_text(encoding="utf-8", errors="ignore"))
        elif root.is_dir():
            parts.extend(
                path.read_text(encoding="utf-8", errors="ignore")
                for path in sorted(root.rglob("*.h"))
            )
    return "\n".join(parts)


def source_files() -> list[Path]:
    files: list[Path] = []
    for root in REST_SOURCE_ROOTS:
        if root.is_file():
            files.append(root)
        elif root.is_dir():
            files.extend(sorted(root.glob("*.c")))
    return files


def public_symbols() -> set[str]:
    return set(PUBLIC_SYMBOL_RE.findall(text_from_roots(REST_PUBLIC_ROOTS)))


def source_symbols() -> set[str]:
    symbols: set[str] = set()
    for source in source_files():
        symbols.update(SOURCE_SYMBOL_RE.findall(source.read_text(encoding="utf-8", errors="ignore")))
    return symbols


def function_body(text: str, open_brace: int) -> str:
    depth = 0
    for index in range(open_brace, len(text)):
        char = text[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[open_brace : index + 1]
    return text[open_brace:]


def body_methods(body: str) -> set[str]:
    methods = set(METHOD_TOKEN_RE.findall(body))
    for match in STRING_RE.finditer(body):
        value = decode_c_string(match.group(1))
        if value in QUOTED_METHODS:
            methods.add(value)
    return methods


def body_paths(body: str) -> set[str]:
    paths: set[str] = set()
    for match in STRING_RE.finditer(body):
        value = decode_c_string(match.group(1))
        if value.startswith("/"):
            paths.add(canonical_path(value))
    return paths


def route_symbols(public: set[str], official: set[tuple[str, str]] | None = None) -> dict[tuple[str, str], set[str]]:
    official_canonical = None
    if official is not None:
        official_canonical = {(method, canonical_path(path)) for method, path in official}

    routes: dict[tuple[str, str], set[str]] = {}
    for source in source_files():
        text = source.read_text(encoding="utf-8", errors="ignore")
        for match in SOURCE_SYMBOL_RE.finditer(text):
            symbol = match.group(1)
            if symbol not in public:
                continue
            open_brace = text.find("{", match.end())
            if open_brace < 0:
                continue
            body = function_body(text, open_brace)
            for method in body_methods(body):
                for path in body_paths(body):
                    route = (method, path)
                    if official_canonical is not None and route not in official_canonical:
                        continue
                    routes.setdefault(route, set()).add(symbol)

    for route, symbols in OFFICIAL_ROUTE_OVERRIDES.items():
        routes.setdefault(route, set()).update(symbols)
    return routes


def main() -> int:
    try:
        docs = docs_root()
        official, labels = official_routes(docs)
    except OSError as exc:
        print(f"discord api docs surface audit failed: {exc}", file=sys.stderr)
        return 2

    official_canonical = {(method, canonical_path(path)) for method, path in official}
    public = public_symbols()
    source = source_symbols()
    implemented = route_symbols(public, official)

    errors: list[str] = []
    unknown_route_sources = (
        route_tag_source_relatives(docs)
        - configured_route_source_relatives(docs)
        - IGNORED_DOC_ROUTE_SOURCES
    )
    for source in sorted(unknown_route_sources):
        errors.append(f"route source not classified by audit: {source}")

    for method, path in sorted(official_canonical):
        if (method, path) not in implemented:
            errors.append(f"route missing public wrapper: {method} {path}")

    for route, symbols in sorted(OFFICIAL_ROUTE_OVERRIDES.items()):
        for symbol in sorted(symbols):
            if symbol not in public:
                errors.append(f"override {route[0]} {route[1]} references non-public symbol: {symbol}")
            if symbol not in source:
                errors.append(f"override {route[0]} {route[1]} references unimplemented symbol: {symbol}")

    if errors:
        print("discord api docs surface audit failed:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1

    print(
        "discord api docs surface audit passed: "
        f"{len(set(labels))} official route entries, "
        f"{len(official)} documented method/path routes, "
        f"{len(official_canonical)} canonical method/path routes, "
        f"{len(implemented)} DCC public route bindings"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
