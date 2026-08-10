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

## Fix Round 2 — serialized teardown and exact transactions

### Review findings addressed

- Split listener shutdown into serialized detach and finalize phases. External
  unlisten/destruction now wait until source removal and all route middleware
  cleanup complete; cleanup never runs while the App listener lock is held.
- Changed `dcc_app_destroy` to return `dcc_status_t`. App-owned route, event,
  task, middleware, error, and cleanup callback frames reject recursive
  destruction with `DCC_ERR_STATE` before mutation; null/success return
  `DCC_OK`.
- Deep-copied App-global and route middleware arrays under the listener lock
  for each dispatch and retained the canonical listener through the whole
  middleware/handler/error/snapshot lifetime.
- Added prepare/commit/abort command-registry additions and private staging for
  listener, route, and schedule arrays. Every fallible schema copy, capacity
  growth, route policy, schedule allocation, and metadata copy occurs before a
  no-fail commit or restores the exact original pointer/count/capacity/IDs.
- Enforced the listener-kind command matrix: slash, subcommand, and
  autocomplete accept CHAT_INPUT only; user and message context menus require
  USER and MESSAGE respectively; omitted type resolves to CHAT_INPUT. Menu
  builders reject descriptions/options and chat builders require a non-empty
  description.
- Kept fallback pointer/count pairs unpublished until nested copying succeeds,
  and reject allocation-size overflow before validation walks caller arrays.

### TDD evidence

- RED detach barrier: the focused concurrency test reported `listener
  finalized before route detach completed: pre-remove-cleanup=1 ...` before
  detach/finalize completion was serialized.
- RED self-destroy contract: the route/event/task test failed to compile while
  `dcc_app_destroy` still returned `void`; after the status API and callback
  frames, all three return `DCC_ERR_STATE` and leave the App unchanged.
- RED middleware barrier: forcing route middleware reallocation while dispatch
  paused in global middleware terminated with `Bus error`; owned snapshots
  made the same test pass under regular, ASan, and TSan builds.
- RED exact transaction: forced route-policy allocation initially returned
  success and changed listener/route/registry pointers and capacities. The
  completed test now snapshots listener/route/schedule pointers, counts,
  capacities, both next IDs, and registry size/state/entries/count/cap across
  policy OOM, schedule OOM, route-ID exhaustion, schema-copy overflow,
  registry-growth OOM, and partial metadata OOM. Every snapshot is identical
  and failed-listener cleanup remains uncalled.
- Added two-thread once dispatch, event destruction, and a real one-millisecond
  LLAM scheduled worker destruction barrier; each proves one claim/cleanup and
  no cleanup before in-flight work exits.

### Verification and exact results

- Implementation commit: `e42d188` (`fix: close App listener review gaps`).
  Commits `54bb77d` and earlier were not rewritten.
- Full configured build passed. Full CTest passed `174/174` enabled tests in
  75.18s; the configured 24 LLAM subdirectory tests remained disabled.
- Focused listener concurrency and contract CTest passed `2/2` in 1.12s.
- ASan/UBSan focused CTest passed `2/2` in 1.22s with
  `detect_leaks=0`; the installed macOS ASan runtime does not support its leak
  mode. TSan focused CTest passed `2/2` in 1.60s with no race report.
- Strict C11 `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
  -fsyntax-only` checks passed for all changed App/registry sources and both
  focused tests. Standalone `<dcc/app.h>` probes passed as C11 and C++17.
- `dcc_v2_surface_audit`, `dcc_public_api_audit`,
  `dcc_project_layout_audit`, `dcc_release_contract_audit`, and all other
  enabled audits passed. `git diff --check` passed.

### Remaining platform note

- The macOS POSIX paths were exercised under both address/undefined-behavior
  and thread sanitizers. The Win32 synchronization branch remains
  compile-time-isolated and was not executed because no Windows runner was
  available in this workspace.

## Fix Round 3 — implicit schemas, callback-safe stop, and Windows policy

### Review findings addressed

- Builder-less slash, subcommand, root autocomplete, and nested autocomplete
  listeners now require a non-null, non-empty description before any App or
  command-registry mutation. They all stage a valid implicit CHAT_INPUT root
  schema with the correct command name and description. Supplied command
  builders retain the existing exact kind/type/description validation.
