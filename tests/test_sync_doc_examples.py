#!/usr/bin/env python3
from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
TOOL = ROOT / "tools/sync_doc_examples.py"


class SyncDocExamplesTests(unittest.TestCase):
    def run_tool(
        self, source_root: Path, *args: str
    ) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(TOOL), "--source-root", str(source_root), *args],
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )

    def write_fixture(self, root: Path, source: str, document: str) -> Path:
        source_path = root / "examples/docs/rest_ownership.c"
        document_path = root / "docs/concepts/ownership-and-async.md"
        source_path.parent.mkdir(parents=True)
        document_path.parent.mkdir(parents=True)
        source_path.write_text(source, encoding="utf-8")
        document_path.write_text(document, encoding="utf-8")
        return document_path

    def test_update_renders_the_tagged_c_region_and_preserves_prose(self) -> None:
        # Catches a synchronizer that copies markers, omits the fence, or
        # rewrites prose outside the declared destination region.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            document_path = self.write_fixture(
                root,
                "header\n"
                "/* DCC_DOC_SNIPPET_BEGIN(rest-ownership) */\n"
                "int main(void) { return 0; }\n"
                "/* DCC_DOC_SNIPPET_END(rest-ownership) */\n"
                "footer\n",
                "prose before\n"
                "<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\n"
                "stale\n"
                "<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\n"
                "prose after\n",
            )

            result = self.run_tool(root)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(
                document_path.read_text(encoding="utf-8"),
                "prose before\n"
                "<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\n"
                "```c\n"
                "int main(void) { return 0; }\n"
                "```\n"
                "<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\n"
                "prose after\n",
            )

    def test_check_reports_drift_without_modifying_the_document(self) -> None:
        # Catches a --check implementation that silently repairs drift or
        # succeeds when the checked-in snippet no longer matches its source.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            original = (
                "<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\n"
                "```c\nold();\n```\n"
                "<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\n"
            )
            document_path = self.write_fixture(
                root,
                "/* DCC_DOC_SNIPPET_BEGIN(rest-ownership) */\n"
                "new();\n"
                "/* DCC_DOC_SNIPPET_END(rest-ownership) */\n",
                original,
            )

            result = self.run_tool(root, "--check")

            self.assertEqual(result.returncode, 1, result.stderr)
            self.assertIn("out of date", result.stderr)
            self.assertEqual(document_path.read_text(encoding="utf-8"), original)

    def test_update_preserves_crlf_in_unmarked_document_bytes(self) -> None:
        # Catches text-mode universal-newline reads that silently rewrite
        # prose outside the declared destination region from CRLF to LF.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            document_path = self.write_fixture(
                root,
                "/* DCC_DOC_SNIPPET_BEGIN(rest-ownership) */\n"
                "new();\n"
                "/* DCC_DOC_SNIPPET_END(rest-ownership) */\n",
                "placeholder\n",
            )
            document_path.write_bytes(
                b"prose before\r\n"
                b"<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\r\n"
                b"stale\r\n"
                b"<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\r\n"
                b"prose after\r\n"
            )

            result = self.run_tool(root)

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(
                document_path.read_bytes(),
                b"prose before\r\n"
                b"<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\r\n"
                b"```c\r\n"
                b"new();\r\n"
                b"```\r\n"
                b"<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\r\n"
                b"prose after\r\n",
            )

    def test_missing_marker_is_an_error_with_the_file_and_marker_name(self) -> None:
        # Catches accidental broad replacement when either declaration is
        # incomplete instead of refusing to update an ambiguous document.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            self.write_fixture(
                root,
                "int main(void) { return 0; }\n",
                "<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\n"
                "stale\n"
                "<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\n",
            )

            result = self.run_tool(root)

            self.assertEqual(result.returncode, 2)
            self.assertIn("examples/docs/rest_ownership.c", result.stderr)
            self.assertIn("DCC_DOC_SNIPPET_BEGIN(rest-ownership)", result.stderr)
            self.assertIn("missing", result.stderr.lower())

    def test_duplicate_marker_is_rejected_as_ambiguous(self) -> None:
        # Catches a first-match implementation that would leave a second
        # declared region stale or update an unintended region.
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            begin = "<!-- DCC_DOC_SNIPPET_BEGIN(rest-ownership) -->\n"
            end = "<!-- DCC_DOC_SNIPPET_END(rest-ownership) -->\n"
            self.write_fixture(
                root,
                "/* DCC_DOC_SNIPPET_BEGIN(rest-ownership) */\n"
                "current();\n"
                "/* DCC_DOC_SNIPPET_END(rest-ownership) */\n",
                begin + "one\n" + end + begin + "two\n" + end,
            )

            result = self.run_tool(root, "--check")

            self.assertEqual(result.returncode, 2)
            self.assertIn("docs/concepts/ownership-and-async.md", result.stderr)
            self.assertIn("DCC_DOC_SNIPPET_BEGIN(rest-ownership)", result.stderr)
            self.assertIn("duplicate", result.stderr.lower())


if __name__ == "__main__":
    unittest.main()
