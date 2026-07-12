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
