#!/usr/bin/env python3
"""Compile and enforce the DCC 2 public-header surface contract."""

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
COMMAND_TIMEOUT_SECONDS = 30
TRANSITION_FRAGMENT_ALLOWANCES = frozenset()
REQUIRED_BANNED_PATTERNS = frozenset({
    "DCC_ON_*", "DCC_ROUTE_*", "DCC_DECORATE_*", "DCC_RESPOND*",
    "DCC_SIMPLE_BOT*", "DCC_V2_*", "dcc_rest_future_t",
})
APP_API_ALLOWLIST = frozenset({
    "dcc_app_create", "dcc_app_destroy", "dcc_app_start", "dcc_app_stop",
    "dcc_app_wait", "dcc_app_run", "dcc_app_run_with_signals",
    "dcc_app_client", "dcc_app_store",
    "dcc_app_store_open_file", "dcc_app_store_close", "dcc_app_options_init",
    "dcc_app_command_sync_options_init", "dcc_app_on_error",
    "dcc_app_use_default_error_responses", "dcc_app_listen", "dcc_app_unlisten",
    "dcc_app_module_register", "dcc_app_module_count",
})
APP_API_ALLOWED_PREFIXES = ("dcc_app_env_",)
IDENTIFIER = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
MACRO_DUMP = re.compile(r"^\s*#\s*define\s+(DCC_[A-Za-z0-9_]*)\b", re.MULTILINE)
API = re.compile(r"\bDCC_API\b")
APP_API = re.compile(r"\bDCC_API\b[\s\S]*?\b(dcc_app_[A-Za-z0-9_]+)\s*\(")
SENTINEL = "DCC_V2_AUDIT_MACRO_SENTINEL"
BOT_C_ARTIFACT = "bot_v2_macros_posix_clang_c_static.txt"
BOT_CPP_ARTIFACT = "bot_v2_macros_posix_clang_cpp_static.txt"
BOT_DEPENDENCY_ARTIFACT = "bot_v2_dependency_macros_posix_clang_c_static.txt"
BOT_UI_VARIADIC = frozenset({
    "DCC_UI_ROW", "DCC_UI_SECTION", "DCC_UI_CARD", "DCC_UI_CARD_ACCENT",
    "DCC_UI_GALLERY", "DCC_UI_STRING_SELECT", "DCC_UI_RADIO_GROUP",
    "DCC_UI_CHECKBOX_GROUP", "DCC_UI_MODAL",
})
BOT_OWNED_C = frozenset({
    "DCC_BOT_H", "DCC_BOT_HANDLERS_H", "DCC_BOT_LISTENERS_H",
    "DCC_BOT_REPLIES_H", "DCC_BOT_UI_H", "DCC_BOT_MAIN_H",
    "DCC_SLASH_FN", "DCC_AUTOCOMPLETE_FN", "DCC_BUTTON_FN",
    "DCC_SELECT_FN", "DCC_MODAL_FN", "DCC_EVENT_FN", "DCC_READY_FN",
    "DCC_MESSAGE_FN", "DCC_TASK_FN", "DCC_LISTENER_CONFIG_INIT",
    "DCC_BOT_CONFIG_INIT", "DCC_DEV_BOT_MAIN", "DCC_BOT_MAIN",
    "DCC_DEV_BOT_MAIN_WITH", "DCC_BOT_MAIN_WITH",
}) | BOT_UI_VARIADIC


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--compiler", required=True, help="configured C compiler")
    parser.add_argument("--cxx-compiler", required=True, help="configured C++ compiler")
    parser.add_argument("--llam-include", type=Path, help="directory containing llam/runtime.h")
    parser.add_argument("--removed-names", type=Path, help="removed-name configuration")
    parser.add_argument("--transition", action="store_true")
    return parser.parse_args()


