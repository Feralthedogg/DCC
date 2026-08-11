# Task 4 review-fix report — nonblocking exact cancellation

## Implementation commit

- `b26369c` — `fix: keep REST cancellation nonblocking`

## Review finding closed

`dcc_rest_request_cancel()` and the implicit cancellation performed by
`dcc_rest_request_destroy()` no longer detach and finalize a pending request on
the caller's stack. Under the REST lock they now mark an exact queued request as
canceled and leave it owned by the client queue. Canceled work bypasses route,
rate-limit, and retry-delay admission and is delivered by the existing LLAM
runtime worker path. No detached thread or auxiliary OS thread was introduced.

The existing terminal claim still arbitrates cancellation against worker
completion and client teardown. Terminal ordering remains callback, observer,
completion publication, and waiter wakeup, exactly once. Active cancellation
continues to close the captured descriptor after releasing the REST lock.

When client stopping has begun, ordinary drain deliberately does not start more
work. The pending canceled request remains queue-owned until the finishing
worker or client teardown detaches the pending list and performs the documented
bulk terminal delivery. Exact cancellation therefore still returns without
running a user callback on its own stack.

## Queue and route invariants

- Canceled queue entries are immediately eligible but still obey the configured
  concurrency limit and priority scan.
- Each active job records whether it actually claimed a route. A canceled job
  that bypasses an already-active same route cannot unmark the original job's
  route when it finishes.
- A failed runtime spawn restores the exact queue position. Exact cancellation
  and worker completion each perform one bounded retry, allowing the existing
  deterministic one-shot rejection to recover without stranding the last
  canceled request. Persistent rejection leaves the request queue-owned for
  wait/teardown recovery.
- Legacy asynchronous REST jobs, bulk cancellation, handle references, and
  post-client-destruction retained-handle behavior continue through their prior
  ownership paths.

Public callback-affinity and cancellation documentation in
`include/dcc/rest/request.h` now states that normal/exact completion while the
runtime accepts work occurs on a runtime worker, exact cancellation never
invokes terminal callbacks on its own stack, and stopping may defer delivery to
bulk teardown.

## Deterministic RED evidence

Before the implementation change, the gated callback and observer fixtures ran
this focused test:

```text
cmake --build build-task4 --target dcc_rest_v2_request_smoke -j2
/opt/homebrew/bin/timeout 20s ./build-task4/dcc_rest_v2_request_smoke
```

Both gates proved terminal user code was entered before the corresponding
cancel/destroy call returned:

```text
nonblocking cancel detail started=1 entered=1 returned=0 status=ok elapsed=2 callback=1 observer=1 stage=2 order=1 result=canceled calls=1
nonblocking destroy detail started=1 entered=1 returned=0 status=ok elapsed=2 callback=1 observer=1 stage=2 order=1 result=canceled calls=1
request queue/handle contract failed
```

The process exited `1`. The correctness assertion is gate ordering, not a
wall-clock threshold, so slow sanitizer or CI scheduling cannot create a false
failure.

## Regression coverage

The canonical `dcc_rest_v2_request_smoke` now additionally verifies:

- exact pending cancel returns before a gated result callback is released;
- pending handle destroy returns before a gated observer is released;
- callback then observer ordering, exactly-once delivery, canceled wait result,
  and no interceptor invocation for either canceled request;
- recovery from a one-shot worker-spawn rejection without inline delivery;
- a canceled same-route job cannot clear the route owned by an original active
  request, and a third same-route request remains blocked;
- cancellation racing client destruction after stopping, including deferred
  pending delivery, teardown waiting for an active worker, and retained handle
  wait/cancel/destroy after client destruction;
- the existing twenty cancel-versus-worker races and legacy queue/handle
  contracts.

Every gate has a bounded cleanup/release path, so a regression reports state
instead of hanging the suite.

## Verification

### Normal build and suite

```text
cmake --build build-task4 -j2
ctest --test-dir build-task4 --output-on-failure
```

Result: `97/97` tests passed, `0` failed, in `90.57s`. This includes the REST
request/HTTP tests, legacy ABI and public-surface audits, install/package
checks, App lifecycle, gateway, voice, cluster, and release/source audits.

The final request fixture was then repeated on the committed source state:

```text
ctest --test-dir build-task4 --output-on-failure \
  --repeat until-fail:10 -R '^dcc_rest_v2_request_smoke$'
```

Result: ten consecutive runs passed in `17.39s`.

### ASan and UBSan

```text
cmake -E env \
  ASAN_OPTIONS=detect_leaks=0:abort_on_error=1:halt_on_error=1 \
  UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_rest_v2_request_smoke|dcc_http_smoke|dcc_app_v2_error_smoke)$'
```

Result: `3/3` passed in `4.82s` with no address or undefined-behavior finding.
Darwin's ASan runtime aborted a separate `detect_leaks=1` invocation because
leak detection is unsupported on this platform; address and undefined-behavior
checks were rerun with leak detection disabled as shown above.

### TSan

```text
cmake -E env TSAN_OPTIONS=halt_on_error=1:exitcode=66:history_size=7 \
  ctest --test-dir build-v2-api-tsan --output-on-failure \
  --repeat until-fail:10 -R '^dcc_rest_v2_request_smoke$'
```

Result: ten consecutive runs passed in `20.21s` with no race report.

### Installed headers, package consumer, and MinGW

The final install completed successfully. Its public request header passed
standalone warning-as-error compilation as both strict C11 and C++17. An
external CMake package consumer configured against that install, built, and
executed with exit `0`.

The same public header and all touched production sources
(`rest_async_drain.c`, `rest_async_queue.c`,
`rest_async_worker_lifecycle.c`, and `rest_request_handle.c`) passed MinGW
x86_64 C11 compilation with `-Wall -Wextra -Werror`.

`git diff --check` and `git diff --cached --check` passed. The authoritative
Task 4 brief, design, plan, and progress ledger were not edited. No push was
performed.
