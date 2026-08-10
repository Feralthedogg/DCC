# Task 3 teardown and flow re-entry fix report

## Scope and commits

- Base: `a52e1f6d92be6ff20d7e909077991896eda1477f`
- Implementation: `514992b6edba9eead4020aa019a150b5f4bb065c`
- Scope: close the remaining App/client REST teardown lifetime gap and make
  plain `dcc_interaction_flow_t` initial-response admission safe against a
  synchronous interceptor re-entering the same flow.

## Root causes

1. App cleanup detached its error sink and reclaimed App-owned callback data
   before pending/active REST producers were unable to enter a new terminal
   frame. The old REST deinitializer also detached and freed active requests
   even though their LLAM workers still owned them.
2. A plain interaction flow committed its initial-response state only after
   the REST call returned. A synchronous interceptor could therefore re-enter
   reply/defer/modal on the same flow and admit a second initial response.
3. The first implementation closed start/wait lifecycle admission before REST
   admission. A pre-admitted, stalled start could consequently leave REST open
   after teardown was observable. Independent review found this ordering gap.

## Implemented invariants

### App/client teardown

The destruction sequence is now:

1. Mark App teardown visible.
2. Close REST admission as the first client teardown mutation.
3. Close wait/start admission and drain starts already admitted.
4. Stop voice/client producers.
5. Cancel pending REST requests terminally; request cancellation for active
   workers without detaching or freeing worker-owned request objects.
6. Drain sync/interceptor/async submission frames and repeat pending
   cancellation to catch submissions admitted before the close.
7. Request LLAM stop, quiesce its single process-default scheduler driver,
   drain admitted client wait post-processing, and prove this client's active
   and pending REST worker counts are zero.
8. Detach the App error sink, permanently close terminal-frame entry, and wait
   for the terminal in-flight count to reach zero.
9. Deinitialize REST, then reclaim listeners, routes, modules, App state, and
   the client.

Closed sync/raw/interceptor and async REST admission returns a local non-OK
status without calling the legacy callback, App sink, or public observer.
REST close/deinit is idempotent and never steals an active request from its
worker.

`dcc_client_destroy()` is stop-only from every managed LLAM task, including a
terminal callback. The owning unmanaged thread performs the final destroy. If
runtime quiescence unexpectedly fails, the allocation remains alive with new
start, wait, and REST admission closed, and the public contract documents the
owner retry condition.

### Plain flow synchronous re-entry

Current-layout flows use separate `CLAIMED` and `ADMITTED` response bits:

- claim before calling REST or an interceptor;
- commit admission on success;
- clear the claim and retain failure/retry semantics on local admission
  failure;
- reject nested same-flow initial actions with `DCC_ERR_STATE` without
  changing the outer owner's claim.

The historical 56-byte prefix cannot hold the new flag field, so it uses the
existing state as a coarse in-progress sentinel. The compatibility path never
reads or writes beyond that prefix; a poisoned-prefix/canary regression covers
the boundary.

## Regression coverage

- All eight outer/nested reply, defer, ephemeral defer, deferred update, and
  modal combinations relevant to synchronous initial-response re-entry.
- Historical 56-byte prefix with a trailing canary.
- Blocking successful sync terminal callback versus concurrent App destroy.
- Teardown-time sync and async REST rejection with zero late callbacks and
  observers.
- Deterministic synthetic pre-admitted start drain: REST must already reject
  sync and async work while destroy is blocked waiting for that start.
- One real active plus one pending async HTTP request, with a separate peer
  LLAM runtime reference proving App teardown does not depend on final global
  runtime release.
- Managed-task client destroy preserving allocation until owner retry.

## RED evidence

The initial plain-flow regression failed before the claim implementation:

```text
initial reentry claim failed (reply -> defer): outer=0 nested=0 requests=2 callbacks=1 elapsed=0 state=4 sent=1
```

The initial App admission regression failed before REST admission closure:

```text
REST admission remained open during App teardown: sync=0 async=0 requests=3 callbacks=2 observers=2
```

A teardown-time `dcc_client_start()` probe also hung before lifecycle admission
was made permanently closeable because it could reopen the runtime after stop.

Independent review then identified the start-drain ordering window. The new
deterministic regression was run with the old order and failed as follows:

