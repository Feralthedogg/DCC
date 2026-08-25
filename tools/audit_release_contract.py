#!/usr/bin/env python3
"""Keep the source, generated header, package metadata, and runtime version aligned."""

from __future__ import annotations

import argparse
import pathlib
import re
import sys


def read(path: pathlib.Path) -> str:
    return path.read_text(encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=pathlib.Path, required=True)
    parser.add_argument("--build", type=pathlib.Path, required=True)
    args = parser.parse_args()

    source = args.source.resolve()
    build = args.build.resolve()
    errors: list[str] = []

    cmake = read(source / "CMakeLists.txt")
    match = re.search(r"project\(dcc\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)", cmake)
    if not match:
        errors.append("CMakeLists.txt does not declare a numeric DCC project version")
        version = ""
    else:
        version = match.group(1)
        if version != "2.0.0":
            errors.append(f"Stable release version must be exactly 2.0.0, found {version}")

    if "SOVERSION ${PROJECT_VERSION_MAJOR}" not in cmake:
        errors.append("shared library SOVERSION must follow ABI major 2")
    if 'set(DCC_LLAM_REQUIRED_VERSION "2.2.0")' not in cmake:
        errors.append("LLAM minimum must be the non-cache exact floor 2.2.0")
    app_aggregate = read(source / "include/dcc/app.h")
    app_includes = re.findall(r"#include <(dcc/app/[^>]+)>", app_aggregate)
    expected_app = [
        "dcc/app/base.h", "dcc/app/options.h", "dcc/app/lifecycle.h",
        "dcc/app/listeners.h", "dcc/app/context.h", "dcc/app/modules.h",
        "dcc/app/store.h", "dcc/app/env.h",
    ]
    if app_includes != expected_app:
        errors.append(f"<dcc/app.h> must expose the exact eight-header sequence: {app_includes}")
    dcc_aggregate = read(source / "include/dcc/dcc.h")
    if "#include <dcc/bot.h>" not in dcc_aggregate:
        errors.append("<dcc/dcc.h> must include <dcc/bot.h>")
    if "dcc/sugar" in dcc_aggregate or "dcc/app/legacy.h" in dcc_aggregate:
        errors.append("DCC 2 aggregate contains a removed compatibility edge")
    if "DCC 2.0.0 Stable" not in read(source / "README.md"):
        errors.append("README does not identify DCC 2.0.0 Stable")
    if not (source / "docs/reference/api/index.md").is_file():
        errors.append("generated DCC 2 API reference is missing")
    if not (source / "tools/api_v2_symbols.txt").is_file():
        errors.append("DCC 2 symbol baseline is missing")
    workflow = read(source / ".github/workflows/release.yml")
    if "- 'v2.0.0'" not in workflow or "prerelease: false" not in workflow:
        errors.append("release workflow must publish only v2.0.0 as non-prerelease")

    generated = build / "generated/include/dcc/version_generated.h"
    if not generated.is_file():
        errors.append(f"generated version header is missing: {generated}")
    elif version and f'#define DCC_VERSION_STRING "{version}"' not in read(generated):
        errors.append("generated version header does not match project(dcc VERSION ...)")

    expected_source_fragments = {
        source / "src/client/client_utils.c": "return DCC_VERSION_STRING;",
        source / "include/dcc/version.h": "#include <dcc/version_generated.h>",
        source / "cmake/dcc.pc.in": "Version: @PROJECT_VERSION@",
        source / "tools/package_release_windows.ps1": 'DCC_CMAKE_TOOLSET',
        source / ".github/workflows/release.yml": "DCC_CMAKE_TOOLSET: ClangCL",
    }
    for path, fragment in expected_source_fragments.items():
        if fragment not in read(path):
            errors.append(f"{path.relative_to(source)} must contain {fragment!r}")

    stale_release = re.compile(r"(?<![0-9])1\.4\.1(?![0-9])")
    for relative in ("docs/release.md", "docs/troubleshooting.md", ".github/workflows/release.yml"):
        path = source / relative
        for line_number, line in enumerate(read(path).splitlines(), 1):
            if stale_release.search(line):
                errors.append(f"{relative}:{line_number}: stale release version 1.4.1")

    if errors:
        print("release contract audit failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    print(f"release contract audit passed: DCC {version}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
