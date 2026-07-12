#!/usr/bin/env python3
"""Generate a deterministic CycloneDX SBOM for DCC release artifacts."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path


VERSION_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+(?:[-+][0-9A-Za-z.-]+)?$")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", required=True)
    parser.add_argument("--artifacts", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    version = args.version.removeprefix("v")
    if not VERSION_RE.fullmatch(version):
        parser.error("--version must be a semantic version")
    artifact_root = args.artifacts.resolve()
    output = args.output.resolve()
    if not artifact_root.is_dir():
        parser.error("--artifacts must be a directory")

    artifact_components: list[dict[str, object]] = []
    for path in sorted(artifact_root.rglob("*")):
        resolved = path.resolve()
        if not path.is_file() or path.is_symlink() or resolved == output:
            continue
        relative = path.relative_to(artifact_root).as_posix()
        artifact_components.append(
            {
                "type": "file",
                "bom-ref": f"artifact:{relative}",
                "name": relative,
                "hashes": [{"alg": "SHA-256", "content": sha256(path)}],
                "properties": [{"name": "dcc:artifact:size", "value": str(path.stat().st_size)}],
            }
        )

    dependencies = [
        {
            "type": "library",
            "bom-ref": "pkg:github/Feralthedogg/LLAM@2.2.0",
            "name": "LLAM",
            "version": "2.2.0",
            "purl": "pkg:github/Feralthedogg/LLAM@2.2.0",
        },
        {
            "type": "library",
            "bom-ref": "dependency:openssl",
            "name": "OpenSSL",
            "scope": "required",
        },
        {
            "type": "library",
            "bom-ref": "dependency:discord-libdave",
            "name": "discord/libdave",
            "scope": "optional",
            "purl": "pkg:github/discord/libdave",
        },
        {
            "type": "library",
            "bom-ref": "dependency:opus",
            "name": "libopus",
            "scope": "optional",
        },
    ]
    root_ref = f"pkg:github/Feralthedogg/DCC@{version}"
    document = {
        "bomFormat": "CycloneDX",
        "specVersion": "1.5",
        "version": 1,
        "metadata": {
            "tools": {
                "components": [
                    {"type": "application", "name": "dcc-generate-sbom", "version": version}
                ]
            },
            "component": {
                "type": "library",
                "bom-ref": root_ref,
                "name": "DCC",
                "version": version,
                "purl": root_ref,
            },
        },
        "components": dependencies + artifact_components,
        "dependencies": [
            {
                "ref": root_ref,
                "dependsOn": [component["bom-ref"] for component in dependencies],
            }
        ],
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(document, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"wrote CycloneDX SBOM with {len(artifact_components)} artifacts: {output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
