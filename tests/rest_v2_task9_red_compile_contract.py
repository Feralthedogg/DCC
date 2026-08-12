#!/usr/bin/env python3
"""Compile the complete manifest-derived Task 9 surface."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess
import sys
import tempfile


def parameter_type(declaration: str) -> str:
    match = re.fullmatch(r"(.+?)([A-Za-z_][A-Za-z0-9_]*)", declaration.strip())
    if match is None:
        raise ValueError(f"cannot parse declaration: {declaration}")
    return match.group(1).rstrip()


def compile_unit(args: argparse.Namespace, text: str) -> subprocess.CompletedProcess[str]:
    with tempfile.TemporaryDirectory(prefix="dcc-task9-red-") as directory:
        unit = Path(directory) / "contract.c"
        unit.write_text(text, encoding="utf-8")
        include_dirs = (
            args.source / "include", args.binary / "generated/include",
            args.source / "src", args.source / "tests", args.llam_include,
        )
        command = [str(args.compiler), "-std=c11", "-fsyntax-only",
                   "-Werror=implicit-function-declaration"]
        command.extend(f"-I{path}" for path in include_dirs)
        command.append(str(unit))
        return subprocess.run(command, check=False, capture_output=True,
                              text=True, encoding="utf-8", errors="replace")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--compiler", type=Path, required=True)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--llam-include", type=Path, required=True)
    args = parser.parse_args()
    for name in ("compiler", "source", "binary", "llam_include"):
        setattr(args, name, getattr(args, name).resolve())

    manifest = json.loads((args.source / "tools/rest_v2_endpoints.json").read_text())
    rows = [row for row in manifest["endpoints"] if row["task"] == 9]
    active_aliases = {symbol for row in rows for symbol in row["legacy_symbols"]}
    composites = {
        symbol for item in manifest["transition_composites"]
        if item.get("removal_task") == 9 for symbol in item["symbols"]
    }
    stale = {
        "dcc_rest_bulk_edit_guild_command_permissions",
        "dcc_rest_bulk_edit_guild_command_permissions_params",
        "dcc_rest_create_guild_from_template",
        "dcc_rest_create_guild_from_template_params",
    }
    removed = active_aliases | composites | stale
    if len(rows) != 57 or len(active_aliases) != 30 or len(composites) != 5 or len(removed) != 39:
        print("Task 9 manifest-derived 57/30/5/39 inventory changed", file=sys.stderr)
        return 1

    canonical_lines = ["#include <dcc/rest.h>"]
    for row in rows:
        types = ", ".join(parameter_type(item["declaration"])
                          for item in row["canonical_parameters"])
        name = row["canonical"]
        canonical_lines.append(
            f"typedef dcc_status_t (*{name}_task9_fn)({types});\n"
            f"_Static_assert(_Generic(&{name}, {name}_task9_fn: 1, default: 0), "
            f"\"{name} signature\");")
    canonical = compile_unit(args, "\n".join(canonical_lines) + "\n")

    legacy_lines = ["#include <dcc/rest.h>", "void task9_legacy(void) {"]
    legacy_lines.extend(f"  (void)&{name};" for name in sorted(active_aliases | composites))
    legacy_lines.append("}")
    legacy = compile_unit(args, "\n".join(legacy_lines) + "\n")

    if canonical.returncode != 0:
        print("Task 9 canonical signatures do not compile:", file=sys.stderr)
        print(f"{canonical.stdout}\n{canonical.stderr}"[:12000], file=sys.stderr)
        return 1
    canonical_output = f"{canonical.stdout}\n{canonical.stderr}"
    if "No such file or directory" in canonical_output or "file not found" in canonical_output:
        print("Task 9 canonical compile failed because an include is unavailable",
              file=sys.stderr)
        return 1
    if legacy.returncode == 0:
        print("Task 9 removal symbols unexpectedly remain declared", file=sys.stderr)
        return 1
    print(
        "Task 9 compile contract confirmed: all 57 exact request signatures are "
        "available and all 39 alias/composite/stale symbols are absent")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
