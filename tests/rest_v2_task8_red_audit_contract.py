#!/usr/bin/env python3
"""Keep Task 8 RED while the registered Task 7 manifest gate stays green."""

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def run(audit: Path, source: Path, progress: int) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [
            sys.executable,
            str(audit),
            "--source",
            str(source),
            "--progress-through",
            str(progress),
        ],
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    audit = source / "tools/audit_rest_v2_endpoints.py"

    task7 = run(audit, source, 7)
    if task7.returncode != 0:
        print("Task 7 regressed during Task 8 RED setup:", file=sys.stderr)
        print((task7.stderr or task7.stdout)[:10000], file=sys.stderr)
        return 1

    task8 = run(audit, source, 8)
    if task8.returncode == 0:
        print(
            "Task 8 audit unexpectedly became GREEN; replace this RED wrapper "
            "with the registered --progress-through 8 gate.",
            file=sys.stderr,
        )
        return 1

    output = f"{task8.stdout}\n{task8.stderr}"
    required = (
        "dcc_rest_add_guild_member",
        "dcc_rest_get_guild",
        "dcc_rest_modify_guild_role_positions",
        "DCC_ENDPOINT_AUTH_POLICY_",
        "DCC_ENDPOINT_AUDIT_REASON_",
        "removal task",
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
            "Task 8 audit failed outside the reviewed old-implementation gap: "
            f"missing={missing} unexpected={unexpected}",
            file=sys.stderr,
        )
        print(output[:16000], file=sys.stderr)
        return 1

    print(
        "Task 8 RED audit confirmed: progress-through-7 passes at 224 active "
        "endpoints; progress-through-8 rejects the old 47 guild/member/role implementations"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
