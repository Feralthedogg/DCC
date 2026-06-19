#!/usr/bin/env python3
"""Audit source-package exclusions for local secret/cache files."""

from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(relative: str, errors: list[str]) -> str:
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"missing required file: {relative}")
        return ""
    return path.read_text(encoding="utf-8", errors="replace")


def require_line(relative: str, text: str, line: str, errors: list[str]) -> None:
    lines = {entry.strip() for entry in text.splitlines()}
    if line not in lines:
        errors.append(f"{relative} is missing required line: {line}")


def main() -> int:
    errors: list[str] = []
    gitignore = read(".gitignore", errors)
    cmake = read("CMakeLists.txt", errors)

    require_line(".gitignore", gitignore, ".env", errors)
    require_line(".gitignore", gitignore, ".env.*", errors)
    require_line(".gitignore", gitignore, "__pycache__/", errors)
    require_line(".gitignore", gitignore, "*.pyc", errors)
    require_line(".gitignore", gitignore, "compile_commands.json", errors)
    require_line(".gitignore", gitignore, ".DS_Store", errors)
    require_line(".gitignore", gitignore, "site/", errors)
    require_line(".gitignore", gitignore, "target/", errors)
    require_line(".gitignore", gitignore, ".venv/", errors)

    required_cpack_excludes = {
        '"/build[^/]*/"': "build trees",
        '"/site/"': "MkDocs output",
        '"/target/"': "release artifacts",
        '"/dist/"': "distribution artifacts",
        '"/\\\\\\\\.venv/"': "Python virtualenvs",
        '"/compile_commands\\\\\\\\.json$"': "compile_commands.json",
        '"/__pycache__/"': "__pycache__ directories",
        '"\\\\\\\\.pyc$"': "Python bytecode",
        '"/\\\\\\\\.DS_Store$"': ".DS_Store files",
        '"/\\\\\\\\.env($|\\\\\\\\.)"': "hidden .env files",
        '"/\\\\\\\\.git/"': ".git directories",
    }
    for needle, label in required_cpack_excludes.items():
        if needle not in cmake:
            errors.append(f"CMakeLists.txt CPACK_SOURCE_IGNORE_FILES must exclude {label}")

    for template in (
        "deploy/interaction-webhook/dcc-interaction-webhook.env",
        "deploy/hot-reload/dcc-hot-reload.env",
    ):
        if not (ROOT / template).is_file():
            errors.append(f"missing deployment env template: {template}")

    if "PATTERN \"*.env\"" not in cmake:
        errors.append("CMake install rules must keep deployment .env templates installed")

    if errors:
        print("DCC source package audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("DCC source package audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
