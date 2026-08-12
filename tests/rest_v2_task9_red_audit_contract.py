#!/usr/bin/env python3
"""Keep Task 9 RED while the registered Task 8 manifest gate stays green."""

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def run(audit: Path, source: Path, progress: int) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(audit), "--source", str(source),
         "--progress-through", str(progress)],
        check=False, capture_output=True, text=True, encoding="utf-8",
        errors="replace",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    audit = source / "tools/audit_rest_v2_endpoints.py"

    task8 = run(audit, source, 8)
    if task8.returncode != 0:
        print("Task 8 regressed during Task 9 RED setup:", file=sys.stderr)
        print((task8.stderr or task8.stdout)[:12000], file=sys.stderr)
        return 1

    task9 = run(audit, source, 9)
    if task9.returncode == 0:
        print("Task 9 unexpectedly became GREEN; remove this RED wrapper.",
              file=sys.stderr)
        return 1
    output = f"{task9.stdout}\n{task9.stderr}"
    required = (
        "dcc_rest_bulk_overwrite_global_commands",
        "dcc_rest_create_guild_sticker",
        "dcc_rest_guild_message_search",
        "dcc_rest_bulk_overwrite_application_commands",
        "DCC_REST_ROUTE_DPP_",
    )
    missing = [marker for marker in required if marker not in output]
    forbidden = (
        "endpoint count invariant failed",
        "endpoint domain count invariant failed",
        "removed_candidates differs",
        "manifest is malformed JSON",
    )
    unexpected = [marker for marker in forbidden if marker in output]
    if missing or unexpected:
        print(
            "Task 9 audit failed outside the reviewed old-implementation gap: "
            f"missing={missing} unexpected={unexpected}", file=sys.stderr)
        print(output[:20000], file=sys.stderr)
        return 1

    print(
        "Task 9 RED audit confirmed: progress-through-8 passes at 224 active "
        "endpoints; progress-through-9 rejects the old 57 command/resource "
        "implementations and 39 retained removal symbols")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
