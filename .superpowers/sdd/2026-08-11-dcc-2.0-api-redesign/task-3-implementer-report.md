# Task 3 implementer report — structured observable errors

## Implementation commit

- `fd39096` — `feat: add structured observable errors`

## What I implemented

- Added the size/versioned public `dcc_error_t` and `dcc_rest_result_t`
  contracts, including result status, clone, and free helpers.
- Added owner-typed `dcc_client_error_fn` and `dcc_app_error_fn` callbacks.
  `(NULL, NULL)` clears either public observer, while a null callback with
  non-null user data is rejected.
- Kept the public client observer and App-owned private sink in separate slots.
  Registration and snapshotting use a dedicated client observer lock; user
  callbacks, legacy request callbacks, and logging run after locks are released.
- Added one terminal REST completion path shared by synchronous requests,
  interceptors, asynchronous workers, pending/active cancellation, retry
  exhaustion, and shutdown drain.
- Deferred asynchronous observation until the retry/requeue decision is final,
  so transient 429 responses are not reported and terminal failures are
  reported exactly once even without a per-request callback.
- Decoded Discord `code` and `message` through the bounded JSON parser using
  the supplied body length. Raw response bytes are never treated as a C string.
- Replaced the temporary context-shaped App error callback with the structured
  App-owned observer. Route, event, and scheduled-task failures are delivered
  before their borrowed operation/context view expires.
- Separated telemetry from end-user behavior. The opt-in default policy sends
  exactly one fixed generic ephemeral response only while the interaction is
  still ready; an already-started response is left untouched.
- Migrated the App, Sugar, and installed-package fixtures from the old callback
  signature without changing the legacy per-request REST response contract.

## TDD evidence

- RED: the initial `dcc_app_v2_error_smoke` fixture failed to compile because
  `<dcc/error_details.h>`, `<dcc/rest/result.h>`, the structured types, and the
  result/observer functions did not exist. Its owner-typed App callback was also
  incompatible with the temporary context/status/message observer.
- GREEN: the focused fixture now covers result mapping and ownership, exact
  body-byte cloning with an embedded NUL, original-buffer mutation and free,
  invalid size/version and output invalidation, and retry metadata retention.
- The terminal matrix covers sync 400/500, callback plus observer delivery,
  intercepted response/transport failures, real async transient and terminal
  429 responses, pending and active cancellation, shutdown drain, and the rule
  that validation/admission rejection is not a completion.
- JSON coverage separately exercises malformed, truncated, non-object, and
  valid non-NUL-terminated spans without out-of-bounds reads or fabricated
  Discord fields.
- App coverage includes route, event, and real scheduled-task failures, exact
  generic response text, already-responded behavior, coexistence of the public
  client observer and private App sink, and self-replacement/self-clear.
- A deterministic lifetime test blocks inside an App observer reached through
  the private REST sink, replaces and clears the App observer while the copied
  callback is in flight, starts `dcc_app_destroy` on another thread, proves
  teardown waits, then releases the callback and verifies clean completion.

## Canonical build and test evidence

The canonical `build-v2-api` configuration had tests, examples, tools, DAVE,
Opus, and the LLAM subdirectory enabled. LLAM subdirectory tests were
intentionally disabled by `DCC_LLAM_ENABLE_SUBDIRECTORY_TESTS=OFF`.

- `cmake --build build-v2-api -j4`: passed, including tools and examples.
- `ctest --test-dir build-v2-api --output-on-failure`: `175/175` enabled tests
  passed in 75.54s; 199 tests were enumerated and 24 LLAM tests were disabled.
- The full run included `dcc_app_v2_error_smoke`, `dcc_http_smoke`, the App
  listener/concurrency suites, `dcc_v2_surface_audit`,
  `dcc_public_api_audit`, project-layout/source-package/release audits, and all
  other enabled tests.
- The V2 transition audit compiled standalone public headers as C11 and C++17;
  it passed in 21.93s. The public API/symbol/header audit passed in 14.71s.
- `python3 tools/audit_project_layout.py .`: passed.
- The staged implementation passed `git diff --cached --check`.

An earlier exploratory `build-task3` directory had DAVE disabled, so its
configuration-incompatible fake-DAVE test was not treated as release evidence.
The canonical DAVE-enabled build above passed that test and the full matrix.

## Installed-package consumer

- Installed the canonical build to `build-task3-install`.
- Configured `tests/package_consumer` in an independent build directory using
  only the installed DCC package plus the configured LLAM root/library.
- The package consumer executable and hot-reload module built successfully.
- `./build-task3-package-consumer/dcc_package_consumer`: exited `0`.

This verifies that the new headers and exported result/observer symbols are
present and usable from the installed CMake package, not only the source tree.

## Sanitizer evidence

### ASan/UBSan

The focused build used
`-fsanitize=address,undefined -fno-omit-frame-pointer`.