```text
REST opened during start drain: entered=1 waited=1 sync=0 async=0 destroy=0 requests=2 callbacks=2 observers=1
```

After moving REST closure ahead of every teardown wait, the same test passes
with both calls returning `DCC_ERR_STATE` and no delivery.

## Exact verification

### Canonical DAVE-enabled build and full suite

The canonical cache has `DCC_BUILD_TESTS=ON`, `DCC_WITH_DAVE=ON`, and LLAM
`2.2.0`.

```sh
cmake --build build-task3 -j8
ctest --test-dir build-task3 --output-on-failure
```

The first full run was `95/96`: only `dcc_project_layout_audit` failed because
the enlarged `tests/app_v2_error_smoke.c` reached 2003 lines. No runtime test
failed. The new managed-destroy fixture was moved to the existing support
translation unit instead of weakening the audit. The immediate corrected run
was `96/96` in `75.51s`.

After the independent-review ordering fix and its deterministic regression,
the final implementation commit was rebuilt and the same full command passed
`96/96` in `75.05s`. This includes the DAVE backend, source/package, release,
layout, workflow pin, surface, and public API audits.

### Focused normal gate

```sh
cmake --build build-task3 \
  --target dcc_app_v2_error_smoke dcc_interaction_flow_smoke -j8
ctest --test-dir build-task3 --output-on-failure \
  -R '^(dcc_app_v2_error_smoke|dcc_interaction_flow_smoke|dcc_project_layout_audit)$'
```

- `3/3` passed in `1.27s`.

The adjacent lifecycle subset also passed `6/6`:

```sh
ctest --test-dir build-task3 --output-on-failure \
  -R '^(dcc_interaction_flow_layout_smoke|dcc_interaction_server_smoke|dcc_cluster_smoke|dcc_cluster_status_route_smoke|dcc_cluster_gateway_smoke|dcc_hot_reload_health_route_smoke)$'
```

### ASan/UBSan

```sh
cmake --build build-v2-api-asan \
  --target dcc_app_v2_error_smoke dcc_interaction_flow_smoke -j8
ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_app_v2_error_smoke|dcc_interaction_flow_smoke)$'
```

- `2/2` passed in `1.20s`; no sanitizer finding.

### TSan task-focused gate

```sh
cmake --build build-v2-api-tsan \
  --target dcc_app_v2_error_smoke dcc_interaction_flow_smoke -j8
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^(dcc_app_v2_error_smoke|dcc_interaction_flow_smoke)$'
```

- `2/2` passed in `1.60s`; no race report in the changed lifetime/re-entry
  paths.

### MinGW C11 and Windows include order

```sh
x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -pedantic \
  -Iinclude -Ibuild-task3/generated/include -Isrc \
  -I/Users/feral/Desktop/Programming/LLAM/include \
  -c src/runtime/runtime_control.c \
  -o /tmp/dcc-runtime-control-task3.o
x86_64-w64-mingw32-gcc -std=c11 -Wall -Wextra -Werror -pedantic \
  -Iinclude -Ibuild-task3/generated/include -Isrc \
  -I/Users/feral/Desktop/Programming/LLAM/include \
  -c tests/app_windows_include_order_smoke.c \
  -o /tmp/dcc-windows-include-order-task3.o
```

- Both commands exited `0`.

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

- Install, configure, build, and manual executable each exited `0`.

### Patch hygiene

```sh
git diff --check
```

- Passed before the implementation commit and before this report commit.

## Known baseline TSan debt, kept separate

The repository has a pre-existing event-waiter race outside this Task 3 scope.
It reproduces from the base with:

```sh
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^dcc_http_smoke$'
```

- `0/1` passes with one TSan warning: the waiter-completion write in
  `src/events/event_waiter_match.c:89` races the read in
  `src/events/event_waiter_runtime.c:74`.
- This known baseline debt is not in a file changed by this implementation and
  is not counted as a Task 3 sanitizer failure. The focused Task 3 TSan gate
  above is clean.

## Review outcome

The bounded independent review found the REST-before-start-drain ordering issue
described above and no other finding in the requested teardown/re-entry scope.
The ordering issue has a deterministic RED regression, is fixed in the
implementation commit, and passed the final normal, ASan/UBSan, TSan, MinGW,
package-consumer, and complete canonical gates.
