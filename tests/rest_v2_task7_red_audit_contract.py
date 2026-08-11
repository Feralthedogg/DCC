#!/usr/bin/env python3
"""Assert the Task 7 audit is RED while the registered Task 6 gate stays GREEN."""

from __future__ import annotations

import argparse
from pathlib import Path
import runpy
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

    task6 = run(audit, source, 6)
    if task6.returncode != 0:
        print("Task 6 registered audit regressed during Task 7 RED setup:", file=sys.stderr)
        print((task6.stderr or task6.stdout)[:8000], file=sys.stderr)
        return 1

    task7 = run(audit, source, 7)
    if task7.returncode == 0:
        print(
            "Task 7 audit unexpectedly became GREEN; replace this expected-RED wrapper "
            "with the registered --progress-through 7 gate.",
            file=sys.stderr,
        )
        return 1

    output = f"{task7.stdout}\n{task7.stderr}"
    required_markers = (
        "dcc_rest_add_thread_member",
        "dcc_rest_create_forum_thread",
        "dcc_rest_get_invite",
        "DCC_ENDPOINT_AUTH_POLICY_",
        "DCC_ENDPOINT_PATH_",
        "removal task",
    )
    missing = [marker for marker in required_markers if marker not in output]
    forbidden_markers = (
        "endpoint count invariant failed",
        "endpoint domain count invariant failed",
        "removed_candidates differs",
        "unclassified public",
        "manifest is malformed JSON",
    )
    unexpected = [marker for marker in forbidden_markers if marker in output]
    if missing or unexpected:
        print(
            "Task 7 audit did not fail for the reviewed old-implementation reasons; "
            f"missing={missing} unexpected={unexpected}",
            file=sys.stderr,
        )
        print(output[:12000], file=sys.stderr)
        return 1

    audit_namespace = runpy.run_path(str(audit))
    sensitive_errors = audit_namespace["sensitive_transport_contract_errors"](
        source
    )
    sensitive_markers = (
        "sensitive transport contract token(s) missing",
        "sensitive route fingerprint omits",
        "secure-zero primitive lacks optimization-resistant write evidence",
        "sensitive route fingerprint must use the exact domain and wipe temporaries",
        "async REST state must own distinct wire_path",
        "dcc_rest_submit must submit the fixed nonsecret operation identity",
        "rate-limit route keys must use the sensitive route fingerprint helper",
        "async request teardown does not use the secure-zero primitive",
        "prepared Authorization/token teardown does not use the secure-zero primitive",
    )
    missing_sensitive = [
        marker
        for marker in sensitive_markers
        if not any(marker in error for error in sensitive_errors)
    ]
    if missing_sensitive:
        print(
            "Task 7 sensitive transport audit did not expose every reviewed "
            f"old-implementation gap; missing={missing_sensitive}",
            file=sys.stderr,
        )
        print("\n".join(sensitive_errors), file=sys.stderr)
        return 1

    print(
        "Task 7 RED audit confirmed: progress-through-6 passes at 224 active endpoints; "
        "progress-through-7 rejects the old channel/thread/invite and shared capability state"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
