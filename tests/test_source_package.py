"""Exercise generated CPack exclusions with local-only and historical fixtures."""
import argparse
from pathlib import Path
import subprocess
import tarfile
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--cpack", default="cpack")
    args = parser.parse_args()
    with tempfile.TemporaryDirectory() as temporary:
        root = Path(temporary)
        source = root / ".worktrees/source+fixture"
        (source / ".superpowers").mkdir(parents=True)
        (source / ".superpowers/report.md").write_text("local-only evidence")
        (source / ".git").write_text("gitdir: /private/local/worktree")
        (source / ".worktrees/nested").mkdir(parents=True)
        (source / ".worktrees/nested/private.c").write_text("local worktree")
        (source / "build-local").mkdir()
        (source / "build-local/cache").write_text("local build")
        (source / "docs/superpowers").mkdir(parents=True)
        (source / "docs/superpowers/history.md").write_text("tracked historical document")
        (source / "keep.c").write_text("int main(void) { return 0; }\n")
        # Rebase the same production exclusions for the overridden fixture root.
        # The real configured source root is verified by the release archive check.
        module = Path(__file__).resolve().parents[1] / "cmake/SourcePackageIgnore.cmake"
        config = root / "fixture.cmake"
        config.write_text(
            f'include([=[{args.config.resolve()}]=])\n'
            f'include([=[{module}]=])\n'
            f'dcc_source_package_ignore_files([=[{source}]=] CPACK_IGNORE_FILES)\n')
        result = subprocess.run([args.cpack, "-G", "TGZ", "--config", str(config),
                        "-D", f"CPACK_INSTALLED_DIRECTORIES={source};/",
                        "-B", str(root / "output")], capture_output=True, text=True)
        assert result.returncode == 0, result.stdout + result.stderr
        archives = list((root / "output").glob("*.tar.gz"))
        assert len(archives) == 1, archives
        with tarfile.open(archives[0]) as archive:
            names = archive.getnames()
        leaked = [name for name in names if
                  {".git", ".superpowers", ".worktrees", "build-local"} & set(Path(name).parts)]
        assert not leaked, f"source package leaked local-only files: {leaked}"
        assert any(name.endswith("/docs/superpowers/history.md") for name in names), (names, result.stdout, result.stderr)
        assert any(name.endswith("/keep.c") for name in names), names
    print("source package preserves deliverables/history and excludes local-only metadata")


if __name__ == "__main__":
    main()
