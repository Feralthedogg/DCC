# Task 3 review-fix report — structured error lifetime hardening

## Implementation commit

- `6978469` — `fix: harden structured error completion lifetime`

## Review findings closed

### Terminal callback lifetime

- Every legacy callback, error log callback, App-private sink, and public client
  observer now runs inside a linked thread-local REST terminal frame. The frame
  also holds a per-client in-flight reference until all terminal delivery has
  returned.
- `dcc_client_destroy()` called from one of that client's terminal frames only
  requests stop. The owning thread calls destroy again after callback return to
  perform shutdown and reclamation.
- `dcc_app_destroy()` and `dcc_app_wait()` return `DCC_ERR_STATE` without
  mutation from any REST terminal frame on the App's client. Destruction from a
  different thread waits for copied App-sink callbacks before reclaiming App
  state.
- Successful REST completions do not snapshot or increment the App error sink.
  Error snapshots are taken before the legacy callback and remain valid even
  if that callback clears or replaces observers.
- Observer locks are used only to copy callback/user-data pairs. No DCC lock is
  held across user callbacks. Explicit regression coverage proves observer
  re-registration and a synchronous nested REST request from a terminal
  callback are safe and exactly-once.
- Public lifecycle and callback documentation records the borrowed response
  lifetime and deferred-destroy ownership rule.

### Async spawn-admission rollback

- Async queue removal records the exact priority, predecessor, and successor.
  A worker-spawn or route-admission failure restores a pre-existing request at
  that exact location.
- If the newly submitted request itself cannot start, it is detached and freed
  before a non-OK return. No callback, structured observer, or logger can later
  reference its user data.
- If a different, already-owned request fails to start, the new submission
  remains queued and the submission returns `DCC_OK`, preserving the public
  ownership contract.
- A deterministic one-shot spawn failpoint verifies an unchanged public queue
  snapshot, immediate rejected-user-data release, zero terminal delivery,
  clean shutdown, and a successful next submission.
- Worker lifecycle, retry, wait, and generic drain callers retain restored
  requests; only the admission-aware submit path may reject and free the new
  request.

### Interceptor completion mapping

- A callback response with `status == 0` maps its legacy error to a transport
  error (`NETWORK`, `TIMEOUT`, or `CANCELED`) instead of fabricating a Discord
  HTTP failure.
- A real HTTP status keeps transport status `DCC_OK` and is mapped from the
  HTTP status/body, even when the legacy error field is non-OK.
- Missing callbacks with a non-OK interceptor return produce one terminal
  structured error on synchronous requests. Async interception treats this as
  admission rejection and remains silent.
- Null/truncated responses and foreign-client delivery fail closed on the
  original client. Duplicate callbacks are ignored, so legacy and structured
  completion remain exactly-once.

### Initial interaction response admission

- Flow state now tracks whether an initial reply was actually admitted, rather
  than inferring admission from `FAILED` versus `READY`.
- Reply, defer, ephemeral defer, deferred update, modal, component update,
  autocomplete, and auto-defer set the bit only after a `DCC_OK` admission.
  Local validation, serialization, transport, or async admission failures leave
  it clear, allowing the fixed generic fallback to send one safe response.
- A failed edit/followup after an admitted initial reply or defer cannot trigger
  a second initial fallback. A failure in the generic fallback itself does not
  recurse, duplicate delivery, or expose raw error data.
- Auto-defer uses an intermediate `CLAIMED` state so the timer and handler
  cannot both submit an initial response. A local failure before admission can
  be retried. The autocomplete path has its own RED/GREEN regression to ensure
  it participates in the same claim/commit protocol.
- `initial_response_admitted` was appended inside the prior structure's tail
  padding on the canonical ABI. A compile-time assertion preserves the old
  aligned size (`56` bytes on arm64), and runtime helpers read/write the field
  only when the caller-supplied size covers it. Truncated legacy prefixes retain
  their coarse state-based behavior.

## RED evidence

The review fixtures failed before the fixes in the expected ways:

- `dcc_app_v2_error_smoke`: synchronous client destruction from the terminal
  callback aborted the process, App destruction from a success callback could
  deadlock, and `status=0/error=NETWORK` mapped incorrectly.
- `dcc_interaction_flow_smoke`: a failed initial local submission left the flow
  unable to retry an initial reply.
- `dcc_http_smoke`: the injected worker-spawn failure returned non-OK while the
  request remained queued, violating callback user-data ownership.
- `dcc_app_smoke`: the added auto-defer/autocomplete regression failed because
  autocomplete sent an initial response without committing the auto-defer
  admission state.

After implementation, the focused normal matrix for HTTP, App response state,
structured errors, and interaction flow passed `4/4` in 2.40s.

## Canonical verification

The final source state, after the autocomplete review fix, was rebuilt and
tested with the canonical DAVE-, Opus-, tools-, examples-, and LLAM-enabled
configuration:

```sh
cmake --build build-v2-api -j8
ctest --test-dir build-v2-api --output-on-failure
```

- `175/175` enabled tests passed in 75.63s.
- CTest enumerated 199 tests; the same 24 LLAM subdirectory tests remained
  intentionally disabled.
- The run included the focused HTTP, App, listener, listener-concurrency,
  structured-error, and interaction-flow fixtures; all Sugar fixtures; and the
  V2 surface, public API, project layout, source package, deploy, release, SBOM,
  and workflow audits.
- `git diff --cached --check` passed before the implementation commit.

## Installed-package consumer

```sh
cmake --install build-v2-api --prefix build-task3-install
cmake -S tests/package_consumer -B build-task3-package-consumer \
  -DCMAKE_PREFIX_PATH="$PWD/build-task3-install" \
  -DDCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM \
  -DDCC_LLAM_LIBRARY="$PWD/build-v2-api/_deps/llam/libllam_runtime.a"
cmake --build build-task3-package-consumer -j8
./build-task3-package-consumer/dcc_package_consumer
```

The external consumer executable and hot-reload module built successfully,
and the executable exited `0`.

## Sanitizer verification

The ASan/UBSan build used
`-fsanitize=address,undefined -fno-omit-frame-pointer`.

```sh
ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_http_smoke|dcc_app_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_interaction_flow_smoke)$'
```

Result: `6/6` passed in 3.91s with no sanitizer finding.

The TSan build used `-fsanitize=thread -fno-omit-frame-pointer`.

```sh
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^(dcc_app_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_interaction_flow_smoke)$'
```

Result: the Task 3 focused paths passed `5/5` in 3.05s with no race report.

## Remaining verified debt

`dcc_http_smoke` under TSan still exits `8` after reporting the single known
public event-wait race:

- writer: `src/events/event_waiter_match.c:89`;
- reader: `src/events/event_waiter_runtime.c:74`.

The exact race is reproducible at the Task 3 base commit `1833ed5`, and both
threads originate in `run_public_event_wait_smoke`. The deterministic REST
spawn-admission regression runs before that event-wait test and produces no
additional TSan report. This pre-existing event subsystem issue remains
separate from the structured-error change.

The normal and sanitizer runs exercised macOS/POSIX. Win32 branches compiled as
part of the configured source surface where applicable but were not executed
because no Windows runner was available. No other Task 3 review debt remains.
