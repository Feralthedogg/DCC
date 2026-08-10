# Task 2 implementer report — focused status-returning App API

## What I implemented

- Replaced the four declaration fragments with focused public App headers for
  base types, options, lifecycle, listeners, context, modules, and store APIs.
- Added the versioned `dcc_listener_t` registration model with canonical
  `dcc_listener_init`, `dcc_app_listen`, and `dcc_app_unlisten` entry points.
- Made canonical route, typed-route, event, ready, message, message-command,
  and scheduled-task callbacks return `dcc_status_t`; retained the 1.x void
  callback surface only in the explicitly transitional `legacy.h` header.
- Added kind-specific listener metadata validation, typed binding and validator
  bounds checks, subcommand autocomplete targeting, returned-status reporting,
  and exactly-once cleanup semantics.
- Added a focused listener smoke test and kept existing 1.x examples, tools,
  and tests source-compatible through the transition layer.

## TDD evidence

- RED: the initial focused fixture failed on missing `dcc_listener_t`,
  `dcc_listener_init`, `DCC_LISTENER_SLASH`, and `dcc_app_listen`; a status
  callback was incompatible with the old void-returning registration API.
- GREEN: implementing the focused surface made route, typed-route, event, and
  task registration/dispatch tests pass.
- Expanded cleanup coverage exposed a real App-destruction use-after-free: a
  one-pass destructor freed an early listener while a later `unlisten` still
  scanned it.  App destruction now unregisters/cleans every entry first and
  frees entries in a separate second pass.

## Tests and exact results

- Full configured build with tests, examples, and tools enabled: passed.
- Full CTest: `171/171` enabled tests passed in 86.02s; 24 LLAM tests were
  disabled by the current configuration.
- Focused CTest (`dcc_v2_surface_audit`, `dcc_app_smoke`,
  `dcc_app_v2_listener_smoke`, `dcc_project_layout_audit`): `4/4` passed.
- ASan rebuild and `dcc_app_v2_listener_smoke`: `1/1` passed in 0.08s.
- Strict `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -fsyntax-only`
  checks for `src/app/app_listener.c` and `tests/app_v2_listener_smoke.c`:
  passed with no diagnostics.
- `git diff --check`: passed.

## Self-review findings

- Scheduled tasks store the internal listener adapter and stable listener
  entry, check active state before execution, roll back insertion on spawn
  failure, and run cleanup exactly once after unlisten or destruction.
- Typed binding failures never expose an uninitialized validation error; the
  structure is zeroed and generic validation/autocomplete fallbacks are used.
- `dcc_listener_init` initializes the selected target union member's own size
  and version for event, message-command, schedule, and route kinds.
- Canonical route dispatch separates internal adapter state from public context
  user data, so middleware and error observers see the listener's user data.
- Listener destruction uses two phases to prevent cross-entry scans from
  touching freed storage.

## Transition debt and Task 14 obligation

- `include/dcc/app/legacy.h` intentionally consolidates the 1.x App surface so
  existing examples and tools continue to compile during the redesign.
- `tools/audit_project_layout.py` contains one exact
  `TRANSITION_LARGE_FILE_LIMITS` exception for that file.  Task 14 must delete
  both `include/dcc/app/legacy.h` and the matching audit exception; neither is
  intended to survive the 2.0 surface cleanup.

## Files of interest

- `include/dcc/app.h`
- `include/dcc/app/{base,options,lifecycle,listeners,context,modules,store}.h`
- `include/dcc/app/legacy.h`
- `src/app/app_listener.c`
- `src/internal/app/dcc_app_internal.h`
- `tests/app_v2_listener_smoke.c`

## Fix Round 1 — concurrency, contract, and header review

### Review findings addressed

- Added a portable App-local recursive mutex and condition variable using
  pthreads on POSIX and critical sections/condition variables on Windows.
  Listener entries now move from the active array to a stable retired chain,
  carry in-flight references, and are reclaimed only after client teardown.
- Canonical route lookup and listener acquisition now share the registry lock.
  External unlisten waits for in-flight handlers; self-unlisten defers route
  removal and cleanup to the final release, avoiding deadlock and early cleanup.
- Event and task adapters acquire the same lifetime reference. Once listeners
  are claimed and retired atomically, detached before another dispatch can
  claim them, and cleaned exactly once after their sole handler returns.
