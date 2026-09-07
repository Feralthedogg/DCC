#!/usr/bin/env python3
"""Collect opt-in JSON-lines microbenchmarks, retaining every raw sample."""
import argparse
import hashlib
import json
import math
import os
from pathlib import Path
import platform
import statistics
import subprocess
import sys
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[1]


def git(*args):
    try:
        return subprocess.check_output(
            ["git", "-C", str(ROOT), *args], text=True,
            stderr=subprocess.DEVNULL).strip()
    except (OSError, subprocess.CalledProcessError):
        return None


def validate(sample):
    for key in ("fixture", "stage"):
        if not isinstance(sample.get(key), str) or not sample[key]:
            raise ValueError(f"invalid {key}")
    if type(sample.get("iterations")) is not int or sample["iterations"] <= 0:
        raise ValueError("iterations must be a positive integer")
    for key in ("cpu_seconds", "elapsed_seconds"):
        value = sample.get(key)
        if type(value) not in (int, float) or not math.isfinite(value) or value < 0:
            raise ValueError(f"invalid {key}")
    for key in ("p95_elapsed_ns", "p99_elapsed_ns"):
        if key in sample:
            value = sample[key]
            if type(value) not in (int, float) or not math.isfinite(value) or value < 0:
                raise ValueError(f"invalid {key}")
    allocation = sample.get("allocations")
    if allocation is not None:
        if (not isinstance(allocation, dict) or
                allocation.get("kind") not in ("measured", "analytical") or
                not isinstance(allocation.get("scope"), str) or
                type(allocation.get("count")) is not int or allocation["count"] < 0):
            raise ValueError("allocations require kind, scope and nonnegative count")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repeat", type=int, default=5)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--build-dir", type=Path,
                        help="CMake build directory; records compiler/options cache")
    parser.add_argument("--library", type=Path, help="Linked DCC library; records its SHA256")
    parser.add_argument("--library-commit", help="Source revision of linked DCC library")
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    if args.repeat <= 0:
        parser.error("repeat must be positive")
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        parser.error("a benchmark command is required after --")
    runs, keys, groups = [], None, {}
    for index in range(args.repeat):
        completed = subprocess.run(command, capture_output=True, text=True, check=True)
        samples = [json.loads(line) for line in completed.stdout.splitlines() if line.strip()]
        if not samples:
            raise ValueError("benchmark produced no samples")
        for sample in samples:
            validate(sample)
        current = [(s["fixture"], s["stage"], s["iterations"]) for s in samples]
        if len(set(current)) != len(current) or (keys is not None and current != keys):
            raise ValueError("fixture/stage/iterations changed or duplicated between runs")
        keys = current
        runs.append({"index": index, "samples": samples, "stderr": completed.stderr})
        for key, sample in zip(current, samples):
            groups.setdefault(key, []).append(sample)
    summary = []
    for (fixture, stage, iterations), samples in groups.items():
        allocations = samples[0].get("allocations")
        if any(s.get("allocations") != allocations for s in samples):
            raise ValueError("allocations changed between runs")
        item = {"fixture": fixture, "stage": stage,
                        "median_cpu_ns_per_op": statistics.median(
                            s["cpu_seconds"] * 1e9 / iterations for s in samples),
                        "median_elapsed_ns_per_op": statistics.median(
                            s["elapsed_seconds"] * 1e9 / iterations for s in samples),
                        "allocations": allocations}
        for key in ("p95_elapsed_ns", "p99_elapsed_ns"):
            if any(key in s for s in samples):
                if not all(key in s for s in samples):
                    raise ValueError(f"missing {key} in some runs")
                item["median_" + key] = statistics.median(s[key] for s in samples)
        summary.append(item)
    fixture_sources = sorted((ROOT / "tests").glob("*bench*.c"))
    fixture_sources.extend(sorted((ROOT / "tests/support").glob("benchmark*.h")))
    metadata = {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in fixture_sources}
    cache = args.build_dir / "CMakeCache.txt" if args.build_dir else None
    build = {}
    if cache:
        for line in cache.read_text().splitlines():
            if line.startswith(("CMAKE_C_COMPILER:", "CMAKE_BUILD_TYPE:", "CMAKE_C_FLAGS",
                                "DCC_", "CMAKE_OSX_ARCHITECTURES:")) and "=" in line:
                key, value = line.split("=", 1)
                build[key] = value
    toplevel = git("rev-parse", "--show-toplevel")
    own_checkout = toplevel is not None and Path(toplevel).resolve() == ROOT
    commit = git("rev-parse", "HEAD") if own_checkout else None
    dirty = git("status", "--porcelain") if own_checkout else None
    report = {"schema_version": 1, "created_utc": datetime.now(timezone.utc).isoformat(),
              "source": {"commit": commit,
                         "dirty": None if dirty is None else bool(dirty),
                         "fixture_sha256": metadata},
              "environment": {"platform": platform.platform(), "machine": platform.machine(),
                              "processor": platform.processor(), "python": platform.python_version(),
                              "cpu_count": os.cpu_count(), "build": build},
              "command": command, "runs": runs, "summary": summary}
    if args.library:
        report["library"] = {"path": str(args.library.resolve()),
                             "sha256": hashlib.sha256(args.library.read_bytes()).hexdigest(),
                             "commit": args.library_commit}
    encoded = json.dumps(report, indent=2, allow_nan=False) + "\n"
    if args.output:
        args.output.write_text(encoded)
    else:
        print(encoded, end="")


if __name__ == "__main__":
    try:
        main()
    except (ValueError, OSError, subprocess.SubprocessError) as error:
        print(f"benchmark error: {error}", file=sys.stderr)
        sys.exit(1)