def command_parts(command: str, label: str) -> list[str]:
    parts = shlex.split(command, posix=os.name != "nt")
    if not parts:
        raise ValueError(f"{label} compiler is not executable: {command}")
    executable = Path(parts[0])
    if not executable.is_file() and shutil.which(parts[0]) is None:
        raise ValueError(f"{label} compiler is not executable: {command}")
    return parts


def is_msvc(command: list[str]) -> bool:
    return Path(command[0]).name.lower() in {
        "cl", "cl.exe", "clang-cl", "clang-cl.exe",
    }


def first_diagnostic(result: subprocess.CompletedProcess[str]) -> str:
    lines = (result.stderr or result.stdout).strip().splitlines()
    if not lines:
        return "compiler failed without a diagnostic"
    useful = [line.strip() for line in lines if line.strip() and not line.lstrip().startswith("Note:")]
    return " | ".join(useful[:3])


def run(command: list[str]) -> subprocess.CompletedProcess[str] | str:
    try:
        return subprocess.run(command, capture_output=True, text=True, timeout=COMMAND_TIMEOUT_SECONDS)
    except subprocess.TimeoutExpired:
        return f"compiler timed out after {COMMAND_TIMEOUT_SECONDS}s"
    except OSError as exc:
        return f"compiler execution failed: {exc}"


def find_llam_include(source: Path) -> Path | None:
    candidates: list[Path] = []
    if root := os.environ.get("DCC_LLAM_ROOT"):
        candidates.append(Path(root) / "include")
    for ancestor in (source, *source.parents):
        candidates.append(ancestor / "LLAM" / "include")
    return next((item for item in candidates if (item / "llam/runtime.h").is_file()), None)