- Canonical schedules now have an atomic cancel flag, are removed from the App
  schedule array on unlisten, and check cancellation during bounded sleep
  slices. Their storage remains owned by the retired listener until workers
  have quiesced.
- Listener registration now validates and deep-copies nested binding and
  validator strings and fallback arrays before mutation. Route registration is
  inactive until commit, removes the route and restores its ID on failure, and
  adds command schema last so invalid schema attempts leave no partial App
  state and can be retried.
- Validation now rejects non-boolean flags, contradictory DM/guild policies,
  pointer/count mismatches, inactive target/binding/validator fields, component
  command builders, and non-zero inactive schedule/event fields. Nested ABI
  checks use the corresponding public `DCC_LISTENER_*_VERSION` constants.
- `include/dcc/app/context.h` now owns its focused context/wait/binding types
  and directly includes only their dependencies. It no longer imports the
  transition-only legacy App aggregate; `legacy.h` imports the focused header.

### TDD evidence

- RED concurrency test: `ctest --test-dir build-v2-api --output-on-failure -R
  '^dcc_app_v2_listener_concurrency_smoke$'` failed deterministically with
  `kind 1 cleanup raced dispatch: early=1 cleanup=1 handler=1` before lifetime
  synchronization was implemented.
- RED contract test: `ctest --test-dir build-v2-api --output-on-failure -R
  '^dcc_app_v2_listener_contract_smoke$'` failed with `non-boolean guild_only
  was not rejected without mutation (status=0)` before fail-closed validation.
- RED header probe: building `dcc_app_v2_context_header_smoke` failed at its
  `#error` because `<dcc/app/context.h>` exposed `DCC_APP_LEGACY_H` before the
  focused header split.
- GREEN coverage now includes deterministic external unlisten for route,
  event, and task handlers; self-unlisten; once-only dispatch; App destruction;
  exact route/listener/schedule unlinking; task cancellation; route- and
  schema-stage registration rollback/retry; ready/message/message-command;
  prefix components; subcommands and subcommand autocomplete; and owned
  stack/heap metadata after source overwrite/free.

### Verification and exact results

- Implementation commit: `7f951a2` (`fix: harden App listener lifecycle and
  validation`). The original `2ea51ea` commit was not rewritten.
- Full configured build: `cmake --build build-v2-api -j4` passed, including
  tests, examples, and tools.
- Full CTest: `ctest --test-dir build-v2-api --output-on-failure -j4` passed
  `174/174` enabled tests in 21.71s. The configured 24 LLAM subdirectory tests
  remained disabled.
- Focused CTest: the original listener, concurrency, contract, and context
  header tests passed `4/4` in 0.41s.
- ASan: a rebuild with `-fsanitize=address -fno-omit-frame-pointer` followed by
  `ASAN_OPTIONS=halt_on_error=1 ctest --test-dir build-v2-api-asan
  --output-on-failure -R '^dcc_app_v2_(listener_concurrency_smoke|listener_contract_smoke|context_header_smoke|listener_smoke)$'`
  passed `4/4` in 0.62s with no report. LeakSanitizer's `detect_leaks` mode is
  unsupported by the installed macOS ASan runtime, so that optional mode was
  omitted after the runtime rejected it before test execution.
- TSan: a rebuild with `-fsanitize=thread -fno-omit-frame-pointer` followed by
  `TSAN_OPTIONS=halt_on_error=1 ctest --test-dir build-v2-api-tsan
  --output-on-failure -R '^dcc_app_v2_(listener_concurrency_smoke|listener_contract_smoke)$'`
  passed `2/2` in 0.88s with no race report.
- Strict source checks passed with no diagnostics under C11 `-Wall -Wextra
  -Wpedantic -Wconversion -Wshadow -Werror` for the four changed App source
  files. The standalone context-header probe passed under both C11 and C++17
  with `-Wall -Wextra -Wpedantic -Werror`.
- `dcc_v2_surface_audit`, `dcc_public_api_audit`,
  `dcc_project_layout_audit`, `dcc_app_smoke`, and
  `dcc_official_surface_headers_smoke` all passed. `git diff --check` passed.

### Remaining platform note

- The POSIX synchronization and runtime paths were executed by the macOS
  sanitizer runs. The Win32 critical-section/condition-variable path is kept
  compile-time isolated and follows the same lock/wait/wake contract, but this
  round did not have a Windows runner available for execution.
