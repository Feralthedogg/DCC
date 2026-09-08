# Changelog

## 2.1.0 - 2026-09-08

Additive minor update preserving existing public signatures and record layouts,
ABI major 2, and the LLAM 2.2.1 minimum.

- Added eight Context/Flow `_ex` operations with explicit REST call options and
  optional owned request handles for wait, cancellation, and completion.
- Clarified borrowed inputs, callback results, queue admission, and ownership;
  refreshed current guides and generated installed-header API reference.
- Reduced request metadata allocations in measured offline fixtures; published
  scoped before/after evidence without claiming end-to-end latency gains.
- Added reproducible benchmark reporting and monotonic elapsed clocks, and
  derived version tooling from the canonical CMake project version.

## 2.0.2 - 2026-09-07

Patch release preserving the DCC 2 public API and ABI major 2.

- Hardened the legacy REST bridge with correct options/callback ownership and
  completed the previously unsupported integration, onboarding, emoji, sticker,
  scheduled-event, and current-user DM operations.
- Restored command synchronization callbacks and option validation, and added
  regression coverage for the legacy REST and command-sync surfaces.
- Made interaction replay validation timestamp-aware and capacity-bounded, and
  moved gateway, REST, and callback scratch state to request/session/task-owned
  storage for safe concurrent use.
- Replaced hot-reload fork/exec spawning with sanitized `posix_spawn` process
  isolation, removed reachable heap work from the gateway fast path, and made
  JSON payload resets deterministic.
- Enabled warnings-as-errors for the DCC library by default and expanded the
  operational, API, security, and release documentation.

## 2.0.1 - 2026-08-26

Patch release preserving the DCC 2 public API and ABI major 2.

- Updated the tested runtime baseline, installers, SBOM, packages, CI, and
  documentation to the immutable LLAM 2.2.1 release commit.
- Stabilized hosted release checks and restored FreeBSD, OpenBSD, and NetBSD
  build/package validation.
- Closed interaction-flow ownership leaks in sanitizer smoke tests.

## 2.0.0 - 2026-08-25

Stable major release with the canonical five-leaf Bot API, status-returning
handlers, flat listener configuration, ordered interaction actions, tagged
Components v2 builders, and request/result REST ownership. The installed
surface removes DCC 1 Sugar and App legacy headers, publishes exactly 224 REST
endpoints plus 35 generic operations, and requires LLAM 2.2.0 with ABI major 2.

See `docs/migration-1-to-2.md` for the intentional major-version migration.

## 1.5.0 - 2026-07-12

### Added

- Generated version contract and release audits for package, runtime, public headers, and ABI symbols.
- Components V2 message/modal context validation and current Discord field constraints.
- Components V2 checkbox groups, bounds validation, and correct modal/message serialization.
- Interaction callback response type 12, `with_response` REST support, replay protection,
  overload limits, response deadlines, graceful drain state, and health counters.
- Official runtime-loaded libdave session/MLS/key-ratchet/media backend with fail-closed
  Opus encryption/decryption and fake-ABI integration coverage.
- Signal-safe POSIX/Windows app runners and graceful `DCC_RUN_APP*` shutdown.
- Shared cluster IDENTIFY concurrency/session-limit coordination and jittered reconnect backoff.
- Prometheus renderers for cluster, interaction server, voice, and DAVE snapshots.
- `dcc_doctor` no-network installation/runtime diagnostics with human and JSON output.
- Strict replay JSON parsing, validation results, resource bounds, monotonic timestamp checks,
  CI expectation flags, and bounded realtime gaps.
- Normal bot systemd, Compose, and Kubernetes deployment templates.
- CycloneDX release SBOMs, provenance attestations, immutable workflow/dependency pins,
  community policy files, and issue/PR templates.

### Changed

- `DCC_BOT`, `DCC_GUILD_BOT`, and `DCC_APP_PRESET_BOT` now select production-safe
  presets and do not synchronize application commands on READY.
- Generated projects use `DCC_DEV_BOT`; explicit `DCC_DEV_BOT*` and
  `DCC_DEV_GUILD_BOT*` run/main aliases retain development auto-sync.
- DAVE negotiation fails closed when neither libdave nor an explicit external MLS handler is available.
- Release packaging always builds and runs the DCC test suite.
- Official Windows archives use Visual Studio's ClangCL toolset so the public
  C11 decorator API is compiled without MSVC's file-scope compound-literal limitation.
- The release-tested LLAM dependency is pinned to the immutable v2.2.0 commit.

### Fixed

- Generated welcome presets avoid a constant-condition warning when compiled
  with Visual Studio `/W4 /WX`.

### Compatibility

- The public API remains source- and ABI-compatible with DCC 1.4.x; new APIs are additive.
- Source code relying on `DCC_BOT` or `DCC_GUILD_BOT` READY-time command mutation must
  migrate to the explicitly named development aliases or an explicit command deployment step.
- LLAM 2.2.0 or newer with ABI major 2 is required.