def version_defines(source: Path) -> str:
    cmake = (source / "CMakeLists.txt").read_text(encoding="utf-8")
    match = re.search(r"project\(dcc\s+VERSION\s+([0-9]+)\.([0-9]+)\.([0-9]+)", cmake)
    if match is None:
        raise ValueError("CMake project version is missing or malformed")
    major, minor, patch = match.groups()
    return (
        "#ifndef DCC_VERSION_GENERATED_H\n#define DCC_VERSION_GENERATED_H\n"
        f"#define DCC_VERSION_MAJOR {major}\n#define DCC_VERSION_MINOR {minor}\n"
        f"#define DCC_VERSION_PATCH {patch}\n#define DCC_VERSION_STRING \"{major}.{minor}.{patch}\"\n#endif\n"
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
    if not entries or len(entries) != len(set(entries)):
        raise ValueError(f"removed-name configuration is empty or duplicated: {path}")
    missing = sorted(REQUIRED_BANNED_PATTERNS - set(entries))
    if missing:
        raise ValueError("removed-name configuration omits required patterns: " + ", ".join(missing))
    return entries


def compile_command(compiler: list[str], language: str, includes: list[Path], unit: Path) -> list[str]:
    if is_msvc(compiler):
        standard = "/std:c11" if language == "c" else "/std:c++17"
        return [*compiler, "/nologo", standard, "/Zs", *(f"/I{item}" for item in includes), str(unit)]
    standard = "-std=c11" if language == "c" else "-std=c++17"
    return [*compiler, standard, "-fsyntax-only", *(f"-I{item}" for item in includes), str(unit)]


def is_fragment(header: Path) -> bool:
    text = header.read_text(encoding="utf-8")
    return "_decls" in header.name or (
        text.lstrip().startswith("/* Included by <dcc/") and "DCC_API" in text
    )


def is_installed_header(source: Path, header: Path) -> bool:
    relative = header.relative_to(source / "include").as_posix()
    return (relative != "dcc/sugar.h" and
            not relative.startswith("dcc/sugar/") and
            relative != "dcc/app/legacy.h" and
            relative != "dcc/rest/core.h" and
            not relative.startswith("dcc/rest/core/") and
            relative != "dcc/rest/response_helpers.h")


def compile_headers(source: Path, c: list[str], cxx: list[str], includes: list[Path]) -> tuple[list[str], list[str], list[str]]:
    errors: list[str] = []
    allowed_failures: list[str] = []
    stale_allowances: list[str] = []
    with tempfile.TemporaryDirectory(prefix="dcc-v2-header-audit-") as directory:
        temp = Path(directory)
        for header in sorted((source / "include/dcc").rglob("*.h")):
            if not is_installed_header(source, header):
                continue
            relative = header.relative_to(source / "include").as_posix()
            fragment = is_fragment(header)
            for language, compiler, suffix in (("c", c, "c"), ("c++", cxx, "cpp")):
                unit = temp / f"header.{suffix}"
                unit.write_text(f"#include <{relative}>\nint main(void) {{ return 0; }}\n", encoding="utf-8")
                result = run(compile_command(compiler, language, includes, unit))
                label = f"{relative} ({language})"
                if isinstance(result, str):
                    errors.append(f"{label}: {result}")
                elif result.returncode == 0:
                    if relative in TRANSITION_FRAGMENT_ALLOWANCES:
                        stale_allowances.append(label)
                else:
                    detail = first_diagnostic(result)
                    if relative in TRANSITION_FRAGMENT_ALLOWANCES:
                        allowed_failures.append(f"{label}: {detail}")
                    else:
                        errors.append(f"{label}: {detail}")
            if fragment and relative not in TRANSITION_FRAGMENT_ALLOWANCES:
                errors.append(f"unapproved public declaration fragment: {relative}")
    return errors, allowed_failures, stale_allowances


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
            if API.search(line) and not documented_before(lines, index):
                missing.append(f"{header.relative_to(header.parents[2]).as_posix()}:{index + 1}")
    return missing


def undocumented_bot_macros(bot_headers: list[Path]) -> list[str]:
    missing: list[str] = []
    guards = {
        "DCC_BOT_H", "DCC_BOT_HANDLERS_H", "DCC_BOT_LISTENERS_H",
        "DCC_BOT_REPLIES_H", "DCC_BOT_UI_H", "DCC_BOT_MAIN_H",
    }
    for bot_header in bot_headers:
        lines = bot_header.read_text(encoding="utf-8").splitlines()
        for index, line in enumerate(lines):
            match = MACRO_DUMP.match(line)
            if match and match.group(1) not in guards and not documented_before(lines, index):
                missing.append(f"{bot_header.name}:{index + 1}")
    return missing


def bot_topology_errors(source: Path) -> list[str]:
    errors: list[str] = []
    umbrella = source / "include/dcc/bot.h"
    leaves = ["handlers.h", "listeners.h", "replies.h", "ui.h", "main.h"]
    expected = [f"dcc/bot/{leaf}" for leaf in leaves]
    include_pattern = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.MULTILINE)
    if umbrella.is_file():
        actual = include_pattern.findall(umbrella.read_text(encoding="utf-8"))
        if actual != expected:
            errors.append(f"<dcc/bot.h> include topology differs: {actual}")
    forbidden_prefixes = ("dcc/app.h", "dcc/dcc.h", "dcc/sugar.h", "dcc/sugar/", "dcc/app/legacy.h")
    for leaf in leaves:
        path = source / "include/dcc/bot" / leaf
        if not path.is_file():
            errors.append(f"missing Bot leaf: {leaf}")
            continue
        for dependency in include_pattern.findall(path.read_text(encoding="utf-8")):
            if dependency in forbidden_prefixes or dependency.startswith("dcc/sugar/"):
                errors.append(f"forbidden Bot include edge: {leaf} -> {dependency}")
    return errors


def forbidden_identifiers(headers: list[Path], banned: list[str]) -> list[str]:
    found: set[str] = set()
    for header in headers:
        identifiers = set(IDENTIFIER.findall(header.read_text(encoding="utf-8")))
        for entry in banned:
            if entry.endswith("*") and any(value.startswith(entry[:-1]) for value in identifiers):
                found.add(entry)
            elif not entry.endswith("*") and entry in identifiers:
                found.add(entry)
    return sorted(found)


