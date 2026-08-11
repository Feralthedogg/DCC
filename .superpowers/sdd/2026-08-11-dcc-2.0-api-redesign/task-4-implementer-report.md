# Task 4 implementer report — unified asynchronous REST requests

## Outcome

Task 4 is implemented in commit
`4674e9f57caef658d0569965ec8c70bb2562525f` (`feat: unify asynchronous REST
requests`). The canonical REST escape hatch now has one versioned submission
descriptor, one optional callback, and one reference-counted request handle for
waiting, cancellation, completion polling, and destruction. The transition
legacy async/future ABI remains present.

## Public contract implemented

- Added standalone `<dcc/rest/request.h>` and included it from `<dcc/rest.h>`.
- Added version-1 `dcc_rest_call_options_t` and `dcc_rest_request_desc_t`, their
  constant initializer macros, and null-safe initializer functions.
- Added opaque `dcc_rest_request_t`, `dcc_rest_result_fn`, `dcc_rest_submit()`,
  `dcc_rest_request_wait()`, `dcc_rest_request_cancel()`,
  `dcc_rest_request_completed()`, and `dcc_rest_request_destroy()`.
- Documented validation, copied versus borrowed inputs, result lifetimes,
  callback ordering and lack of thread affinity, timeout behavior, recursive
  wait rejection, and the caller-side destroy synchronization boundary.
- Full version-1 descriptor/options prefixes are required, larger sizes are
  accepted, and every local rejection clears the output without callback or
  observer delivery.

## Runtime and ownership design

- An accepted request owns one runtime reference and, when an output handle is
  requested, one caller reference. The async job releases the runtime reference
  only after terminal delivery and waiter publication. Callback-local destroy
  therefore cannot free state still used by the finalizer.
- One atomic terminal claim arbitrates worker completion, exact cancellation,
  bulk pending cancellation, and teardown. The winner builds an owned,
  full-version `dcc_rest_result_t`, invokes the result callback, delivers Task 3
  observers from that same result, publishes completion, and wakes all waiters.
- A thread-local delivery stack makes same-request callback/observer waits
  return `DCC_ERR_STATE`; cancel and destroy remain safe during delivery.
- Unmanaged POSIX waiters use a mutex/condition variable and absolute timed
  waits. Managed LLAM tasks and Windows use runtime-aware polling. Multiple
  waiters and cancel concurrent with wait are supported.
- The handle retains exact response bytes and decoded Discord metadata until
  destruction. The allocation-failure fallback is a valid terminal
  `DCC_ERR_NOMEM` result.
- A completed retained handle no longer needs its client, so wait, completed,
  cancel, and destroy remain usable after client teardown.

## Queue, worker, and cancellation integration

- `dcc_rest_submit()` resolves the method and copies the NUL-terminated path,
  optional content type, and exact binary body before admission. It always uses
  the async queue, including with the synchronous test interceptor, and
  publishes the output handle before a worker can run.
- Async jobs now carry `body_len`, `content_type`, and an optional new request
  handle. Legacy async callers still use the same queue and legacy callback
  surface; the new handle is not implemented as `dcc_rest_future_t`.
- The capture path accepts only the first structurally valid synchronous
  response for the expected client and copies its exact body. Success without a
  response, status zero without an error, and response-copy failure all become
  one terminal runtime/transport result.
- Transient 429 attempts requeue without callback or observation. The terminal
  attempt carries retry metadata and is delivered once.
- Exact cancellation performs membership checks under the REST lock before
  dereferencing the job: it detaches only a matching pending/retry request or
  marks a matching active request canceled. Pending completion and fd close
  happen after unlocking. This also avoids a stale-job dereference when bulk
  teardown has already detached a pending list.
- Bulk pending cancellation and shutdown dispatch handle-backed jobs through
  the same finalizer. Admission rollback only frees both possible references;
  it does not finalize or observe.

## TDD evidence

### Initial RED

After registering `tests/rest_v2_request_smoke.c`, this command was run before
the public API existed:

```text
cmake --build build-task4 --target dcc_rest_v2_request_smoke -j8
```

It failed at compile time as expected with:

```text
fatal error: 'dcc/rest/request.h' file not found
```

