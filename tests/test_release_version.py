"""Exercise release identity against temporary canonical version sources."""
import subprocess
import os
import sys
import tempfile
import unittest
from pathlib import Path

TOOL = Path(__file__).resolve().parents[1] / "tools/release_version.py"


class ReleaseVersionTests(unittest.TestCase):
    def check_version(self, source_version, tag, expected, output=None, *options):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            (source / "CMakeLists.txt").write_text(
                f"project(dcc\n VERSION {source_version}\n LANGUAGES C)\n")
            result = subprocess.run([sys.executable, str(TOOL), "--source", str(source),
                                     "--tag", tag, *options], capture_output=True, text=True)
        self.assertEqual(result.returncode, expected, result.stderr)
        if output is not None:
            self.assertEqual(result.stdout.strip(), output)

    def test_matching_release(self):
        self.check_version("2.1.0", "v2.1.0", 0, "2.1.0")

    def test_matching_prerelease(self):
        self.check_version("2.1.0", "v2.1.0-rc.1+build.9", 0, "2.1.0-rc.1+build.9")

    def test_mismatch(self):
        self.check_version("2.1.0", "v2.0.2", 1)

    def test_classifies_only_prerelease_not_build_metadata(self):
        self.check_version("2.1.0", "v2.1.0+build-1", 0, "false", "--prerelease")
        self.check_version("2.1.0", "v2.1.0-rc.1+build-1", 0, "true", "--prerelease")

    def test_malformed_source(self):
        for version in ("2.1.0oops", "2.1", "02.1.0", "2.1.0-rc.1"):
            with self.subTest(version=version):
                self.check_version(version, "v2.1.0", 1)

    def test_malformed_tag(self):
        for tag in ("", "v2.1.0-01", "v2.1.0-", "v2.1.0+", "v2.1.0.4", "v2.1.0-rc..1"):
            with self.subTest(tag=tag):
                self.check_version("2.1.0", tag, 1)

    @unittest.skipIf(os.name == "nt", "POSIX packager; Windows uses the same validator")
    def test_packager_validates_before_build_and_cannot_bypass_mismatch(self):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            (source / "CMakeLists.txt").write_text("project(dcc VERSION 2.1.0 LANGUAGES C)\n")
            for version, message in (("v2.0.2", "does not match"),
                                     ("v2.1.0-rc.1", "unsupported release target")):
                environment = {**os.environ, "DCC_SOURCE_DIR": str(source),
                               "DCC_RELEASE_VERSION": version, "DCC_PYTHON": sys.executable,
                               "DCC_ALLOW_VERSION_MISMATCH": "1"}
                result = subprocess.run(["sh", str(TOOL.with_name("package_release.sh")),
                                         "unsupported"], env=environment,
                                        capture_output=True, text=True)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn(message, result.stderr)


if __name__ == "__main__":
    unittest.main()
