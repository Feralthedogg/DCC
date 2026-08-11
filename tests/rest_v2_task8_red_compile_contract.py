#!/usr/bin/env python3
"""Compile the Task 8 target surface out-of-band and require it to stay RED."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys


def compile_command(args: argparse.Namespace, unit: Path) -> list[str]:
    include_dirs = (
        args.source / "include",
        args.binary / "generated/include",
        args.source / "src",
        args.source / "tests",
        args.llam_include,
    )
    command = [str(args.compiler), "-std=c11", "-fsyntax-only", "-Werror=implicit-function-declaration"]
    command.extend(f"-I{directory}" for directory in include_dirs)
    command.append(str(unit))
    return command


def compile_unit(args: argparse.Namespace, unit: Path) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        compile_command(args, unit),
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--llam-include", type=Path, required=True)
    args = parser.parse_args()
    for name in ("compiler", "source", "binary", "llam_include"):
        setattr(args, name, getattr(args, name).resolve())

    canonical_unit = args.source / "tests/support/rest_v2_endpoint_task8_contracts.c"
    legacy_unit = args.source / "tests/rest_v2_task8_legacy_surface_probe.c"
    manifest = json.loads((args.source / "tools/rest_v2_endpoints.json").read_text())
    entries = [entry for entry in manifest["endpoints"] if entry["task"] == 8]
    expected_canonical = {entry["canonical"] for entry in entries}
    expected_legacy = {
        symbol for entry in entries for symbol in entry["legacy_symbols"]
    }
    canonical_text = canonical_unit.read_text()
    legacy_text = legacy_unit.read_text()
    declared = set(re.findall(
        r"DCC_EXPECT_TASK8_SIGNATURE\s*\(\s*(dcc_rest_[A-Za-z0-9_]+)",
        canonical_text,
    ))
    legacy = set(re.findall(r"&\s*(dcc_rest_[A-Za-z0-9_]+)\s*;", legacy_text))
    if declared != expected_canonical or legacy != expected_legacy:
        print(
            "Task 8 compile probes differ from the 47/32 manifest contract: "
            f"canonical_missing={sorted(expected_canonical-declared)} "
            f"canonical_extra={sorted(declared-expected_canonical)} "
            f"legacy_missing={sorted(expected_legacy-legacy)} "
            f"legacy_extra={sorted(legacy-expected_legacy)}",
            file=sys.stderr,
        )
        return 1

    canonical_result = compile_unit(args, canonical_unit)
    legacy_result = compile_unit(args, legacy_unit)
    if canonical_result.returncode == 0:
        print(
            "Task 8 canonical compile contract unexpectedly became GREEN; "
            "register the support unit and remove this wrapper.",
            file=sys.stderr,
        )
        return 1
    output = f"{canonical_result.stdout}\n{canonical_result.stderr}"
    markers = (
        "dcc_rest_guild_member_add_t",
        "dcc_rest_guild_update_t",
        "dcc_rest_guild_role_position_t",
        "static assertion",
        "Static_assert",
    )
    if not any(marker in output for marker in markers):
        print("Canonical compile failed for an unrelated reason:", file=sys.stderr)
        print("\n".join(output.splitlines()[:30]), file=sys.stderr)
        return 1
    if "No such file or directory" in output or "file not found" in output:
        print("Canonical compile failed because an include was unavailable", file=sys.stderr)
        return 1
    if legacy_result.returncode != 0:
        legacy_output = f"{legacy_result.stdout}\n{legacy_result.stderr}"
        print("Task 8 legacy baseline probe no longer compiles:", file=sys.stderr)
        print("\n".join(legacy_output.splitlines()[:30]), file=sys.stderr)
        return 1

    print(
        "Task 8 RED compile contract confirmed: the 47 typed request signatures "
        "are unavailable and all 32 reviewed legacy symbols remain declared"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
