#!/usr/bin/env python3
from __future__ import annotations

import os
from pathlib import Path
import stat
import subprocess
import sys
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[1]
TOOL = ROOT / "tools/dcc_migrate_v2.py"


class MigrationToolTests(unittest.TestCase):
    def run_tool(self, *args: str) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [sys.executable, str(TOOL), *args],
            cwd=ROOT,
            text=True,
            capture_output=True,
            check=False,
        )

    def test_safe_token_dry_run_and_write_are_idempotent(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "bot.c"
            source.write_text(
                "void f(void) { DCC_RESPOND_TEXT(ctx, \"ok\"); }\n",
                encoding="utf-8",
            )
            dry = self.run_tool(str(source))
            self.assertEqual(dry.returncode, 1, dry.stderr)
            self.assertIn("-void f(void) { DCC_RESPOND_TEXT", dry.stdout)
            self.assertIn("+void f(void) { DCC_CTX_REPLY_TEXT", dry.stdout)
            self.assertIn(f"{source}:1:16", dry.stderr)
            written = self.run_tool("--write", str(source))
            self.assertEqual(written.returncode, 0, written.stderr)
            self.assertIn("DCC_CTX_REPLY_TEXT", source.read_text(encoding="utf-8"))
            again = self.run_tool("--write", str(source))
            self.assertEqual(again.returncode, 0, again.stderr)
            self.assertEqual(again.stdout, "")

    def test_comments_literals_larger_tokens_and_preprocessor_are_preserved(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "lexical.c"
            original = (
                "// DCC_RESPOND_TEXT\n"
                "const char *s = \"DCC_RESPOND_TEXT\";\n"
                "int DCC_RESPOND_TEXT_EXTRA;\n"
                "#define DCC_RESPOND_TEXT(x) x\n"
                "#if defined(DCC_RESPOND_TEXT)\n"
                "#endif\n"
            )
            source.write_text(original, encoding="utf-8")
            result = self.run_tool("--write", str(source))
            self.assertEqual(result.returncode, 2)
            self.assertEqual(source.read_text(encoding="utf-8"), original)
            self.assertIn("user-owned macro definition", result.stderr)
            self.assertIn("conditional macro use", result.stderr)

    def test_direct_looking_and_family_migrations_are_manual(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "manual.c"
            source.write_text(
                "#include <dcc/sugar.h>\n"
                "DCC_SIMPLE_BOT_MAIN(DCC_V2_TEXT(\"x\"))\n"
                "DCC_PRIVATE_TEXT(ctx, \"secret\");\n"
                "DCC_ON_READY(on_ready);\n",
                encoding="utf-8",
            )
            result = self.run_tool("--write", str(source))
            self.assertEqual(result.returncode, 2)
            self.assertIn("include selection is semantic", result.stderr)
            self.assertIn("development/production policy decision", result.stderr)
            self.assertIn("storage-duration change", result.stderr)
            self.assertIn("ephemeral-policy decision", result.stderr)
            self.assertIn("listener conversion required", result.stderr)

    def test_crlf_mode_and_final_newline_state_are_preserved(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            source = Path(directory) / "mode.c"
            source.write_bytes(b"DCC_RESPOND_TEXT(ctx, \"ok\");\r\nDCC_RESPOND_TEXT(ctx, \"x\");")
            source.chmod(0o751)
            result = self.run_tool("--write", str(source))
            self.assertEqual(result.returncode, 0, result.stderr)
            data = source.read_bytes()
            self.assertEqual(data.count(b"\r\n"), 1)
            self.assertFalse(data.endswith(b"\n"))
            self.assertNotIn(b"DCC_RESPOND_TEXT", data)
            if os.name != "nt":
                self.assertEqual(stat.S_IMODE(source.stat().st_mode), 0o751)

    def test_missing_input_has_io_exit(self) -> None:
        result = self.run_tool("/definitely/missing/dcc-file.c")
        self.assertEqual(result.returncode, 3)
        self.assertIn("I/O error", result.stderr)


if __name__ == "__main__":
    unittest.main()
