"""Read the canonical project version and validate release identities."""
import argparse
from pathlib import Path
import re
import sys

NUMERIC = r"(?:0|[1-9][0-9]*)"
CORE = rf"{NUMERIC}\.{NUMERIC}\.{NUMERIC}"
IDENTIFIER = r"[0-9A-Za-z-]+"
SEMVER = re.compile(rf"({CORE})(?:-({IDENTIFIER}(?:\.{IDENTIFIER})*))?(?:\+{IDENTIFIER}(?:\.{IDENTIFIER})*)?")


def project_version(source: Path) -> str:
    cmake = (source / "CMakeLists.txt").read_text(encoding="utf-8")
    match = re.search(r"project\s*\(\s*dcc\s+VERSION\s+([^\s)]+)", cmake)
    if match is None or re.fullmatch(CORE, match.group(1)) is None:
        raise ValueError("CMakeLists.txt must declare a numeric x.y.z DCC project version")
    return match.group(1)


def release_version(source: Path, tag: str | None = None) -> str:
    current = project_version(source)
    version = (current if tag is None else tag).removeprefix("v")
    match = SEMVER.fullmatch(version)
    if match is None or (match.group(2) and any(
            part.isdigit() and len(part) > 1 and part.startswith("0")
            for part in match.group(2).split("."))):
        raise ValueError(f"invalid semantic release version: {version}")
    if match.group(1) != current:
        raise ValueError(f"release version {version} does not match CMake project version {current}")
    return version


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--tag")
    parser.add_argument("--prerelease", action="store_true",
                        help="print true only when the validated identity has a prerelease suffix")
    args = parser.parse_args()
    try:
        version = release_version(args.source, args.tag)
        print(str("-" in version.split("+", 1)[0]).lower() if args.prerelease else version)
    except (OSError, ValueError) as error:
        print(error, file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
