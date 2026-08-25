#!/usr/bin/env python3
"""Compile public headers alone and reject accidental ABI removals."""

from __future__ import annotations

import argparse
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile


API_NAME = re.compile(r"\bDCC_API\b[\s\S]*?\b(dcc_[A-Za-z0-9_]+)\s*\(")


def api_names_from_text(text: str) -> set[str]:
    names: set[str] = set()
    for declaration in text.split("DCC_API")[1:]:
        match = re.search(r"\b(dcc_[A-Za-z0-9_]+)\s*\(", declaration)
        if match:
            names.add(match.group(1))
    return names


def current_api_names(include_dir: pathlib.Path) -> set[str]:
    names: set[str] = set()
    for header in include_dir.rglob("*.h"):
        relative = header.relative_to(include_dir).as_posix()
        if (relative == "sugar.h" or relative.startswith("sugar/") or
                relative == "app/legacy.h" or relative == "rest/core.h" or
                relative.startswith("rest/core/") or
                relative == "rest/response_helpers.h"):
            continue
        names.update(api_names_from_text(header.read_text(encoding="utf-8")))
    return names


def previous_api_names(source: pathlib.Path) -> set[str] | None:
    try:
        base = subprocess.run(
            ["git", "merge-base", "HEAD", "origin/main"],
            cwd=source,
            check=True,
            capture_output=True,
            text=True,
        ).stdout.strip()
        files = subprocess.run(
            ["git", "ls-tree", "-r", "--name-only", base, "include/dcc"],
            cwd=source,
            check=True,
            capture_output=True,
            text=True,
        ).stdout.splitlines()
    except (OSError, subprocess.CalledProcessError):
        return None

    names: set[str] = set()
    for filename in files:
        if not filename.endswith(".h"):
            continue
        try:
            text = subprocess.run(
                ["git", "show", f"{base}:{filename}"],
                cwd=source,
                check=True,
                capture_output=True,
                text=True,
            ).stdout
        except subprocess.CalledProcessError:
            continue
        names.update(api_names_from_text(text))
    return names


def intentional_api_removals(source: pathlib.Path) -> set[str]:
    manifest_path = source / "tools/rest_v2_endpoints.json"
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    endpoints = manifest.get("endpoints")
    if not isinstance(endpoints, list):
        raise ValueError("rest_v2_endpoints.json endpoints must be a list")

    symbols: set[str] = set()
    helpers = manifest.get("transition_helpers", [])
    for helper in helpers:
        symbol = helper.get("symbol") if isinstance(helper, dict) else None
        if isinstance(symbol, str):
            symbols.add(symbol)
    for index, endpoint in enumerate(endpoints):
        if not isinstance(endpoint, dict):
            raise ValueError(f"endpoint {index} must be an object")
        legacy_symbols = endpoint.get("legacy_symbols")
        if not isinstance(legacy_symbols, list) or not all(
            isinstance(symbol, str) and symbol.startswith("dcc_")
            for symbol in legacy_symbols
        ):
            raise ValueError(
                f"endpoint {index} legacy_symbols must contain dcc_ names"
            )
        symbols.update(legacy_symbols)
    removed = manifest.get("removed_candidates")
    if not isinstance(removed, list):
        raise ValueError("rest_v2_endpoints.json removed_candidates must be a list")
    for index, candidate in enumerate(removed):
        if not isinstance(candidate, dict):
            raise ValueError(f"removed candidate {index} must be an object")
        canonical = candidate.get("canonical")
        legacy_symbols = candidate.get("legacy_symbols")
        if not isinstance(canonical, str) or not canonical.startswith("dcc_"):
            raise ValueError(f"removed candidate {index} has an invalid canonical")
        if not isinstance(legacy_symbols, list) or not all(
            isinstance(symbol, str) and symbol.startswith("dcc_")
            for symbol in legacy_symbols
        ):
            raise ValueError(
                f"removed candidate {index} legacy_symbols must contain dcc_ names"
            )
        symbols.add(canonical)
        symbols.update(legacy_symbols)
    composites = manifest.get("transition_composites")
    if not isinstance(composites, list):
        raise ValueError("rest_v2_endpoints.json transition_composites must be a list")
    for index, composite in enumerate(composites):
        if not isinstance(composite, dict):
            raise ValueError(f"transition composite {index} must be an object")
        composite_symbols = composite.get("symbols")
        if not isinstance(composite_symbols, list) or not all(
            isinstance(symbol, str) and symbol.startswith("dcc_")
            for symbol in composite_symbols
        ):
            raise ValueError(
                f"transition composite {index} symbols must contain dcc_ names"
            )
        symbols.update(composite_symbols)
    symbols.add("dcc_flow_init")
    return symbols