### Focused behavioral REDs

- Adding the interceptor status-zero case made
  `dcc_rest_v2_request_smoke` fail with `request queue/handle contract failed`.
  Normalizing that malformed completion to `DCC_ERR_RUNTIME` made it green.
- Adding the terminal 429 retry metadata assertion produced the same focused
  test failure. Populating terminal `retry_after_ms` made it green.
- The delayed-submit test initially exposed that LLAM preemption signals can
  interrupt a test `usleep()`. The fixture was corrected to retry `nanosleep()`
  on `EINTR`, so the 250 ms contract measures the implementation rather than
  signal timing.

The final smoke test covers initializers; all invalid metadata/pointer/output
cases; larger versioned sizes; a 250 ms interceptor with sub-100 ms submission;
immediate path/content-type/binary-body mutation; timeout then wait; repeated
and concurrent waits; recursive wait; exact pending/active cancellation;
twenty cancel/completion races; destroy pending/active/from callback;
callback-only and fire-and-observe ownership; admission spawn rejection;
no-response and status-zero interceptors; result clone and Discord metadata;
transient and terminal 429; and retained handles through client teardown.

## Files changed

- `CMakeLists.txt`
- `include/dcc/rest.h`
- `include/dcc/rest/request.h`
- `src/internal/rest/dcc_rest_async_request_internal.h`
- `src/internal/rest/dcc_rest_capture_internal.h`
- `src/internal/rest/dcc_rest_error_observer_internal.h`
- `src/internal/rest/dcc_rest_request_handle_internal.h`
- `src/rest/rest_async_cancel_pending.c`
- `src/rest/rest_async_request.c`
- `src/rest/rest_async_request_new.c`
- `src/rest/rest_async_submit.c`
- `src/rest/rest_async_worker.c`
- `src/rest/rest_capture.c`
- `src/rest/rest_error_observer.c`
- `src/rest/rest_request_handle.c`
- `src/rest/rest_submit.c`
- `tests/rest_v2_request_smoke.c`

The authoritative plan, design, and ignored progress ledger were not edited.

## Verification and exact results

### Normal build and focused tests

```text
cmake -S . -B build-task4 -DDCC_BUILD_TESTS=ON
cmake --build build-task4 -j8
```

Both completed successfully. A focused run covering the transition audit,
package consumer, new REST request test, existing HTTP suite, official surface,
Task 3/App lifecycle and error tests, interaction flow, and surface audit passed
11/11 in 25.62 seconds.

### Full enabled suite

```text
ctest --test-dir build-task4 --output-on-failure
```

Final post-documentation run: 97/97 passed, 0 failed, in 76.58 seconds. This
includes `dcc_rest_v2_request_smoke` (1.37 s), `dcc_http_smoke` (1.72 s), legacy
ABI layout, transition v2 surface audit, official/public REST audits, App,
gateway, voice, cluster, hot-reload, package, and source/release audits.

### ASan and UBSan

The sanitizer tree was configured with:

```text
cmake -S . -B build-v2-api-asan -DDCC_BUILD_TESTS=ON \
  -DDCC_ENABLE_SANITIZERS=ON \
  -DCMAKE_C_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined' \
  -DCMAKE_SHARED_LINKER_FLAGS='-fsanitize=address,undefined'
```

The broad focused run passed 7/7 (`dcc_rest_v2_request_smoke`, HTTP, App,
listener, App error, interaction flow, and pkg-config consumer) in 4.71 seconds.
The final focused rerun was:

```text
cmake -E env \
  ASAN_OPTIONS=detect_leaks=0:abort_on_error=1:halt_on_error=1 \
  UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_rest_v2_request_smoke|dcc_http_smoke|dcc_app_v2_error_smoke)$'
```

Result: 3/3 passed, 0 failed, in 4.51 seconds. Darwin's ASan runtime aborts
when `detect_leaks=1` is requested with `detect_leaks is not supported on this
platform`; leak detection was therefore disabled while address and undefined
behavior checks remained enabled.

### TSan

The TSan tree was configured with:

