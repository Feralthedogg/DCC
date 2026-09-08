"""Regression tests for actual generated API pages."""
import subprocess
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class ReferenceTests(unittest.TestCase):
    def test_real_callable_headings_are_unique_and_match_their_signatures(self):
        cases = {
            "app/lifecycle": ("dcc_app_destroy", "dcc_app_stop"),
            "rest/official_surface": ("dcc_rest_get_gateway",),
            "rest/request": ("dcc_rest_request_wait",),
            "rest/result": ("dcc_rest_result_free",),
        }
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            shutil.copy2(ROOT / "CMakeLists.txt", source / "CMakeLists.txt")
            for stem in cases:
                header = source / f"include/dcc/{stem}.h"
                header.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(ROOT / f"include/dcc/{stem}.h", header)
            result = subprocess.run([sys.executable, str(ROOT / "tools/generate_api_reference.py"),
                                     "--source", str(source), "--write"], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            for stem, names in cases.items():
                page = (source / f"docs/reference/api/{stem}.md").read_text()
                headings = [line for line in page.splitlines() if line.startswith("## ")]
                self.assertEqual(len(headings), len(set(headings)), stem)
                for name in names:
                    heading = f"## `{name}`\n"
                    self.assertEqual(page.count(heading), 1, name)
                    section = page.split(heading, 1)[1].split("\n## ", 1)[0]
                    signature = section.split("```c\n", 1)[1].split("\n```", 1)[0]
                    self.assertIn(name + "(", signature)

    def test_declaration_excludes_file_preamble(self):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary)
            (source / "include/dcc").mkdir(parents=True)
            (source / "CMakeLists.txt").write_text("project(dcc VERSION 2.1.0 LANGUAGES C)\n")
            (source / "include/dcc/fixture.h").write_text('''/** File overview. */
#ifndef DCC_FIXTURE_H
#define DCC_FIXTURE_H
typedef void (*dcc_fixture_cb)(void *);
/** Perform the fixture operation after dcc_other_operation(). */
DCC_API /* dcc_another_operation() is not the name. */ int dcc_fixture(int value, dcc_fixture_cb callback);
#endif
''')
            result = subprocess.run([sys.executable, str(ROOT / "tools/generate_api_reference.py"),
                                     "--source", str(source), "--write"], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            page = (source / "docs/reference/api/fixture.md").read_text()
            self.assertIn("Perform the fixture operation after dcc_other_operation().\n", page)
            self.assertIn("## `dcc_fixture`\n", page)
            self.assertNotIn("## `dcc_other_operation`", page)
            self.assertIn("```c\nDCC_API int dcc_fixture(int value, dcc_fixture_cb callback);\n```", page)
            self.assertNotIn("File overview", page)
            self.assertIn("DCC 2.1.0", (source / "docs/reference/api/index.md").read_text())


if __name__ == "__main__":
    unittest.main()