def noncanonical_app_api(headers: list[Path]) -> list[str]:
    names: set[str] = set()
    for header in headers:
        names.update(APP_API.findall(header.read_text(encoding="utf-8")))
    return sorted(name for name in names if name not in APP_API_ALLOWLIST and not name.startswith(APP_API_ALLOWED_PREFIXES))


def bot_macro_count(bot_header: Path, compiler: list[str], includes: list[Path], temp: Path) -> tuple[int | None, str | None]:
    if not bot_header.is_file():
        return None, "<dcc/bot.h> is missing"
    probe = temp / "bot_macros.c"
    probe.write_text("#include <dcc/bot.h>\n", encoding="utf-8")
    if is_msvc(compiler):
        command = [*compiler, "/nologo", "/EP", "/d1PP", f"/D{SENTINEL}=1", *(f"/I{item}" for item in includes), str(probe)]
    else:
        command = [*compiler, "-dM", "-E", f"-D{SENTINEL}=1", *(f"-I{item}" for item in includes), str(probe)]
    result = run(command)
    if isinstance(result, str):
        return None, result
    if result.returncode != 0:
        return None, first_diagnostic(result)
    names = set(MACRO_DUMP.findall(result.stdout))
    if SENTINEL not in names:
        return None, "compiler did not provide a complete DCC_ macro dump (sentinel missing)"
    names.remove(SENTINEL)
    return len(names), None


def normalize_macro_dump(output: str) -> tuple[str, set[str]]:
    records: list[str] = []
    names: set[str] = set()
    pattern = re.compile(r"^#define\s+(DCC_[A-Za-z0-9_]*)(\(([^)]*)\))?\s*(.*)$")
    for line in output.splitlines():
        match = pattern.match(line.strip())
        if match is None:
            continue
        name, invocation, parameters, expansion = match.groups()
        names.add(name)
        expansion = " ".join(expansion.split())
        if not expansion:
            expansion = "<empty>"
        if invocation is None:
            kind, arity = "object", "-"
        else:
            kind = "function"
            args = [] if parameters == "" else [item.strip() for item in parameters.split(",")]
            arity = str(len(args)) + ("+" if args and args[-1] == "..." else "")
        records.append(f"{name}\t{kind}\t{arity}\t{expansion}")
    return "\n".join(sorted(records, key=lambda item: item.encode("utf-8"))) + "\n", names


def full_macro_dump(
    compiler: list[str], language: str, includes: list[Path], unit: Path
) -> tuple[str | None, set[str], str | None]:
    if is_msvc(compiler):
        command = [*compiler, "/nologo", "/EP", "/d1PP", f"/D{SENTINEL}=1",
                   *(f"/I{item}" for item in includes), str(unit)]
    else:
        standard = "-std=c11" if language == "c" else "-std=c++17"
        command = [*compiler, standard, "-dM", "-E", f"-D{SENTINEL}=1",
                   *(f"-I{item}" for item in includes), str(unit)]
    result = run(command)
    if isinstance(result, str):
        return None, set(), result
    if result.returncode != 0:
        return None, set(), first_diagnostic(result)
    normalized, names = normalize_macro_dump(result.stdout)
    if SENTINEL not in names:
        return None, names, "compiler did not provide a complete DCC_ macro dump (sentinel missing)"
    sentinel_prefix = f"{SENTINEL}\t"
    normalized = "\n".join(
        line for line in normalized.splitlines() if not line.startswith(sentinel_prefix)
    ) + "\n"
    names.remove(SENTINEL)
    return normalized, names, None