- Extended exact-transaction coverage from pre-populated listener, route,
  schedule, and public registry arrays. Null/empty descriptions, public
  registry growth failure, private policy/schedule/metadata/registry
  failpoints, route-ID exhaustion, and oversized metadata all preserve the
  complete snapshot and support a clean retry. One-shot failpoints are proven
  to reset.
- Split App schedule shutdown into a nonblocking request phase and serialized
  owner reap phase. `dcc_app_stop` now only publishes cancellation and issues
  task-group/client/voice stop requests; it is safe and idempotent in every
  App-owned callback. `dcc_app_wait` rejects callback self-wait before mutation,
  while owner wait/destroy joins and destroys the task group exactly once.
- A failed wait/destroy reap restores the exact task-group pointer and leaves
  listener/client/cleanup ownership intact for retry. Destroy does not begin
  partial teardown until reaping succeeds, and App cleanup callbacks retain a
  live client while they call stop.
- Client stop now requests owned voice websocket shutdown without polling or
  joining. The voice registry lock protects each voice-client lifetime while
  the cancellation flag and current-socket abort are published.
- App internals now include the shared Windows policy header before public or
  LLAM headers instead of including `windows.h` directly. A dedicated fixture
  asserts lean/nominmax and winsock2 ordering and compiles under strict MinGW
  C11; it is a build-only target so the configured CTest count remains 174.

### TDD evidence

- RED implicit-schema contract: the focused contract test reported
  `implicit slash changed transaction state: status=0/1 id=3` when a missing
  description was accepted and committed.
- RED callback-safe stop: real one-millisecond scheduled callbacks reported
  `canonical scheduled stop was not callback-safe` with an approximately
  five-second callback stop latency and lost task-group ownership before stop
  and reap were separated.
- RED Windows policy: strict MinGW compilation of the include-order fixture
  failed for missing `WIN32_LEAN_AND_MEAN`, missing `NOMINMAX`, LLAM's platform
  include-order guard, and a winsock2 ordering warning before the shared policy
  header was used.
- GREEN tests exercise real canonical and legacy scheduled callbacks, prompt
  callback stop, preserved owner task-group state, callback-side wait rejection,
  owner reap, repeated/no-schedule stop, cleanup-side stop, and deterministic
  reap failure followed by successful retry with cleanup exactly once.

### Verification and exact results

- Implementation commit: `ea8635d` (`fix: harden App listener transactions and
  shutdown`). Commit `88f9789` and all earlier history were not rewritten.
- Full configured build passed. Full CTest passed `174/174` enabled tests in
  24.61s; the configured 24 LLAM subdirectory tests remained disabled.
- The original listener, concurrency, contract, and context-header focused
  CTest set passed `4/4` in 1.16s.
- ASan/UBSan focused CTest passed `4/4` in 1.39s with
  `ASAN_OPTIONS=halt_on_error=1:detect_leaks=0` and
  `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`, with no report.
- TSan concurrency/contract CTest passed `2/2` in 1.70s with
  `TSAN_OPTIONS=halt_on_error=1`, with no race report.
- Strict native C11 `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
  -fsyntax-only` passed for every changed source and focused test. Standalone
  `<dcc/app.h>` probes passed as strict C11 and C++17.
- Strict MinGW C11 `-Wall -Wextra -Wpedantic -Werror -fsyntax-only` passed for
  `src/app/app.c`, `src/app/app_scheduler.c`, and the Windows include-order
  fixture. The native fixture executable also passed.
- The full run included the V2 surface, public API, project-layout, release
  contract, official-header, and remaining enabled audits; all passed.
  `git diff --check` passed.

### Self-review findings

- Reaper failure is ownership-preserving: neither task-group destroy nor App
  teardown proceeds after a failed join, the pointer is restored under the App
  lock, and the next owner wait/destroy can retry deterministically.
- Late schedule registration cannot spawn against a stopping or reaping App,
  and schedule cancellation flags are reset only when a fresh owner start
  creates the task group.
- The final voice-stop refinement performs no waits while preventing an
  unregister/destroy race around a borrowed voice-client pointer.
