# Task 15 implementer report — DCC 2.0.0 Stable release

## Result

Task 15 is implemented on `codex/v2-api-impl`. The project, ABI, packages,
documentation, CI release workflow, and changelog identify the release as
`2.0.0` / `v2.0.0` Stable. The shared-library ABI major is 2.

## Release identity and artifacts

- CMake project version: **2.0.0**.
- Shared ABI: `libdcc.2.0.0.dylib`, install name `@rpath/libdcc.2.dylib`, with
  major and unversioned symlinks.
- LLAM compatibility floor: **2.2.0**; attempts to override the floor are
  rejected.
- Release workflow accepts only the exact `v2.0.0` tag and publishes a
  non-prerelease GitHub release.
- CPack binary and source archives use the 2.0.0 identity and pass package
  hygiene/negative-path checks.
- README, getting started, API-layer, Components v2, release, versioning, and
  migration documentation describe the canonical DCC 2 surface.

## Generated contract and documentation

- `tools/api_v2_symbols.txt` freezes all **1,239** public declarations.
- Ten deterministic JSON/text artifacts preserve declarations, types, records,
  inline APIs, macro provenance, reachability, package topology, and the major
  reset ledger.
- The generated API reference contains **175 pages** and is checked for
  freshness in CTest.
- `python3 -m mkdocs build --strict` passes.

## Release gate evidence

`DCC_RELEASE_VERSION=v2.0.0 DCC_BUILD_PARALLEL=4
DCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM tools/release_check.sh`
completed successfully with:

- primary native suite: **121/121 passed**;
- ASan/UBSan deterministic suite: **118/118 passed**;
- no-Opus suite: **121/121 passed**;
- normal and minimal install/package consumers: passed;
- normal and minimal pkg-config consumers: passed;
- CPack and source package hygiene: passed;
- hot-reload, webhook, normal bot, deploy-template, voice dry-run, and JSON
  seed-corpus checks: passed;
- JSON fuzzer target: built and seed corpus completed.

The native suite continues to cover the cluster gateway, hot-reload canary,
and last-good process integrations. On macOS those three process/network cases
are excluded only from the sanitizer pass because ASan can block teardown or
emit its documented `__asan_handle_no_return` false-positive warning around
stackful worker termination.

The release-check pkg-config runner now supplies the external LLAM runtime
directory at execution time on Darwin, Linux, and NetBSD, fixing the final
installed-consumer dynamic-link failure.

## Performance sample

The release gate's 20,000-iteration gateway benchmark completed successfully.
The representative `message_create` gateway fast path measured about
5.53 microseconds per operation (88.29 MiB/s) on the release host. These values
are evidence from one host, not cross-library benchmark claims.
