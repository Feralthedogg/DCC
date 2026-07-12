#!/usr/bin/env python3
"""Fail when CI executes a third-party action or LLAM checkout through a mutable ref."""

from __future__ import annotations

import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SHA = r"[0-9a-f]{40}"
LLAM_220_COMMIT = "2808e99c0339b8b3d6ed5ef7fab3dc231927c568"


def main() -> int:
    errors: list[str] = []
    for path in sorted((ROOT / ".github" / "workflows").glob("*.yml")):
        text = path.read_text(encoding="utf-8")
        for line_no, line in enumerate(text.splitlines(), 1):
            match = re.search(r"\buses:\s*([^\s#]+)", line)
            if match and not re.fullmatch(rf"[^@]+@{SHA}", match.group(1)):
                errors.append(f"{path.relative_to(ROOT)}:{line_no}: mutable action ref {match.group(1)}")
        blocks = text.split("- name: Checkout LLAM")
        for block_index, block in enumerate(blocks[1:], 1):
            checkout_block = block.split("- name:", 1)[0]
            if f"ref: {LLAM_220_COMMIT}" not in checkout_block:
                errors.append(
                    f"{path.relative_to(ROOT)}: Checkout LLAM block {block_index} is not pinned to "
                    f"the v2.2.0 commit"
                )
    if errors:
        print("DCC workflow pin audit failed:")
        for error in errors:
            print(f" - {error}")
        return 1
    print("DCC workflow pin audit passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
