#!/usr/bin/env python3
"""Compile all 44 exact Task 10 request signatures and removal names."""

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
    with tempfile.TemporaryDirectory(prefix="dcc-task10-red-") as directory:
        unit = Path(directory) / "contract.c"
        unit.write_text(text, encoding="utf-8")
        command = [str(args.compiler), "-std=c11", "-fsyntax-only",
                   "-Werror=implicit-function-declaration"]
        for path in (args.source / "include", args.binary / "generated/include",
                     args.source / "src", args.source / "tests", args.llam_include):
            command.append(f"-I{path}")
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
    rows = [row for row in manifest["endpoints"] if row["task"] == 10]
    aliases = {symbol for row in rows for symbol in row["legacy_symbols"]}
    removed = aliases | {
        "dcc_rest_official_body_json_free",
        "dcc_rest_create_direct_message",
        "dcc_rest_create_direct_message_builder",
        "dcc_rest_get_current_user_dms",
    }
    if len(rows) != 44 or len(aliases) != 28 or len(removed) != 32:
        print("Task 10 manifest-derived 44/28/32 inventory changed", file=sys.stderr)
        return 1

    canonical_lines = ["#include <dcc/rest.h>"]
    for row in rows:
        types = ", ".join(parameter_type(item["declaration"])
                          for item in row["canonical_parameters"])
        name = row["canonical"]
        canonical_lines.append(
            f"typedef dcc_status_t (*{name}_task10_fn)({types});\n"
            f"_Static_assert(_Generic(&{name}, {name}_task10_fn: 1, default: 0), "
            f"\"{name} signature\");")
    canonical = compile_unit(args, "\n".join(canonical_lines) + "\n")

    legacy_lines = ["#include <dcc/rest.h>", "void task10_legacy(void) {"]
    legacy_lines.extend(f"  (void)&{name};" for name in sorted(removed))
    legacy_lines.append("}")
    legacy = compile_unit(args, "\n".join(legacy_lines) + "\n")

    if canonical.returncode != 0:
        print("Task 10 canonical signatures do not compile:", file=sys.stderr)
        print(f"{canonical.stdout}\n{canonical.stderr}"[:16000], file=sys.stderr)
        return 1
    if legacy.returncode == 0:
        print("Task 10 removal symbols unexpectedly remain declared", file=sys.stderr)
        return 1
    print("Task 10 compile contract confirmed: 44 signatures and 32 removals")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