```text
dcc_http_smoke                           PASS
dcc_app_v2_listener_smoke                PASS
dcc_app_v2_error_smoke                   PASS
dcc_app_v2_listener_concurrency_smoke    PASS
```

Result: `4/4` passed in 3.73s with no sanitizer report.

### Task 3 TSan paths

The focused build used `-fsanitize=thread -fno-omit-frame-pointer`.

```text
dcc_app_v2_listener_smoke                PASS
dcc_app_v2_error_smoke                   PASS
dcc_app_v2_listener_concurrency_smoke    PASS
```

Result: `3/3` passed in 2.70s with no race report. The error test includes
observer replacement/clear, async completion/cancel/shutdown, and the private
App sink versus concurrent App destruction lifetime barrier.

## Reproduced pre-existing TSan debt

Running `dcc_http_smoke` under TSan reports an event-waiter race outside the
Task 3 changes. To distinguish it from this implementation, I created a fresh
detached worktree at the exact Task 3 base, `1833ed5`, and configured it with
the same compiler, optimization, sanitizer, and feature settings:

```text
AppleClang 21.0.0.21000101
CMAKE_BUILD_TYPE=Release
CMAKE_C_FLAGS=-fsanitize=thread -fno-omit-frame-pointer
CMAKE_EXE_LINKER_FLAGS=-fsanitize=thread
DCC_BUILD_TESTS=ON
DCC_BUILD_EXAMPLES=OFF
DCC_BUILD_TOOLS=OFF
DCC_WITH_DAVE=ON
DCC_WITH_OPUS=ON
DCC_LLAM_USE_SUBDIRECTORY=ON
```

The baseline commands were:

```sh
cmake -S . -B build-task3-tsan-baseline \
  -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=/usr/bin/cc \
  '-DCMAKE_C_FLAGS=-fsanitize=thread -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=thread \
  -DDCC_BUILD_TESTS=ON -DDCC_BUILD_EXAMPLES=OFF -DDCC_BUILD_TOOLS=OFF \
  -DDCC_ENABLE_SANITIZERS=OFF -DDCC_WITH_DAVE=ON -DDCC_WITH_OPUS=ON \
  -DDCC_LLAM_USE_SUBDIRECTORY=ON \
  -DDCC_LLAM_ENABLE_SUBDIRECTORY_TESTS=OFF \
  -DDCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM
cmake --build build-task3-tsan-baseline --target dcc_http_smoke -j4
ctest --test-dir build-task3-tsan-baseline \
  --output-on-failure -R '^dcc_http_smoke$'
```

The unchanged base reproduced the same failure (`0/1`, CTest exit `8`):

- writer: `src/events/event_waiter_match.c:89`,
  `waiter->completed = 1`, in `dcc_event_bus_signal_waiters_locked`;
- reader: `src/events/event_waiter_runtime.c:74`,
  `if (waiter->completed)`, in `dcc_event_waiter_wait_channel`;
- both threads originate in `run_public_event_wait_smoke`.

This is final-release sanitizer debt in the pre-existing public event-wait
path. It was deliberately not mixed into the structured-error implementation.
The Task 3-specific TSan matrix above is clean.

## Self-review findings

- Observer pairs are copied under their own lock and invoked after unlock.
  App private-sink snapshots increment an in-flight count while still under
  that lock, so detach cannot miss a callback that already copied App state.
- App destruction clears the private sink and waits before reclaiming App
  listeners, observer user data, or the client. Reentrant App destruction from
  the observer remains rejected by the existing callback-frame contract.
- Every completion owns or borrows its operation and body through the entire
  callback sequence. Async request storage and captured HTTP bodies are freed
  only after legacy and structured delivery returns.
- Pending-list cancellation and shutdown drain detach lists under the REST
  lock, then invoke callbacks and logging after unlock.
- Synchronous raw retries observe only the final attempt. Async raw attempts
  never observe directly; the worker reports only after retry exhaustion,
  cancellation, or final success/failure is known.
- Result helpers validate the size/version prefix before reading covered
  fields. Clone failure always nulls the output, checks allocation overflow,
  deep-copies the decoded message, and preserves exact body bytes.
- The generic end-user response is the fixed text
  `Something went wrong. Please try again.` and never includes status text,
  operation, token, Discord body, or transport details.

## Platform note

The normal, ASan/UBSan, and TSan executions covered the POSIX/macOS paths. The
Win32 branches remain compile-time isolated but were not executed because this
workspace did not provide a Windows runner.

## Files of interest

- `include/dcc/error_details.h`
- `include/dcc/rest/result.h`
- `include/dcc/client.h`
- `include/dcc/app/base.h`
- `src/rest/rest_result.c`
- `src/rest/rest_error_observer.c`
- `src/rest/rest_request.c`
- `src/rest/rest_async_worker.c`
- `src/rest/rest_lifecycle.c`
- `src/app/app_error.c`
- `tests/app_v2_error_smoke.c`
