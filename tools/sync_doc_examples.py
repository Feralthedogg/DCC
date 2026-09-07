#!/usr/bin/env python3
"""Synchronize declared C example regions into documentation."""

from __future__ import annotations

import argparse
from pathlib import Path
import sys


SNIPPETS = (
    (
        "rest-ownership",
        Path("examples/docs/rest_ownership.c"),
        Path("docs/concepts/ownership-and-async.md"),
    ),
    ("interaction-flows", Path("examples/docs/guide_flows.c"),
     Path("docs/guides/interaction-flows.md")),
    ("component-sessions", Path("examples/docs/guide_sessions.c"),
     Path("docs/guides/component-sessions.md")),
    ("command-registry", Path("examples/docs/guide_configuration.c"),
     Path("docs/guides/command-registry.md")),
    ("replay-records", Path("examples/docs/guide_configuration.c"),
     Path("docs/guides/replay.md")),
    ("rest-firewall", Path("examples/docs/guide_configuration.c"),
     Path("docs/guides/rate-limit-firewall.md")),
    ("hot-reload-canary", Path("examples/docs/guide_configuration.c"),
     Path("docs/guides/hot-reload.md")),
)


class SyncError(Exception):
    pass


def _unique_marker(text: str, marker: str, path: Path) -> int:
    count = text.count(marker)
    if count == 0:
        raise SyncError(f"{path}: missing marker {marker}")
    if count != 1:
        raise SyncError(f"{path}: duplicate marker {marker}")
    return text.index(marker)


def _region(text: str, begin: str, end: str, path: Path) -> str:
    begin_at = _unique_marker(text, begin, path)
    end_at = _unique_marker(text, end, path)
    content_at = begin_at + len(begin)
    if end_at < content_at:
        raise SyncError(f"{path}: end marker appears before begin marker {begin}")
    return text[content_at:end_at].strip("\r\n")


def _render_document(name: str, source: str, document: str, source_path: Path,
                     document_path: Path) -> str:
    source_begin = f"/* DCC_DOC_SNIPPET_BEGIN({name}) */"
    source_end = f"/* DCC_DOC_SNIPPET_END({name}) */"
    document_begin = f"<!-- DCC_DOC_SNIPPET_BEGIN({name}) -->"
    document_end = f"<!-- DCC_DOC_SNIPPET_END({name}) -->"

    snippet = _region(source, source_begin, source_end, source_path)
    begin_at = _unique_marker(document, document_begin, document_path)
    end_at = _unique_marker(document, document_end, document_path)
    replace_end = end_at + len(document_end)
    if end_at < begin_at + len(document_begin):
        raise SyncError(
            f"{document_path}: end marker appears before begin marker {document_begin}"
        )
    newline = "\r\n" if "\r\n" in document else "\n"
    snippet = snippet.replace("\r\n", "\n").replace("\r", "\n")
    snippet = snippet.replace("\n", newline)
    rendered = newline.join(
        (document_begin, "```c", snippet, "```", document_end)
    )
    return document[:begin_at] + rendered + document[replace_end:]


def sync(source_root: Path, check: bool) -> int:
    pending: list[tuple[Path, str]] = []
    for name, source_relative, document_relative in SNIPPETS:
        source_path = source_root / source_relative
        document_path = source_root / document_relative
        try:
            source = source_path.read_bytes().decode("utf-8")
            document = document_path.read_bytes().decode("utf-8")
        except OSError as error:
            raise SyncError(str(error)) from error
        rendered = _render_document(
            name, source, document, source_relative, document_relative
        )
        if rendered != document:
            pending.append((document_path, rendered))

    if check:
        for path, _ in pending:
            print(f"{path.relative_to(source_root)}: generated snippet is out of date",
                  file=sys.stderr)
        return 1 if pending else 0

    for path, rendered in pending:
        path.write_bytes(rendered.encode("utf-8"))
        print(f"updated {path.relative_to(source_root)}")
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--source-root",
        type=Path,
        default=Path(__file__).resolve().parents[1],
        help="repository root (defaults to the root containing this script)",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="report stale generated snippets without modifying files",
    )
    args = parser.parse_args(argv)
    try:
        return sync(args.source_root.resolve(), args.check)
    except SyncError as error:
        print(f"sync_doc_examples: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
