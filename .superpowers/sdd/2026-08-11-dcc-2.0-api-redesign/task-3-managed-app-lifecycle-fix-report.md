# Task 3 managed App lifecycle fix report

## Scope and commits

- Base: `3b47ce2ce7c9e392da1231aacac6814ce8df1b2c`
- Implementation: `1107eda118c6b98e863acf172405b88905e517c4`
- Scope: reject `dcc_app_wait()` and `dcc_app_destroy()` from every managed
  LLAM task before either function mutates App, client, runtime, or REST
  lifecycle state.

## Root cause

The existing guards rejected App-owned callbacks and REST terminal callbacks,
but an arbitrary public `dcc_task_group_spawn()` task had neither frame. A
managed task could therefore enter App teardown and reach runtime quiescence
while the same scheduler driver was executing that task. The task waited for
the driver, and the driver could not finish until the task returned.

Managed `dcc_app_wait()` did not hang in the same probe, but it re-entered the
already-driven runtime, returned `DCC_ERR_RUNTIME`, and changed App/client
started state. That violated the fail-fast/no-mutation lifecycle contract.

## Implementation

- `dcc_app_destroy(NULL)` still returns `DCC_OK`.
- For a non-NULL App, `dcc_app_destroy()` now checks
  `llam_current_task()` immediately after the NULL case and returns
  `DCC_ERR_STATE` before callback-frame checks, stop, reap, admission close,
  or teardown publication.
- `dcc_app_wait()` performs the same managed-task check immediately after its
  NULL validation and before any App/client lifecycle access or mutation.
- The public lifecycle header documents that every managed LLAM task must
  request stop and leave wait/destruction to the owning unmanaged thread.
- `dcc_client_destroy()` was not changed; its existing managed-task behavior
  remains stop-only with final ownership retained by the unmanaged owner.

## Regression coverage

The new fixture uses only the public task-group API to spawn the offending
managed task. It covers both `dcc_app_wait()` and `dcc_app_destroy()` and
requires:

- exact `DCC_ERR_STATE` in less than 100 ms, plus a 3-second process hard
  timeout so the historical destroy deadlock cannot stall CTest;
- byte-for-value lifecycle snapshots proving no change to App state/cleanup,
  listeners, App task group and schedules, teardown/reap flags, App/client
  started and stopping state, start/wait admission, runtime initialization,
  REST admission/terminal closure, or in-flight REST operation count;
- successful owner REST admission, stop, wait, and destroy after the task;
- exact public task-group result for one completed task and successful group
  destruction;
- exactly one App state cleanup and one intercepted REST callback.

The existing App callback/cleanup and REST terminal callback rejection cases,
plus the existing managed `dcc_client_destroy()` stop-only case, continue to
run in the same focused test.

## RED evidence

Before the implementation, the focused test failed as follows:

```text
managed App wait contract failed: action=3 elapsed=0 unchanged=0 rest=0 stop=0 owner_wait=0 join=0 group=0/0 tasks=1/1/0 destroy=0 cleanup=1 requests=1/1
managed App destroy did not fail fast
```

The wait path returned `DCC_ERR_RUNTIME` and changed lifecycle state. The
destroy case reached the fixture's 3-second hard timeout with exit `124`.
After adding the two entry guards, the same focused test passed.

## Exact verification

### Focused normal gate

```sh
cmake --build build-task3 -j8
ctest --test-dir build-task3 --output-on-failure \
  -R '^(dcc_app_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_app_v2_listener_contract_smoke|dcc_interaction_flow_smoke|dcc_project_layout_audit)$'
```

- `7/7` passed in `2.47s`.

### ASan/UBSan

```sh
cmake --build build-v2-api-asan \
  --target dcc_app_smoke dcc_app_v2_error_smoke \
  dcc_app_v2_listener_smoke dcc_app_v2_listener_concurrency_smoke \
  dcc_app_v2_listener_contract_smoke -j8
ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_app_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_app_v2_listener_contract_smoke)$'
```

- `5/5` passed in `2.54s`; no sanitizer finding.

### TSan focused gate

```sh
cmake --build build-v2-api-tsan \
  --target dcc_app_smoke dcc_app_v2_error_smoke \
  dcc_app_v2_listener_smoke dcc_app_v2_listener_concurrency_smoke \
  dcc_app_v2_listener_contract_smoke -j8
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^(dcc_app_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_app_v2_listener_contract_smoke)$'
```

- `5/5` passed in `3.56s`; no race report in the changed paths.

### Canonical DAVE-enabled full suite

The canonical cache uses `DCC_BUILD_TESTS=ON`, `DCC_WITH_DAVE=ON`, and LLAM
`2.2.0`.

```sh
cmake --build build-task3 -j8
ctest --test-dir build-task3 --output-on-failure
```

- `96/96` passed in `75.41s`.

### Strict MinGW C11 and installed header

```sh
x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -pedantic \
  -Iinclude -Ibuild-task3/generated/include -Isrc \
  -I/Users/feral/Desktop/Programming/LLAM/include \
  -c src/app/app.c -o /tmp/dcc-task3-app-round4.o
x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -pedantic \
  -Iinclude -Ibuild-task3/generated/include -Isrc \
  -I/Users/feral/Desktop/Programming/LLAM/include \
  -c tests/app_windows_include_order_smoke.c \
  -o /tmp/dcc-task3-app-windows-round4.o
cmp include/dcc/app/lifecycle.h \
  build-task3-install/include/dcc/app/lifecycle.h
```

- Both strict cross-compiles and the installed-header comparison exited `0`.

### Installed package consumer

```sh
cmake --install build-task3 --prefix "$PWD/build-task3-install"
cmake -S tests/package_consumer -B build-task3-package-consumer \
  -DCMAKE_PREFIX_PATH="$PWD/build-task3-install" \
  -DDCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM \
  -DDCC_LLAM_LIBRARY="$PWD/build-v2-api/_deps/llam/libllam_runtime.a"
cmake --build build-task3-package-consumer -j8
./build-task3-package-consumer/dcc_package_consumer
```

- Install, configure, build, and executable each exited `0`.

### Patch hygiene

```sh
git diff --check
```

- Passed before the implementation commit.

## Independent review

The bounded independent reviewer reported no Critical, Important, or Minor
findings. In addition to code inspection, it ran the focused regression 30
times and repeated the ASan/UBSan and TSan focused gates successfully.

## Known baseline debt, kept separate

### Retry-After teardown delay

The previously recorded 30-second `Retry-After` wake path can delay teardown
because its cancellation fallback sleeps in the OS. The prior base and Round 3
probe each hit a 3-second timeout. This Round 4 entry-guard patch neither
touches nor claims to resolve that retry scheduler debt.

### Event-waiter TSan race

The repository also has a previously recorded TSan race between the waiter
completion write in `src/events/event_waiter_match.c:89` and the read in
`src/events/event_waiter_runtime.c:74`. Those files are outside this patch.
The failing broad event-wait gate remains separate from the clean focused TSan
gate above.