def exported_symbols(library: pathlib.Path) -> set[str] | None:
    commands = (["nm", "-g", str(library)], ["nm", "-gU", str(library)])
    output = None
    for command in commands:
        try:
            output = subprocess.run(command, check=True, capture_output=True, text=True).stdout
            break
        except (OSError, subprocess.CalledProcessError):
            pass
    if output is None:
        return None
    symbols: set[str] = set()
    for line in output.splitlines():
        fields = line.split()
        if not fields:
            continue
        name = fields[-1]
        if name.startswith("_dcc_"):
            name = name[1:]
        if name.startswith("dcc_"):
            symbols.add(name)
    return symbols


def compile_headers(source: pathlib.Path, build: pathlib.Path) -> list[str]:
    compiler = os.environ.get("CC", "cc")
    if pathlib.Path(compiler).name.lower() in {
            "cl", "cl.exe", "clang-cl", "clang-cl.exe"}:
        return []
    include_dir = source / "include"
    include_args = [
        f"-I{include_dir}",
        f"-I{build / 'generated/include'}",
        f"-I{source.parent / 'LLAM/include'}",
    ]
    errors: list[str] = []
    with tempfile.TemporaryDirectory(prefix="dcc-header-audit-") as temp:
        unit = pathlib.Path(temp) / "header.c"
        for header in sorted((include_dir / "dcc").rglob("*.h")):
            relative = header.relative_to(include_dir).as_posix()
            if (relative == "dcc/sugar.h" or relative.startswith("dcc/sugar/") or
                    relative == "dcc/app/legacy.h" or relative == "dcc/rest/core.h" or
                    relative.startswith("dcc/rest/core/") or
                    relative == "dcc/rest/response_helpers.h"):
                continue
            header_text = header.read_text(encoding="utf-8")
            if header_text.startswith("/* Included by <dcc/"):
                # Declaration fragments are validated through their owning umbrella.
                continue
            unit.write_text(f"#include <{relative}>\nint main(void) {{ return 0; }}\n", encoding="utf-8")
            result = subprocess.run(
                [compiler, "-std=c11", "-fsyntax-only", *include_args, str(unit)],
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                detail = result.stderr.strip().splitlines()
                errors.append(f"{relative}: {detail[-1] if detail else 'compile failed'}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=pathlib.Path, required=True)
    parser.add_argument("--build", type=pathlib.Path, required=True)
    parser.add_argument("--library", type=pathlib.Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    current = current_api_names(source / "include/dcc")
    errors = compile_headers(source, args.build.resolve())
    baseline_path = source / "tools/api_v2_symbols.txt"
    if not baseline_path.is_file():
        errors.append("missing DCC 2 symbol baseline")
    else:
        baseline_lines = baseline_path.read_text(encoding="utf-8").splitlines()
        if baseline_lines != sorted(set(baseline_lines)):
            errors.append("DCC 2 symbol baseline is not sorted and unique")
        if set(baseline_lines) != current:
            errors.append("DCC 2 symbol baseline does not match installed declarations")

    previous = previous_api_names(source)
    project_text = (source / "CMakeLists.txt").read_text(encoding="utf-8")
    major_reset = bool(re.search(r"project\(dcc\s+VERSION\s+2\.", project_text))
    if previous is not None and not major_reset:
        try:
            allowed_removals = intentional_api_removals(source)
        except (OSError, ValueError, json.JSONDecodeError) as exc:
            errors.append(f"could not load intentional API removals: {exc}")
            allowed_removals = set()
        removed = sorted(previous - current - allowed_removals)
        if removed:
            errors.append("public API removals relative to origin/main: " + ", ".join(removed))

    symbols = exported_symbols(args.library.resolve())
    if symbols is not None:
        missing = sorted(current - symbols)
        extra = sorted(symbols - current)
        if missing:
            errors.append("DCC_API declarations missing from library: " + ", ".join(missing))
        if extra and args.library.resolve().suffix.lower() not in {".a", ".lib"}:
            errors.append("library exports absent from DCC 2 baseline: " + ", ".join(extra))

    if errors:
        print("public API audit failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    symbol_note = "not available" if symbols is None else str(len(symbols))
    print(f"public API audit passed: {len(current)} declarations, {symbol_note} library symbols")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
