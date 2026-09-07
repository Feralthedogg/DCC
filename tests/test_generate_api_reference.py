"""Regression tests for actual generated API pages."""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class ReferenceTests(unittest.TestCase):
    def test_declaration_excludes_file_preamble(self):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            (source / "include/dcc").mkdir(parents=True)
            (source / "CMakeLists.txt").write_text("project(dcc VERSION 2.1.0 LANGUAGES C)\n")
            (source / "include/dcc/fixture.h").write_text('''/** File overview. */
#ifndef DCC_FIXTURE_H
#define DCC_FIXTURE_H
typedef void (*dcc_fixture_cb)(void *);
/** Perform the fixture operation. */
DCC_API int dcc_fixture(int value, dcc_fixture_cb callback);
#endif
''')
            result = subprocess.run([sys.executable, str(ROOT / "tools/generate_api_reference.py"),
                                     "--source", str(source), "--write"], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            page = (source / "docs/reference/api/fixture.md").read_text()
            self.assertIn("Perform the fixture operation.\n", page)
            self.assertIn("```c\nDCC_API int dcc_fixture(int value, dcc_fixture_cb callback);\n```", page)
            self.assertNotIn("File overview", page)
            self.assertIn("DCC 2.1.0", (source / "docs/reference/api/index.md").read_text())


if __name__ == "__main__":
    unittest.main()
