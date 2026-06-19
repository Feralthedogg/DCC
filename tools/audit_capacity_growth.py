#!/usr/bin/env python3
"""Audit capacity growth arithmetic for overflow guards.

Dynamic buffers and pointer arrays should check the doubling limit before
computing the doubled capacity. This audit catches the common fuzzing-sensitive
pattern where a capacity variable is multiplied by two without a nearby
SIZE_MAX / 2 guard.
"""

from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_ROOTS = (ROOT / "src",)
GUARD_RE = re.compile(r"(?:SIZE_MAX|\(size_t\)-1)\s*/\s*2U?")
CAPACITY_MUL_RE = re.compile(
    r"(?P<assign>\b(?:next|next_cap|next_bucket_count|current)\b\s*=|"
    r"\b(?:next|next_cap|bucket_count)\b\s*\*=|"
    r"\b\w*(?:cap|capacity|bucket_count)\w*\b\s*=)"
    r"[^;]*\*\s*2U?\b"
)


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def has_nearby_guard(lines: list[str], index: int) -> bool:
    start = max(0, index - 8)
    for line in lines[start:index]:
        if GUARD_RE.search(line):
            return True
    return False


def main() -> int:
    errors: list[str] = []
    for root in SOURCE_ROOTS:
        for path in sorted(root.rglob("*.[ch]")):
            text = path.read_text(encoding="utf-8", errors="replace")
            lines = text.splitlines()
            for index, line in enumerate(lines):
                stripped = line.strip()
                if "delay" in stripped:
                    continue
                if not CAPACITY_MUL_RE.search(stripped):
                    continue
                if not has_nearby_guard(lines, index):
                    errors.append(
                        f"{rel(path)}:{index + 1}: capacity doubling without nearby SIZE_MAX / 2 guard"
                    )

    if errors:
        print("DCC capacity growth audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("DCC capacity growth audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