def compare_macro_artifact(path: Path, actual: str) -> str | None:
    if not path.is_file():
        return f"missing macro artifact: {path.name}"
    expected = path.read_text(encoding="utf-8")
    return None if expected == actual else f"macro artifact changed: {path.name}"


def summary(label: str, values: list[str], limit: int = 5) -> str:
    preview = ", ".join(values[:limit])
    suffix = "" if len(values) <= limit else f", ... (+{len(values) - limit} more)"
    return f"{label}: {len(values)} ({preview}{suffix})"


def main() -> int:
    args = parse_args()
    source = args.source.resolve()
    errors: list[str] = []
    debt: list[str] = []
    try:
        if not (source / "include/dcc").is_dir():
            raise ValueError(f"missing public include root: {source / 'include/dcc'}")
        c_compiler = command_parts(args.compiler, "C")
        cxx_compiler = command_parts(args.cxx_compiler, "C++")
        banned = load_banned_names((args.removed_names or source / "tools/api_v2_removed_names.txt").resolve())
        generated = version_defines(source)
        llam_include = (args.llam_include.resolve() if args.llam_include else find_llam_include(source))
        if llam_include is None or not (llam_include / "llam/runtime.h").is_file():
            raise ValueError("LLAM include is missing llam/runtime.h")
    except ValueError as exc:
        errors.append(str(exc))
        c_compiler, cxx_compiler, banned, generated, llam_include = [], [], [], "", Path(".")
    if errors:
        print("DCC v2 surface audit failed:", file=sys.stderr)
        print("- " + "\n- ".join(errors), file=sys.stderr)
        return 1

    headers = [header for header in sorted((source / "include/dcc").rglob("*.h"))
               if is_installed_header(source, header)]
    with tempfile.TemporaryDirectory(prefix="dcc-v2-surface-") as directory:
        temp = Path(directory)
        generated_dir = temp / "generated/dcc"
        generated_dir.mkdir(parents=True)
        (generated_dir / "version_generated.h").write_text(generated, encoding="utf-8")
        includes = [temp / "generated", source / "include", llam_include]
        compile_errors, fragment_failures, stale_allowances = compile_headers(source, c_compiler, cxx_compiler, includes)
        errors.extend(compile_errors)
        if stale_allowances:
            errors.append(summary("stale declaration-fragment allowances", stale_allowances))
        if fragment_failures:
            (debt if args.transition else errors).append(summary("declaration-fragment standalone compile failures", fragment_failures))
        bot_header = source / "include/dcc/bot.h"
        errors.extend(bot_topology_errors(source))
        macro_count, macro_error = bot_macro_count(bot_header, c_compiler, includes, temp)
        if macro_error:
            (debt if args.transition else errors).append(macro_error)
        elif macro_count is not None and macro_count > MAX_BOT_MACROS:
            (debt if args.transition else errors).append(f"<dcc/bot.h> exports {macro_count} DCC_ macros; maximum is {MAX_BOT_MACROS}")
        if bot_header.is_file():
            dependency_unit = temp / "bot_dependencies.c"
            dependency_unit.write_text(
                "#include <dcc/app/base.h>\n"
                "#include <dcc/app/listeners.h>\n"
                "#include <dcc/app/context.h>\n"
                "#include <dcc/message.h>\n"
                "#include <dcc/modal.h>\n"
                "#include <dcc/autocomplete.h>\n"
                "#include <dcc/component_v2.h>\n"
                "#include <dcc/app/options.h>\n"
                "#include <dcc/app/lifecycle.h>\n"
                "#include <dcc/app/env.h>\n",
                encoding="utf-8",
            )
            bot_c_unit = temp / "bot.c"
            bot_cpp_unit = temp / "bot.cpp"
            bot_c_unit.write_text("#include <dcc/bot.h>\n", encoding="utf-8")
            bot_cpp_unit.write_text("#include <dcc/bot.h>\n", encoding="utf-8")
            dependency_dump, dependency_names, dependency_error = full_macro_dump(
                c_compiler, "c", includes, dependency_unit)
            bot_c_dump, bot_c_names, bot_c_error = full_macro_dump(
                c_compiler, "c", includes, bot_c_unit)
            bot_cpp_dump, bot_cpp_names, bot_cpp_error = full_macro_dump(
                cxx_compiler, "c++", includes, bot_cpp_unit)
            for label, detail in (
                ("Bot dependency macro dump", dependency_error),
                ("Bot C macro dump", bot_c_error),
                ("Bot C++ macro dump", bot_cpp_error),
            ):
                if detail is not None:
                    errors.append(f"{label}: {detail}")
            if dependency_dump is not None and bot_c_dump is not None:
                owned = bot_c_names - dependency_names
                if owned != BOT_OWNED_C:
                    errors.append(summary("unexpected Bot-owned C macros",
                                          sorted(owned ^ BOT_OWNED_C)))
                if len(bot_c_names) != len(dependency_names) + 30:
                    errors.append(
                        f"Bot C macro arithmetic is {len(dependency_names)} + "
                        f"{len(bot_c_names) - len(dependency_names)}, expected +30")
                if len(bot_c_names) > MAX_BOT_MACROS:
                    errors.append(
                        f"Bot C macro total is {len(bot_c_names)}, maximum is {MAX_BOT_MACROS}")
                artifact_error = compare_macro_artifact(
                    source / "tools" / BOT_DEPENDENCY_ARTIFACT,
                    dependency_dump)
                if artifact_error is not None:
                    errors.append(artifact_error)
                artifact_error = compare_macro_artifact(
                    source / "tools" / BOT_C_ARTIFACT, bot_c_dump)
                if artifact_error is not None:
                    errors.append(artifact_error)
            if bot_cpp_dump is not None:
                if bot_cpp_names & BOT_UI_VARIADIC:
                    errors.append(summary("C-only Bot UI macros visible in C++",
                                          sorted(bot_cpp_names & BOT_UI_VARIADIC)))
                artifact_error = compare_macro_artifact(
                    source / "tools" / BOT_CPP_ARTIFACT, bot_cpp_dump)
                if artifact_error is not None:
                    errors.append(artifact_error)
        forbidden = forbidden_identifiers(headers, banned)
        if forbidden:
            (debt if args.transition else errors).append(summary("removed public identifiers", forbidden))
        fragments = [header.relative_to(source / "include").as_posix() for header in headers if is_fragment(header)]
        if fragments:
            known = [item for item in fragments if item in TRANSITION_FRAGMENT_ALLOWANCES]
            unknown = [item for item in fragments if item not in TRANSITION_FRAGMENT_ALLOWANCES]
            if known:
                (debt if args.transition else errors).append(summary("public declaration fragments", known))
            if unknown:
                errors.append(summary("unapproved public declaration fragments", unknown))
        noncanonical = noncanonical_app_api(headers)
        if noncanonical:
            (debt if args.transition else errors).append(summary("non-canonical App DCC_API declarations", noncanonical))
        undocumented = undocumented_api_declarations(headers)
        if undocumented:
            debt.append(summary("documentation backlog", undocumented))
        if bot_header.is_file():
            bot_headers = [bot_header, *sorted((source / "include/dcc/bot").glob("*.h"))]
            undocumented_macros = undocumented_bot_macros(bot_headers)
            if undocumented_macros:
                (debt if args.transition else errors).append(summary("undocumented Bot macros", undocumented_macros))
    if debt:
        print("DCC v2 transition debt:")
        print("- " + "\n- ".join(debt))
    if errors:
        print("DCC v2 surface audit failed:", file=sys.stderr)
        print("- " + "\n- ".join(errors), file=sys.stderr)
        return 1
    print(f"DCC v2 surface audit passed ({'transition' if args.transition else 'strict'}): {len(headers)} headers checked")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
