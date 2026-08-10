#!/usr/bin/env python3
"""Enforce the DCC 2 public-header contract.

Strict mode is the default and is intended for the completed 2.0 surface.
``--transition`` keeps the current 1.x headers buildable while explicitly
reporting debt that later API tasks must remove.  It never hides configuration
or unrelated C/C++ public-header compile failures.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import sys
import tempfile


MAX_BOT_MACROS = 300
FRAGMENT_HEADERS = frozenset({
    "dcc/app/core_decls.h",
    "dcc/app/rest_decls_a.h",
    "dcc/app/rest_decls_b.h",
    "dcc/app/context_decls.h",
})
IDENTIFIER = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
MACRO = re.compile(r"^\s*#\s*define\s+(DCC_[A-Za-z0-9_]*)\b", re.MULTILINE)
PUBLIC_INCLUDE = re.compile(r"^\s*#\s*include\s*<((?:dcc/)[A-Za-z0-9_./-]+)>", re.MULTILINE)
API = re.compile(r"\bDCC_API\b")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True, help="DCC source root")
    parser.add_argument("--compiler", required=True, help="C compiler command or path")
    parser.add_argument("--cxx-compiler", help="C++ compiler command or path")
    parser.add_argument("--transition", action="store_true", help="record known 1.x debt")
    return parser.parse_args()


def command_parts(command: str) -> list[str]:
    parts = shlex.split(command, posix=os.name != "nt")
    if not parts or shutil.which(parts[0]) is None:
        raise ValueError(f"compiler is not executable: {command}")
    return parts


def default_cxx(c_compiler: list[str]) -> str:
    configured = os.environ.get("CXX")
    if configured:
        return configured
    name = Path(c_compiler[0]).name.lower()
    replacements = {
        "cc": "c++", "gcc": "g++", "clang": "clang++", "icc": "icpc",
        "icx": "icpx", "cl": "cl", "cl.exe": "cl.exe",
    }
    return replacements.get(name, "c++")


def find_llam_include(source: Path) -> Path | None:
    candidates: list[Path] = []
    if root := os.environ.get("DCC_LLAM_ROOT"):
        candidates.append(Path(root) / "include")
    for ancestor in (source, *source.parents):
        candidates.append(ancestor / "LLAM" / "include")
    for candidate in candidates:
        if (candidate / "llam/runtime.h").is_file():
            return candidate
    return None


def version_defines(source: Path) -> str:
    cmake = (source / "CMakeLists.txt").read_text(encoding="utf-8")
    match = re.search(r"project\(dcc\s+VERSION\s+([0-9]+)\.([0-9]+)\.([0-9]+)", cmake)
    if match is None:
        raise ValueError("CMake project version is missing or malformed")
    major, minor, patch = match.groups()
    return (
        "#ifndef DCC_VERSION_GENERATED_H\n#define DCC_VERSION_GENERATED_H\n"
        f"#define DCC_VERSION_MAJOR {major}\n#define DCC_VERSION_MINOR {minor}\n"
        f"#define DCC_VERSION_PATCH {patch}\n"
        f"#define DCC_VERSION_STRING \"{major}.{minor}.{patch}\"\n#endif\n"
    )


def load_banned_names(path: Path) -> list[str]:
    if not path.is_file():
        raise ValueError(f"missing removed-name configuration: {path}")
    entries: list[str] = []
    for number, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        value = raw.split("#", 1)[0].strip()
        if not value:
            continue
        token = value[:-1] if value.endswith("*") else value
        if not IDENTIFIER.fullmatch(token):
            raise ValueError(f"invalid removed-name entry at {path}:{number}: {value}")
        entries.append(value)
    if not entries:
        raise ValueError(f"removed-name configuration is empty: {path}")
    if len(entries) != len(set(entries)):
        raise ValueError(f"removed-name configuration has duplicate entries: {path}")
    return entries


def is_msvc(command: list[str]) -> bool:
    return Path(command[0]).name.lower() in {"cl", "cl.exe"}


def compile_command(
    compiler: list[str], language: str, includes: list[Path], unit: Path
) -> list[str]:
    if is_msvc(compiler):
        standard = "/std:c11" if language == "c" else "/std:c++17"
        return [*compiler, "/nologo", standard, "/Zs", *(f"/I{item}" for item in includes), str(unit)]
    standard = "-std=c11" if language == "c" else "-std=c++17"
    return [*compiler, standard, "-fsyntax-only", *(f"-I{item}" for item in includes), str(unit)]


def compile_headers(
    source: Path, c_compiler: list[str], cxx_compiler: list[str], includes: list[Path]
) -> tuple[list[str], list[str], list[str]]:
    headers = sorted((source / "include/dcc").rglob("*.h"))
    errors: list[str] = []
    fragment_failures: list[str] = []
    fragment_successes: list[str] = []
    with tempfile.TemporaryDirectory(prefix="dcc-v2-header-audit-") as temp_dir:
        temp = Path(temp_dir)
        for header in headers:
            relative = header.relative_to(source / "include").as_posix()
            for language, compiler, suffix in (
                ("c", c_compiler, "c"), ("c++", cxx_compiler, "cpp")
            ):
                unit = temp / f"header.{suffix}"
                unit.write_text(
                    f"#include <{relative}>\nint main(void) {{ return 0; }}\n",
                    encoding="utf-8",
                )
                result = subprocess.run(
                    compile_command(compiler, language, includes, unit),
                    capture_output=True,
                    text=True,
                )
                if result.returncode == 0:
                    if relative in FRAGMENT_HEADERS:
                        fragment_successes.append(f"{relative} ({language})")
                    continue
                detail = (result.stderr or result.stdout).strip().splitlines()
                message = f"{relative} ({language}): {detail[-1] if detail else 'compile failed'}"
                if relative in FRAGMENT_HEADERS:
                    fragment_failures.append(message)
                else:
                    errors.append(message)
    return errors, fragment_failures, fragment_successes


def documented_before(lines: list[str], index: int) -> bool:
    cursor = index - 1
    while cursor >= 0 and not lines[cursor].strip():
        cursor -= 1
    if cursor < 0 or "*/" not in lines[cursor]:
        return False
    while cursor >= 0:
        if "/**" in lines[cursor]:
            return True
        if "/*" in lines[cursor]:
            return False
        cursor -= 1
    return False


def undocumented_api_declarations(headers: list[Path]) -> list[str]:
    missing: list[str] = []
    for header in headers:
        lines = header.read_text(encoding="utf-8").splitlines()
        for index, line in enumerate(lines):
            if not API.search(line):
                continue
            if not documented_before(lines, index):
                missing.append(f"{header.as_posix()}:{index + 1}")
    return missing


def undocumented_bot_macros(bot_header: Path) -> list[str]:
    lines = bot_header.read_text(encoding="utf-8").splitlines()
    missing: list[str] = []
    for index, line in enumerate(lines):
        if not MACRO.match(line):
            continue
        if not documented_before(lines, index):
            missing.append(f"{bot_header.as_posix()}:{index + 1}")
    return missing


def forbidden_identifiers(headers: list[Path], banned: list[str]) -> list[str]:
    found: set[str] = set()
    for header in headers:
        identifiers = set(IDENTIFIER.findall(header.read_text(encoding="utf-8")))
        for entry in banned:
            if entry.endswith("*"):
                prefix = entry[:-1]
                if any(value.startswith(prefix) for value in identifiers):
                    found.add(entry)
            elif entry in identifiers:
                found.add(entry)
    return sorted(found)


def bot_macro_candidates(bot_header: Path, include_root: Path) -> set[str]:
    candidates: set[str] = set()
    pending = [bot_header]
    seen: set[Path] = set()
    while pending:
        header = pending.pop()
        if header in seen:
            continue
        seen.add(header)
        text = header.read_text(encoding="utf-8")
        candidates.update(match.group(1) for match in MACRO.finditer(text))
        for match in PUBLIC_INCLUDE.finditer(text):
            child = include_root / match.group(1)
            if child.is_file():
                pending.append(child)
    return candidates


def bot_macro_count(
    bot_header: Path, compiler: list[str], includes: list[Path], temp: Path
) -> tuple[int | None, str | None]:
    if not bot_header.is_file():
        return None, "<dcc/bot.h> is missing"
    candidates = bot_macro_candidates(bot_header, includes[1])
    probe = temp / "bot_macros.c"
    markers = "\n".join(
        f"#ifdef {name}\nDCC_V2_AUDIT_PRESENT_{name}\n#endif" for name in sorted(candidates)
    )
    probe.write_text(f"#include <dcc/bot.h>\n{markers}\n", encoding="utf-8")
    command = [*compiler, *(f"-I{item}" for item in includes), str(probe)]
    if is_msvc(compiler):
        command[1:1] = ["/nologo", "/EP"]
    else:
        command[1:1] = ["-E"]
    result = subprocess.run(
        command,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        detail = (result.stderr or result.stdout).strip().splitlines()
        return None, detail[-1] if detail else "failed to preprocess <dcc/bot.h>"
    names = set(re.findall(r"\bDCC_V2_AUDIT_PRESENT_(DCC_[A-Za-z0-9_]*)\b", result.stdout))
    return len(names), None


def summary(label: str, values: list[str], limit: int = 5) -> str:
    preview = ", ".join(values[:limit])
    suffix = "" if len(values) <= limit else f", ... (+{len(values) - limit} more)"
    return f"{label}: {len(values)} ({preview}{suffix})"


def main() -> int:
    args = parse_args()
    source = args.source.resolve()
    errors: list[str] = []
    debt: list[str] = []
    if not (source / "include/dcc").is_dir():
        errors.append(f"missing public include root: {source / 'include/dcc'}")
    try:
        c_compiler = command_parts(args.compiler)
        cxx_compiler = command_parts(args.cxx_compiler or default_cxx(c_compiler))
        banned = load_banned_names(source / "tools/api_v2_removed_names.txt")
        generated = version_defines(source)
    except ValueError as exc:
        errors.append(str(exc))
        c_compiler = []
        cxx_compiler = []
        banned = []
        generated = ""
    llam_include = find_llam_include(source)
    if llam_include is None:
        errors.append("LLAM headers were not found; set DCC_LLAM_ROOT")
    if errors:
        print("DCC v2 surface audit failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    headers = sorted((source / "include/dcc").rglob("*.h"))
    with tempfile.TemporaryDirectory(prefix="dcc-v2-surface-") as temp_dir:
        temp = Path(temp_dir)
        generated_include = temp / "generated/dcc"
        generated_include.mkdir(parents=True)
        (generated_include / "version_generated.h").write_text(generated, encoding="utf-8")
        includes = [temp / "generated", source / "include", llam_include]
        compile_errors, fragment_failures, fragment_successes = compile_headers(
            source, c_compiler, cxx_compiler, includes
        )
        errors.extend(compile_errors)
        if fragment_successes:
            errors.append(summary("stale declaration-fragment allowances", fragment_successes))
        if fragment_failures:
            target = debt if args.transition else errors
            target.append(summary("declaration-fragment standalone compile failures", fragment_failures))

        bot_header = source / "include/dcc/bot.h"
        macro_count, macro_error = bot_macro_count(bot_header, c_compiler, includes, temp)
        if macro_error is not None:
            (debt if args.transition else errors).append(macro_error)
        elif macro_count is not None and macro_count > MAX_BOT_MACROS:
            (debt if args.transition else errors).append(
                f"<dcc/bot.h> exports {macro_count} DCC_ macros; maximum is {MAX_BOT_MACROS}"
            )

        forbidden = forbidden_identifiers(headers, banned)
        if forbidden:
            (debt if args.transition else errors).append(summary("removed public identifiers", forbidden))
        fragments = sorted(
            header.relative_to(source / "include").as_posix()
            for header in headers
            if header.relative_to(source / "include").as_posix() in FRAGMENT_HEADERS
        )
        if fragments:
            (debt if args.transition else errors).append(summary("public declaration fragments", fragments))
        undocumented = undocumented_api_declarations(headers)
        if undocumented:
            (debt if args.transition else errors).append(summary("undocumented DCC_API declarations", undocumented))
        if bot_header.is_file():
            undocumented_macros = undocumented_bot_macros(bot_header)
            if undocumented_macros:
                (debt if args.transition else errors).append(
                    summary("undocumented Bot macros", undocumented_macros)
                )

    if debt:
        print("DCC v2 transition debt:")
        for item in debt:
            print(f"- {item}")
    if errors:
        print("DCC v2 surface audit failed:", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1
    mode = "transition" if args.transition else "strict"
    print(f"DCC v2 surface audit passed ({mode}): {len(headers)} headers checked")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
