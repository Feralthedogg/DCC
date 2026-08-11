#!/usr/bin/env python3
"""Keep the Task 7 compile contract RED without breaking the normal build."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--llam-include", type=Path, required=True)
    return parser.parse_args()


def compile_command(
    compiler: Path,
    source_root: Path,
    binary_root: Path,
    llam_include: Path,
    translation_unit: Path,
) -> list[str]:
    include_directories = (
        source_root / "include",
        binary_root / "generated/include",
        source_root / "src",
        source_root / "tests",
        llam_include,
    )
    compiler_name = compiler.name.lower()
    if compiler_name in {"cl", "cl.exe"}:
        command = [
            str(compiler),
            "/nologo",
            "/std:c11",
            "/experimental:c11atomics",
            "/utf-8",
            "/Zs",
        ]
        command.extend(f"/I{directory}" for directory in include_directories)
    else:
        command = [
            str(compiler),
            "-std=c11",
            "-fsyntax-only",
            "-Werror=implicit-function-declaration",
        ]
        command.extend(f"-I{directory}" for directory in include_directories)
    command.append(str(translation_unit))
    return command


def run_compile(command: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def diagnostic_preview(result: subprocess.CompletedProcess[str]) -> str:
    output = "\n".join(part for part in (result.stdout, result.stderr) if part)
    return "\n".join(output.splitlines()[:24])


def main() -> int:
    args = parse_args()
    source_root = args.source.resolve()
    compiler = args.compiler.resolve()
    canonical_unit = source_root / "tests/support/rest_v2_endpoint_task7_contracts.c"
    legacy_unit = source_root / "tests/rest_v2_task7_legacy_surface_probe.c"
    manifest = json.loads(
        (source_root / "tools/rest_v2_endpoints.json").read_text(encoding="utf-8")
    )
    task7_entries = [entry for entry in manifest["endpoints"] if entry["task"] == 7]
    expected_canonical = {entry["canonical"] for entry in task7_entries}
    expected_legacy = {
        symbol for entry in task7_entries for symbol in entry["legacy_symbols"]
    }
    expected_sensitive = {
        entry["canonical"] for entry in manifest["endpoints"]
        if entry["sensitive_path"]
    }
    canonical_text = canonical_unit.read_text(encoding="utf-8")
    legacy_text = legacy_unit.read_text(encoding="utf-8")
    sensitive_probe_text = (
        source_root / "tests/support/rest_v2_task7_sensitive_probe.h"
    ).read_text(encoding="utf-8")
    preparation_probe_text = (
        source_root / "tests/support/rest_v2_task7_preparation_probe.h"
    ).read_text(encoding="utf-8")
    declared_contracts = set(re.findall(
        r"DCC_EXPECT_SIGNATURE\s*\(\s*(dcc_rest_[A-Za-z0-9_]+)",
        canonical_text,
    ))
    legacy_contracts = set(re.findall(
        r"&\s*(dcc_rest_[A-Za-z0-9_]+)\s*;",
        legacy_text,
    ))
    sensitive_contracts = set(re.findall(
        r"TASK7_EXPECT_SENSITIVE_FAILURE\s*\(\s*"
        r'"(dcc_rest_[A-Za-z0-9_]+)"',
        canonical_text,
    ))
    if declared_contracts != expected_canonical:
        print(
            "Task 7 signature contract differs from the exact 35-entry manifest: "
            f"missing={sorted(expected_canonical - declared_contracts)} "
            f"extra={sorted(declared_contracts - expected_canonical)}",
            file=sys.stderr,
        )
        return 1
    if legacy_contracts != expected_legacy:
        print(
            "Task 7 legacy compile probe differs from the exact 18 removals: "
            f"missing={sorted(expected_legacy - legacy_contracts)} "
            f"extra={sorted(legacy_contracts - expected_legacy)}",
            file=sys.stderr,
        )
        return 1
    if sensitive_contracts != expected_sensitive:
        print(
            "Sensitive runtime matrix differs from the exact 17 manifest entries: "
            f"missing={sorted(expected_sensitive - sensitive_contracts)} "
            f"extra={sorted(sensitive_contracts - expected_sensitive)}",
            file=sys.stderr,
        )
        return 1
    lifecycle_tokens = (
        "DCC_ENDPOINT_SENSITIVE_WIPED_COMPLETION",
        "DCC_ENDPOINT_SENSITIVE_WIPED_CANCEL",
        "DCC_ENDPOINT_SENSITIVE_WIPED_DESTROY",
        "DCC_ENDPOINT_SENSITIVE_WIPED_ADMISSION_FAILURE",
        "DCC_ENDPOINT_SENSITIVE_WIPED_ROLLBACK",
        "DCC_ENDPOINT_SENSITIVE_WIPED_CLIENT_TEARDOWN",
        "DCC_ENDPOINT_SENSITIVE_MATERIAL_ESCAPED_TOKEN",
        "DCC_ENDPOINT_SENSITIVE_MATERIAL_WIRE_PATH",
        "DCC_ENDPOINT_SENSITIVE_MATERIAL_ROUTE_DIGEST",
        "DCC_ENDPOINT_SENSITIVE_MATERIAL_BEARER_TOKEN",
        "DCC_ENDPOINT_SENSITIVE_MATERIAL_AUTHORIZATION",
        "DCC_ENDPOINT_PREPARATION_CHECK_OPERATION",
        "DCC_ENDPOINT_PREPARATION_CHECK_AUDIT_HEADER",
        "DCC_ENDPOINT_PREPARATION_CHECK_AUTHORIZATION",
        '"dcc_rest_submit"',
    )
    lifecycle_contract = (
        f"{canonical_text}\n{sensitive_probe_text}\n{preparation_probe_text}"
    )
    missing_lifecycle = [
        token for token in lifecycle_tokens if token not in lifecycle_contract
    ]
    if missing_lifecycle:
        print(
            "Task 7 sensitive lifecycle contract is incomplete: "
            f"missing={missing_lifecycle}",
            file=sys.stderr,
        )
        return 1

    canonical = run_compile(compile_command(
        compiler,
        source_root,
        args.binary.resolve(),
        args.llam_include.resolve(),
        canonical_unit,
    ))
    legacy = run_compile(compile_command(
        compiler,
        source_root,
        args.binary.resolve(),
        args.llam_include.resolve(),
        legacy_unit,
    ))

    if canonical.returncode == 0:
        print(
            "Task 7 RED compile contract unexpectedly became GREEN; "
            "register the support unit in dcc_rest_v2_endpoint_smoke and remove this wrapper.",
            file=sys.stderr,
        )
        return 1

    canonical_output = f"{canonical.stdout}\n{canonical.stderr}"
    task7_markers = (
        "dcc_rest_auth_mode",
        "audit_log_reason",
        "dcc_rest_create_forum_thread",
        "dcc_rest_thread_archive_query",
        "dcc_rest_invite_query",
        "DCC_ENDPOINT_SENSITIVE_PROBE",
        "static assertion",
        "Static_assert",
    )
    if not any(marker in canonical_output for marker in task7_markers):
        print("Canonical compile failed for an unrelated reason:", file=sys.stderr)
        print(diagnostic_preview(canonical), file=sys.stderr)
        return 1
    if "No such file or directory" in canonical_output or "file not found" in canonical_output:
        print("Canonical compile failed because an include was unavailable:", file=sys.stderr)
        print(diagnostic_preview(canonical), file=sys.stderr)
        return 1

    if legacy.returncode != 0:
        print(
            "Task 7 legacy baseline probe no longer compiles before the canonical contract is GREEN:",
            file=sys.stderr,
        )
        print(diagnostic_preview(legacy), file=sys.stderr)
        return 1

    print(
        "Task 7 RED compile contract confirmed: canonical channel/thread/invite API "
        "is unavailable and all 18 reviewed legacy symbols are still declared"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
