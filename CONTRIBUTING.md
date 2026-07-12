# Contributing to DCC

Thank you for improving DCC. Keep changes focused, testable, and compatible
with the public C11 API.

## Before Coding

Open an issue for new public API, protocol behavior, or architectural changes.
Small fixes and documentation corrections can go directly to a pull request.
Never include Discord tokens, replay captures containing user data, private
guild IDs, or generated credentials.

## Development Setup

DCC 1.5.x requires LLAM 2.2.0 or newer. CI pins the tested LLAM 2.2.0 commit.

```sh
cmake -S . -B build-dev \
  -DDCC_BUILD_TESTS=ON \
  -DDCC_BUILD_EXAMPLES=ON \
  -DDCC_BUILD_TOOLS=ON \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ROOT=../LLAM
cmake --build build-dev
ctest --test-dir build-dev --output-on-failure
```

Use C11, preserve ownership rules, keep public declarations under
`include/dcc/`, and keep implementation-only declarations under
`src/internal/`. Public structs are size-versioned; append fields and gate reads
by `size`. New public functions require a standalone-header compile check and a
test covering error contracts.

## Pull Requests

Run `tools/release_check.sh` for release-sensitive changes. At minimum, run the
focused test plus `git diff --check`. Update `CHANGELOG.md` and the relevant
guide when behavior visible to users changes. Explain compatibility impact,
failure modes, and verification in the pull request description.

By participating, you agree to follow the [Code of Conduct](CODE_OF_CONDUCT.md).
