#!/usr/bin/env python3
"""Conservative lexical assistant for migrating DCC 1 source to DCC 2."""

from __future__ import annotations

import argparse
import difflib
import os
from pathlib import Path
import re
import stat
import sys
import tempfile


EXIT_CLEAN, EXIT_SAFE_PENDING, EXIT_MANUAL, EXIT_IO = 0, 1, 2, 3
SAFE_OLD = "DCC_RESPOND_TEXT"
SAFE_NEW = "DCC_CTX_REPLY_TEXT"
SOURCE_SUFFIXES = {".c", ".h", ".cc", ".cpp", ".cxx", ".hpp"}
IDENTIFIER = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
SUGAR_INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]dcc/sugar\.h[>"]')


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--write", action="store_true", help="atomically apply safe edits")
    parser.add_argument("paths", nargs="+", type=Path)
    return parser.parse_args()


def line_column(text: str, offset: int) -> tuple[int, int]:
    line = text.count("\n", 0, offset) + 1
    return line, offset - text.rfind("\n", 0, offset)


def preprocessor_ranges(text: str) -> list[tuple[int, int]]:
    ranges: list[tuple[int, int]] = []
    offset = 0
    continued = False
    for line in text.splitlines(keepends=True):
        active = continued or line.lstrip(" \t").startswith("#")
        if active:
            ranges.append((offset, offset + len(line)))
        continued = active and line.rstrip("\r\n").endswith("\\")
        offset += len(line)
    if offset < len(text) and (continued or text[offset:].lstrip(" \t").startswith("#")):
        ranges.append((offset, len(text)))
    return ranges


def protected_spans(text: str) -> list[tuple[int, int]]:
    spans = preprocessor_ranges(text)
    i = 0
    while i < len(text):
        if text.startswith("//", i):
            end = text.find("\n", i + 2)
            end = len(text) if end < 0 else end
            spans.append((i, end))
            i = end
        elif text.startswith("/*", i):
            end = text.find("*/", i + 2)
            end = len(text) if end < 0 else end + 2
            spans.append((i, end))
            i = end
        elif text.startswith('R"', i):
            delimiter_end = text.find("(", i + 2)
            if delimiter_end < 0 or delimiter_end - i - 2 > 16:
                i += 2
                continue
            delimiter = text[i + 2:delimiter_end]
            marker = ")" + delimiter + '"'
            end = text.find(marker, delimiter_end + 1)
            end = len(text) if end < 0 else end + len(marker)
            spans.append((i, end))
            i = end
        elif text[i] in {'"', "'"}:
            quote = text[i]
            end = i + 1
            while end < len(text):
                if text[end] == "\\":
                    end += 2
                elif text[end] == quote:
                    end += 1
                    break
                else:
                    end += 1
            spans.append((i, min(end, len(text))))
            i = end
        else:
            i += 1
    return sorted(spans)


def is_protected(offset: int, spans: list[tuple[int, int]], cursor: list[int]) -> bool:
    while cursor[0] < len(spans) and spans[cursor[0]][1] <= offset:
        cursor[0] += 1
    return cursor[0] < len(spans) and spans[cursor[0]][0] <= offset < spans[cursor[0]][1]


def manual_reason(token: str) -> str | None:
    if token == "DCC_SIMPLE_BOT_MAIN":
        return "development/production policy decision required"
    if token == "DCC_V2_TEXT":
        return "storage-duration change requires manual conversion"
    if token.startswith(("DCC_ON_", "DCC_ROUTE_", "DCC_DECORATE_")):
        return "listener conversion required"
    if token.startswith("DCC_PRIVATE"):
        return "ephemeral-policy decision required"
    if token.startswith(("DCC_PUBLIC", "DCC_DONE")):
        return "response status propagation requires review"
    if token.startswith("DCC_RESPOND") and token != SAFE_OLD:
        return "response status propagation requires review"
    if token.startswith("DCC_V2_"):
        return "tagged DCC_UI value conversion required"
    if token.startswith(("DCC_COG", "DCC_FEATURE", "DCC_VIEW", "DCC_APP_DEFINITION")):
        return "legacy App composition requires manual conversion"
    return None


def preprocessor_findings(path: Path, text: str) -> list[str]:
    findings: list[str] = []
    offset = 0
    for source_line in text.splitlines(keepends=True):
        if SUGAR_INCLUDE.match(source_line):
            line, column = line_column(text, offset + source_line.find("dcc/sugar.h"))
            findings.append(f"{path}:{line}:{column}: manual <dcc/sugar.h>: include selection is semantic")
        macro = re.match(r"^\s*#\s*(define|undef)\s+DCC_RESPOND_TEXT\b", source_line)
        if macro:
            line, column = line_column(text, offset + source_line.find(SAFE_OLD))
            findings.append(f"{path}:{line}:{column}: manual {SAFE_OLD}: user-owned macro definition")
        conditional = re.match(r"^\s*#\s*(if|elif)\b", source_line)
        defined = re.search(r"\bdefined\s*\(\s*DCC_RESPOND_TEXT\s*\)", source_line)
        if conditional and defined:
            line, column = line_column(text, offset + source_line.find(SAFE_OLD, defined.start()))
            findings.append(f"{path}:{line}:{column}: manual {SAFE_OLD}: conditional macro use")
        offset += len(source_line)
    return findings


def migrate_text(path: Path, text: str) -> tuple[str, list[str], list[str]]:
    spans = protected_spans(text)
    cursor = [0]
    safe: list[str] = []
    manual = preprocessor_findings(path, text)
    pieces: list[str] = []
    last = 0
    for match in IDENTIFIER.finditer(text):
        if is_protected(match.start(), spans, cursor):
            continue
        token = match.group(0)
        line, column = line_column(text, match.start())
        if token == SAFE_OLD:
            pieces.extend((text[last:match.start()], SAFE_NEW))
            last = match.end()
            safe.append(f"{path}:{line}:{column}: safe {SAFE_OLD} -> {SAFE_NEW}")
        elif (reason := manual_reason(token)) is not None:
            manual.append(f"{path}:{line}:{column}: manual {token}: {reason}")
    pieces.append(text[last:])
    return "".join(pieces), safe, manual


def atomic_write(path: Path, data: bytes, mode: int) -> None:
    descriptor, temporary = tempfile.mkstemp(prefix=f".{path.name}.", dir=path.parent)
    try:
        with os.fdopen(descriptor, "wb") as stream:
            stream.write(data)
            stream.flush()
            os.fsync(stream.fileno())
        os.chmod(temporary, stat.S_IMODE(mode))
        os.replace(temporary, path)
    except BaseException:
        try:
            os.unlink(temporary)
        except FileNotFoundError:
            pass
        raise


def expand_paths(paths: list[Path]) -> list[Path]:
    files: list[Path] = []
    for path in paths:
        if path.is_dir():
            files.extend(item for item in sorted(path.rglob("*"))
                         if item.is_file() and item.suffix.lower() in SOURCE_SUFFIXES)
        else:
            files.append(path)
    return sorted(dict.fromkeys(files), key=lambda item: os.fsencode(str(item)))


def main() -> int:
    args = parse_args()
    safe_pending = False
    manual_found = False
    for path in expand_paths(args.paths):
        try:
            original = path.read_bytes().decode("utf-8")
            migrated, safe, manual = migrate_text(path, original)
        except (OSError, UnicodeError) as exc:
            print(f"{path}: I/O error: {exc}", file=sys.stderr)
            return EXIT_IO
        if safe:
            print(*safe, sep="\n", file=sys.stderr)
        if manual:
            print(*manual, sep="\n", file=sys.stderr)
        manual_found = manual_found or bool(manual)
        if migrated == original:
            continue
        if args.write:
            try:
                atomic_write(path, migrated.encode("utf-8"), path.stat().st_mode)
            except OSError as exc:
                print(f"{path}: I/O error: {exc}", file=sys.stderr)
                return EXIT_IO
        else:
            safe_pending = True
            print("".join(difflib.unified_diff(
                original.splitlines(keepends=True), migrated.splitlines(keepends=True),
                fromfile=str(path), tofile=str(path), lineterm="\n")), end="")
    return EXIT_MANUAL if manual_found else EXIT_SAFE_PENDING if safe_pending else EXIT_CLEAN


if __name__ == "__main__":
    raise SystemExit(main())
