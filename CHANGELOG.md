# Changelog

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
