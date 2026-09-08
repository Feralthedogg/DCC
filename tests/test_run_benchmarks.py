"""Runner regressions: dropped samples, invented allocation data, bad input."""
import json
import hashlib
import os
from pathlib import Path
import subprocess
import shutil
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
TOOL = ROOT / "tools/run_benchmarks.py"


class BenchmarkRunnerTests(unittest.TestCase):
    def test_source_archive_without_git_metadata_still_reports_samples(self):
        self.check_source_archive(no_git=False)

    def test_source_archive_without_git_executable_still_reports_samples(self):
        self.check_source_archive(no_git=True)

    @unittest.skipUnless(shutil.which("git"), "requires git to create the enclosing checkout")
    def test_source_archive_does_not_claim_enclosing_repository_metadata(self):
        self.check_source_archive(no_git=False, enclosing_git=True)

    def check_source_archive(self, no_git, enclosing_git=False):
        record = {"fixture": "archive", "stage": "parse", "iterations": 2,
                  "cpu_seconds": 0.01, "elapsed_seconds": 0.02}
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory)
            if enclosing_git:
                subprocess.run(["git", "init", "-q", str(source)], check=True)
                subprocess.run(["git", "-C", str(source), "-c", "user.name=Test",
                                "-c", "user.email=test@example.invalid", "commit",
                                "--allow-empty", "-qm", "enclosing repository"], check=True)
                source = source / "archive"
                source.mkdir()
            (source / "tools").mkdir()
            copied = source / "tools/run_benchmarks.py"
            shutil.copy2(TOOL, copied)
            env = os.environ.copy()
            if no_git:
                env["PATH"] = str(source / "empty-path")
            result = subprocess.run(
                [sys.executable, str(copied), "--repeat", "1", "--",
                 sys.executable, "-c", "print(" + repr(json.dumps(record)) + ")"],
                cwd=source, env=env, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            report = json.loads(result.stdout)
            self.assertEqual(report["runs"][0]["samples"], [record])
            self.assertIsNone(report["source"]["commit"])
            self.assertIsNone(report["source"]["dirty"])

    @unittest.skipUnless(shutil.which("git"), "requires git metadata")
    def test_checkout_reports_real_commit_and_dirty_state(self):
        revision = subprocess.run(["git", "-C", str(ROOT), "rev-parse", "HEAD"],
                                  capture_output=True, text=True)
        top = subprocess.run(["git", "-C", str(ROOT), "rev-parse", "--show-toplevel"],
                             capture_output=True, text=True)
        if revision.returncode or top.returncode or Path(top.stdout.strip()).resolve() != ROOT:
            self.skipTest("source tree is not its own Git checkout")
        dirty = subprocess.check_output(["git", "-C", str(ROOT), "status", "--porcelain"], text=True)
        record = {"fixture": "checkout", "stage": "parse", "iterations": 2,
                  "cpu_seconds": 0.01, "elapsed_seconds": 0.02}
        result = self.run_fixture(json.dumps(record))
        self.assertEqual(result.returncode, 0, result.stderr)
        source = json.loads(result.stdout)["source"]
        self.assertEqual(source["commit"], revision.stdout.strip())
        self.assertEqual(source["dirty"], bool(dirty.strip()))

    def run_fixture(self, output, *args):
        with tempfile.TemporaryDirectory() as directory:
            fixture = Path(directory) / "fixture.py"
            fixture.write_text("print(" + repr(output) + ")\n")
            return subprocess.run(
                [sys.executable, str(TOOL), "--repeat", "3", *args,
                 "--", sys.executable, str(fixture)],
                text=True, capture_output=True, check=False)

    def test_repeated_samples_keep_measurements_and_unknown_allocations(self):
        record = {"fixture": "request_small", "stage": "construct_free",
                  "iterations": 10, "cpu_seconds": 0.01,
                  "elapsed_seconds": 0.02, "allocations": None}
        result = self.run_fixture(json.dumps(record))
        self.assertEqual(result.returncode, 0, result.stderr)
        report = json.loads(result.stdout)
        self.assertEqual(report["schema_version"], 1)
        self.assertEqual(len(report["runs"]), 3)
        self.assertEqual(report["runs"][2]["samples"], [record])
        self.assertIn("commit", report["source"])
        clock = ROOT / "tests/support/benchmark_clock.h"
        self.assertEqual(report["source"]["fixture_sha256"].get(
            "tests/support/benchmark_clock.h"), hashlib.sha256(clock.read_bytes()).hexdigest())
        self.assertIn("platform", report["environment"])
        self.assertEqual(report["summary"][0]["median_cpu_ns_per_op"], 1000000)
        self.assertIsNone(report["summary"][0]["allocations"])

    def test_rejects_nonpositive_iterations(self):
        record = {"fixture": "bad", "stage": "parse", "iterations": 0,
                  "cpu_seconds": 0.1, "elapsed_seconds": 0.1, "allocations": None}
        result = self.run_fixture(json.dumps(record))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("iterations", result.stderr)

    def test_preserves_latency_percentiles_without_relabelling_as_means(self):
        record = {"fixture": "reply", "stage": "completion", "iterations": 10,
                  "cpu_seconds": 0.01, "elapsed_seconds": 0.02,
                  "allocations": {"kind": "measured", "scope": "constructor", "count": 20},
                  "p95_elapsed_ns": 3000000, "p99_elapsed_ns": 4000000}
        result = self.run_fixture(json.dumps(record))
        self.assertEqual(result.returncode, 0, result.stderr)
        summary = json.loads(result.stdout)["summary"][0]
        self.assertEqual(summary.get("median_p95_elapsed_ns"), 3000000)
        self.assertEqual(summary.get("median_p99_elapsed_ns"), 4000000)
        self.assertEqual(summary["allocations"]["count"], 20)

    def test_rejects_invalid_percentile(self):
        record = {"fixture": "reply", "stage": "completion", "iterations": 10,
                  "cpu_seconds": 0.01, "elapsed_seconds": 0.02, "allocations": None,
                  "p95_elapsed_ns": -1, "p99_elapsed_ns": 2}
        result = self.run_fixture(json.dumps(record))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("p95", result.stderr)

    def test_rejects_unlabelled_allocation_counts(self):
        record = {"fixture": "bad", "stage": "parse", "iterations": 10,
                  "cpu_seconds": 0.1, "elapsed_seconds": 0.1, "allocations": 8}
        result = self.run_fixture(json.dumps(record))
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("allocations", result.stderr)

    def test_rejects_non_json_and_invalid_repeat(self):
        result = self.run_fixture("not JSON")
        self.assertNotEqual(result.returncode, 0)
        result = self.run_fixture("{}", "--repeat", "0")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("repeat", result.stderr)


if __name__ == "__main__":
    unittest.main()
