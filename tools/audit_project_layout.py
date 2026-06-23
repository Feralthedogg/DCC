#!/usr/bin/env python3
from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
PUBLIC_INCLUDE = ROOT / "include" / "dcc"
INTERNAL_INCLUDE = ROOT / "src" / "internal"
MAX_SOURCE_LINES = 2500
MAX_PUBLIC_HEADER_LINES = 1800
MAX_TOOL_FILE_LINES = 500
MAX_TEST_FILE_LINES = 2000
MAX_EXAMPLE_FILE_LINES = 500
KNOWN_LARGE_FILE_LIMITS = {
    "tools/dcc_new_app_action.c": 650,
    "tools/dcc_new_app_options.c": 650,
    "tools/dcc_new_app_routes.c": 600,
    "tools/dcc_new_app_view.c": 650,
    "tests/app_smoke.c": 2100,
    "tests/package_consumer/package_consumer_app.c": 3000,
    "tests/sugar_smoke.c": 5000,
}


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def line_count(path: Path) -> int:
    return path.read_text(encoding="utf-8", errors="replace").count("\n") + 1


def audit_file_size(path: Path, limit: int, errors: list[str]) -> None:
    relative = rel(path)
    budget = KNOWN_LARGE_FILE_LIMITS.get(relative, limit)
    lines = line_count(path)
    if lines > budget:
        if relative in KNOWN_LARGE_FILE_LIMITS:
            errors.append(
                f"known oversized file grew past its split budget: {relative} "
                f"({lines} lines > {budget})"
            )
        else:
            errors.append(
                f"source file is too large; split by module responsibility: {relative} "
                f"({lines} lines > {budget})"
            )


def main() -> int:
    errors: list[str] = []

    if not PUBLIC_INCLUDE.is_dir():
        errors.append("missing public include root: include/dcc")
    if not INTERNAL_INCLUDE.is_dir():
        errors.append("missing internal include root: src/internal")
    if not (ROOT / "docs" / "architecture.md").is_file():
        errors.append("missing docs/architecture.md")
    if not (ROOT / "third_party" / "README.md").is_file():
        errors.append("missing third_party/README.md")
    for template in (
        "deploy/interaction-webhook/README.md",
        "deploy/interaction-webhook/dcc-interaction-webhook.env",
        "deploy/interaction-webhook/dcc-interaction-webhook.service",
        "deploy/interaction-webhook/nginx.conf",
        "deploy/interaction-webhook/docker-compose.yaml",
        "deploy/interaction-webhook/kubernetes.yaml",
        "deploy/interaction-webhook/Caddyfile",
        "deploy/interaction-webhook/Containerfile",
        "deploy/hot-reload/README.md",
        "deploy/hot-reload/dcc-hot-reload.env",
        "deploy/hot-reload/dcc-hot-reload.service",
        "deploy/hot-reload/dcc-hot-reload.tmpfiles.conf",
        "deploy/hot-reload/Caddyfile",
        "deploy/hot-reload/Containerfile",
        "deploy/hot-reload/docker-compose.yaml",
    ):
        if not (ROOT / template).is_file():
            errors.append(f"missing deployment template: {template}")

    for header in sorted((ROOT / "include").rglob("*.h")):
        if PUBLIC_INCLUDE not in header.parents:
            errors.append(f"public header is outside include/dcc: {rel(header)}")
            continue
        audit_file_size(header, MAX_PUBLIC_HEADER_LINES, errors)
        text = header.read_text(encoding="utf-8", errors="replace")
        if re.search(r'#\s*include\s*[<"](?:internal/|src/internal/)', text):
            errors.append(f"public header includes an internal header: {rel(header)}")

    for header in sorted((ROOT / "src").rglob("*.h")):
        if INTERNAL_INCLUDE not in header.parents:
            errors.append(f"source header must live under src/internal: {rel(header)}")

    for source_root in ("src", "tools", "tests", "examples", "fuzz"):
        root = ROOT / source_root
        if not root.exists():
            continue
        for path in sorted(root.rglob("*.[ch]")):
            if source_root == "src" and path.suffix == ".c":
                audit_file_size(path, MAX_SOURCE_LINES, errors)
            if source_root == "tools":
                audit_file_size(path, MAX_TOOL_FILE_LINES, errors)
            if source_root == "tests":
                audit_file_size(path, MAX_TEST_FILE_LINES, errors)
            if source_root == "examples":
                audit_file_size(path, MAX_EXAMPLE_FILE_LINES, errors)
            text = path.read_text(encoding="utf-8", errors="replace")
            if '#include "../' in text or "#include \"../" in text:
                errors.append(f"relative parent include is not allowed: {rel(path)}")
            if (
                rel(path) != "src/internal/dcc_internal.h" and
                re.search(r'#\s*include\s*"internal/dcc_internal\.h"', text)
            ):
                errors.append(
                    f"use a domain-specific internal header instead of dcc_internal.h: {rel(path)}"
                )

    if errors:
        print("DCC project layout audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("DCC project layout audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
