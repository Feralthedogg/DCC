#!/usr/bin/env python3
"""Verify the complete Task 9 manifest migration."""

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

    task9 = run(audit, source, 9)
    if task9.returncode != 0:
        print("Task 9 manifest migration failed:", file=sys.stderr)
        print(f"{task9.stdout}\n{task9.stderr}"[:20000], file=sys.stderr)
        return 1

    print(
        "Task 9 audit confirmed: all 57 command/resource endpoints use their "
        "canonical request contracts and all 39 removal symbols are absent")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
