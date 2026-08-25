#!/usr/bin/env python3
"""Verify the reviewed final REST migration gate."""

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source", type=Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    audit = source / "tools/audit_rest_v2_endpoints.py"
    result = subprocess.run(
        [sys.executable, str(audit), "--source", str(source)],
        check=False, capture_output=True, text=True, encoding="utf-8",
        errors="replace",
    )
    if result.returncode != 0:
        print("Task 10 final REST migration is not complete:", file=sys.stderr)
        print(f"{result.stdout}\n{result.stderr}"[:20000], file=sys.stderr)
        return 1
    expected = (
        "224 endpoints; task 6=41, task 7=35, task 8=47, "
        "task 9=57, task 10=44)"
    )
    if expected not in result.stdout:
        print("Task 10 final endpoint arithmetic changed", file=sys.stderr)
        print(result.stdout, file=sys.stderr)
        return 1
    print("Task 10 audit confirmed: final 224/224 REST contract is green")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
