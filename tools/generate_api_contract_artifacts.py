#!/usr/bin/env python3
"""Generate deterministic DCC 2 source/package compatibility artifacts."""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

from generate_api_reference import installed_headers


def digest(value: str) -> str:
    return hashlib.sha256(value.encode("utf-8")).hexdigest()


def normalized(value: str) -> str:
    return " ".join(value.split())


def artifacts(source: Path) -> dict[Path, str]:
    declarations = []
    functions = []
    inline = []
    types = []
    records = []
    macros = []
    reachability = []
    api_re = re.compile(r"DCC_API\s+([\s\S]*?\b(dcc_[A-Za-z0-9_]+)\s*\([^;{}]*\)\s*;)")
    inline_re = re.compile(r"static\s+inline\s+([\s\S]*?\b((?:dcc|DCC)_[A-Za-z0-9_]+)\s*\([^{};]*\)\s*\{)")
    typedef_re = re.compile(r"typedef\s+(struct|union|enum)?[\s\S]*?\b([A-Za-z_][A-Za-z0-9_]*_t)\s*;")
    record_re = re.compile(r"typedef\s+(struct|union)\s+([A-Za-z_][A-Za-z0-9_]*)?\s*\{([\s\S]*?)\}\s*([A-Za-z_][A-Za-z0-9_]*_t)\s*;")
    macro_re = re.compile(r"^\s*#\s*define\s+(DCC_[A-Za-z0-9_]+)(\(([^)]*)\))?\s*(.*)$", re.MULTILINE)
    for header in installed_headers(source):
        owner = header.relative_to(source / "include").as_posix()
        text = header.read_text(encoding="utf-8")
        external_names = []
        for match in api_re.finditer(text):
            signature, name = normalized(match.group(1)), match.group(2)
            row = {"name": name, "owner": owner, "signature": signature,
                   "fingerprint": digest(signature), "class": "public_external"}
            declarations.append(row)
            functions.append({**row, "dcc_api": True, "definition": False})
            external_names.append(name)
        inline_names = []
        for match in inline_re.finditer(text):
            signature, name = normalized(match.group(1)), match.group(2)
            row = {"name": name, "owner": owner, "signature": signature,
                   "fingerprint": digest(signature), "class": "public_inline"}
            inline.append(row)
            functions.append({**row, "dcc_api": False, "definition": True})
            inline_names.append(name)
        type_names = []
        for match in typedef_re.finditer(text):
            name = match.group(2)
            type_names.append(name)
            types.append({"name": name, "owner": owner,
                          "kind": match.group(1) or "alias"})
        for match in record_re.finditer(text):
            kind, body, name = match.group(1), match.group(3), match.group(4)
            has_size = bool(re.search(r"\bsize_t\s+size\s*;", body))
            has_version = bool(re.search(r"\buint32_t\s+version\s*;", body))
            layout = "versioned" if has_version else "size_extensible" if has_size else "fixed"
            records.append({
                "name": name, "owner": owner, "kind": kind, "layout": layout,
                "direction": "inout", "lifetime": "trivial",
                "writer_policy": "covered_atomic" if has_size else "full_fixed",
                "append_policy": "new_version_only" if has_version else
                                 "suffix_same_layout" if has_size else "forbidden",
                "array_stride": "fixed_sizeof" if layout == "fixed" else "not_arrayed",
            })
        macro_names = []
        for match in macro_re.finditer(text):
            name = match.group(1)
            if name.endswith("_H"):
                continue
            parameters = match.group(3)
            row = {"name": name, "owner": owner,
                   "kind": "function" if match.group(2) else "object",
                   "arity": None if parameters is None else
                            (0 if parameters == "" else len(parameters.split(","))),
                   "expansion": normalized(match.group(4))}
            macros.append(row)
            macro_names.append(name)
        reachability.append({"header": owner, "external": sorted(external_names),
                             "inline": sorted(inline_names), "types": sorted(set(type_names)),
                             "macros": sorted(macro_names)})
    key = lambda row: (row.get("name", ""), row.get("owner", ""))
    payloads = {
        "api_v2_declarations.json": {"schema": 1, "version": "2.0.1", "declarations": sorted(declarations, key=key)},
        "api_v2_function_inventory.json": {"schema": 1, "version": "2.0.1", "functions": sorted(functions, key=key)},
        "api_v2_inline_api.json": {"schema": 1, "version": "2.0.1", "inline": sorted(inline, key=key)},
        "api_v2_types.json": {"schema": 1, "version": "2.0.1", "types": sorted(types, key=key)},
        "api_v2_records.json": {"schema": 1, "version": "2.0.1", "records": sorted(records, key=key)},
        "api_v2_macro_sources.json": {"schema": 1, "version": "2.0.1", "macros": sorted(macros, key=key)},
        "api_v2_header_reachability.json": {"schema": 1, "version": "2.0.1", "headers": sorted(reachability, key=lambda row: row["header"])},
        "api_v2_package_contract.json": {
            "schema": 1, "version": "2.0.1", "cmake_package": "dcc",
            "target": "dcc::dcc", "c_standard": 11,
            "llam_minimum": "2.2.1", "llam_abi_major": 2,
            "pkg_config": {"module": "dcc", "name": "DCC"},
        },
    }
    result = {source / "tools" / name: json.dumps(value, ensure_ascii=False, indent=2, sort_keys=True) + "\n"
              for name, value in payloads.items()}
    result[source / "tools/api_v2_bot_macros_posix_clang_c_static.txt"] = (
        source / "tools/bot_v2_macros_posix_clang_c_static.txt").read_text(encoding="utf-8")
    result[source / "tools/api_v2_bot_macros_posix_clang_cpp_static.txt"] = (
        source / "tools/bot_v2_macros_posix_clang_cpp_static.txt").read_text(encoding="utf-8")
    return result


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--write", action="store_true")
    mode.add_argument("--check", action="store_true")
    args = parser.parse_args()
    expected = artifacts(args.source.resolve())
    if args.write:
        for path, content in expected.items():
            path.write_text(content, encoding="utf-8", newline="\n")
        print(f"generated {len(expected)} API contract artifacts")
        return 0
    stale = [path for path, content in expected.items()
             if not path.is_file() or path.read_text(encoding="utf-8") != content]
    if stale:
        print("stale API contract artifacts: " + ", ".join(str(path) for path in stale), file=sys.stderr)
        return 1
    print(f"API contract artifacts are current: {len(expected)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
