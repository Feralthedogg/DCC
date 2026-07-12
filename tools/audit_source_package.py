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


def require_text(relative: str, text: str, needle: str, errors: list[str]) -> None:
    if needle not in text:
        errors.append(f"{relative} is missing required text: {needle}")


def main() -> int:
    errors: list[str] = []
    gitignore = read(".gitignore", errors)
    cmake = read("CMakeLists.txt", errors)
    release_check = read("tools/release_check.sh", errors)
    package_release = read("tools/package_release.sh", errors)
    bsd_workflow = read(".github/workflows/bsd.yml", errors)

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
        '"/CMakeFiles/"': "in-source CMakeFiles directories",
        '"/CMakeCache[.]txt$"': "in-source CMake caches",
        '"/cmake_install[.]cmake$"': "in-source install scripts",
        '"/install_manifest[.]txt$"': "in-source install manifests",
        '"/Testing/"': "in-source CTest output",
        '"/site/"': "MkDocs output",
        '"/target/"': "release artifacts",
        '"/dist/"': "distribution artifacts",
        '"/[.]venv/"': "Python virtualenvs",
        '"/compile_commands[.]json$"': "compile_commands.json",
        '"/__pycache__/"': "__pycache__ directories",
        '"[.]pyc$"': "Python bytecode",
        '"/[.]DS_Store$"': ".DS_Store files",
        '"/[.]env($|[.])"': "hidden .env files",
        '"/[.]git/"': ".git directories",
    }
    for needle, label in required_cpack_excludes.items():
        if needle not in cmake:
            errors.append(f"CMakeLists.txt CPACK_SOURCE_IGNORE_FILES must exclude {label}")

    require_text(
        ".github/workflows/bsd.yml",
        bsd_workflow,
        "-DDCC_BUILD_TOOLS=ON",
        errors,
    )

    for template in (
        "deploy/interaction-webhook/dcc-interaction-webhook.env",
        "deploy/hot-reload/dcc-hot-reload.env",
        "deploy/bot/dcc-bot.env",
    ):
        if not (ROOT / template).is_file():
            errors.append(f"missing deployment env template: {template}")

    if "PATTERN \"*.env\"" not in cmake:
        errors.append("CMake install rules must keep deployment .env templates installed")
    deploy_install_start = cmake.find("DIRECTORY deploy/")
    deploy_install_end = cmake.find("\n)\n", deploy_install_start)
    deploy_install = cmake[deploy_install_start:deploy_install_end]
    if deploy_install_start < 0 or 'PATTERN "*.sh"' not in deploy_install:
        errors.append("CMake install rules must keep deployment shell entrypoints installed")
    if deploy_install_start < 0 or "USE_SOURCE_PERMISSIONS" not in deploy_install:
        errors.append("CMake install rules must preserve deployment entrypoint permissions")

    official_surface_package_entries = (
        "/docs/reference/official-api-surface.md$",
        "/include/dcc/oauth2.h$",
        "/include/dcc/rest/official_surface.h$",
        "/include/dcc/sugar/official_surface.h$",
        "/include/dcc/webhook_events.h$",
        "/src/gateway/gateway_send_public.c$",
        "/src/oauth2.c$",
        "/src/rest/rest_official_surface.c$",
        "/src/webhook_events.c$",
        "/tests/official_surface_smoke.c$",
        "/tests/package_consumer/package_consumer_official_surface.c$",
        "/tests/support/http_smoke_official_surface.c$",
        "/tools/audit_discord_api_docs_surface.py$",
        "/tools/audit_official_events_surface.py$",
        "/tools/audit_official_surface.py$",
    )
    for entry in official_surface_package_entries:
        require_text("tools/release_check.sh", release_check, entry, errors)
        require_text("tools/package_release.sh", package_release, entry, errors)

    require_text(
        "tools/release_check.sh",
        release_check,
        "/deploy/bot/entrypoint.sh$",
        errors,
    )
    require_text(
        "tools/package_release.sh",
        package_release,
        "/share/dcc/deploy/bot/entrypoint.sh$",
        errors,
    )
    require_text(
        "tools/package_release.sh",
        package_release,
        "DCC release package is missing dcc_doctor; configure with DCC_BUILD_TOOLS=ON",
        errors,
    )

    if errors:
        print("DCC source package audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("DCC source package audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