```text
cmake -S . -B build-v2-api-tsan -DDCC_BUILD_TESTS=ON \
  -DDCC_ENABLE_SANITIZERS=OFF \
  -DCMAKE_C_FLAGS='-fsanitize=thread -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=thread' \
  -DCMAKE_SHARED_LINKER_FLAGS='-fsanitize=thread'
```

```text
cmake -E env TSAN_OPTIONS=halt_on_error=1:exitcode=66:history_size=7 \
  ctest --test-dir build-v2-api-tsan --output-on-failure \
  --repeat until-fail:10 -R '^dcc_rest_v2_request_smoke$'
```

Result: ten consecutive request race runs passed (10/10, 16.40 s). Task 3/App
focused TSan tests passed 4/4 in 2.22 seconds. After the final public-header
documentation edit, the new request test was rebuilt and repeated three more
times; 3/3 passed in 4.95 seconds.

### Installed package and public headers

```text
cmake --install build-task4 --prefix build-task4-install-final
```

Install succeeded and included `include/dcc/rest/request.h`. The installed
header passed standalone warning-as-error compilation:

```text
cc -std=c11 -pedantic-errors -Wall -Wextra -Werror \
  -I build-task4-install-final/include \
  -include dcc/rest/request.h -fsyntax-only -x c /dev/null

c++ -std=c++17 -pedantic-errors -Wall -Wextra -Werror \
  -I build-task4-install-final/include \
  -include dcc/rest/request.h -fsyntax-only -x c++ /dev/null
```

Both exited 0. The installed CMake consumer was configured with the install
prefix plus the LLAM root/library, built, and executed successfully:

```text
cmake -S tests/package_consumer -B build-task4-package-consumer-final \
  -DCMAKE_PREFIX_PATH=/Users/feral/Desktop/Programming/DCC/.worktrees/v2-api-impl/build-task4-install-final \
  -DDCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM \
  -DDCC_LLAM_LIBRARY=/Users/feral/Desktop/Programming/LLAM/libllam_runtime.a
cmake --build build-task4-package-consumer-final -j8
./build-task4-package-consumer-final/dcc_package_consumer
```

All three exited 0. The installed pkg-config metadata returned the expected
include/library paths, and `tests/pkg_config_consumer/main.c` linked and ran
against the installed archive successfully.

### Windows cross-compile

The standalone public header passed MinGW warning-as-error compilation:

```text
/opt/homebrew/bin/x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror \
  -Iinclude -Ibuild-task4/generated/include \
  -include dcc/rest/request.h -fsyntax-only -x c /dev/null
```

With `-Iinclude -Isrc -Ibuild-task4/generated/include` and the LLAM include
directory, MinGW also compiled these sources/fixtures with `-Wall -Wextra
-Werror`:

- `src/rest/rest_request_handle.c`
- `src/rest/rest_submit.c`
- `src/rest/rest_async_worker.c`
- `tests/app_windows_include_order_smoke.c`

All exited 0.

### Symbols and transition ABI

`nm -g build-task4/libdcc.a` found all seven new exported functions and also
confirmed the transition symbols `dcc_rest_request_async`,
`dcc_rest_request_async_priority`, `dcc_rest_future_create`,
`dcc_rest_future_wait`, and `dcc_rest_future_destroy`. The registered
`dcc_v2_surface_audit` and `dcc_legacy_abi_layout_smoke` both passed in the full
suite.

`git diff --check` and `git diff --cached --check` passed before the
implementation commit.

## Known non-regression findings

- `dcc_http_smoke` under TSan still reports the pre-existing event-waiter stack
  race between `dcc_event_bus_signal_waiters_locked` and
  `dcc_event_waiter_wait_channel` (the existing event waiter match/runtime
  paths). No Task 4 file appears in either access stack. The same HTTP test
  passes normally and under ASan/UBSan; the Task 4 request race test and all
  Task 3/App TSan targets are clean.
- LeakSanitizer is unsupported by the Darwin ASan runtime used here, as noted
  above. This is a platform limitation, not a DCC test failure.

No Task 4 regression or unresolved Task 4 failure remains.

## Commit

- `4674e9f57caef658d0569965ec8c70bb2562525f feat: unify asynchronous REST requests`
