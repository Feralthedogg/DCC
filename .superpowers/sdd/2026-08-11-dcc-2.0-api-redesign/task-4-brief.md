### Task 4: Unify Async REST Requests

Implement only Task 4 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The exact public
contract is in the “REST Execution and Result Model” section of
`docs/superpowers/specs/2026-08-11-dcc-2.0-api-redesign-design.md`. Treat those
two tracked documents as authoritative and do not edit them.

Work test-first and leave the worktree green. Do not push. Do not edit the
ignored progress ledger. Do not remove the transition-only legacy async/future
surface yet; Task 14 owns removal.

#### Public API to add

Create standalone `include/dcc/rest/request.h`, include it from
`include/dcc/rest.h`, and fully document every `DCC_API` declaration:

```c
#define DCC_REST_CALL_OPTIONS_VERSION 1U
#define DCC_REST_REQUEST_DESC_VERSION 1U

typedef struct dcc_rest_request dcc_rest_request_t;

typedef void (*dcc_rest_result_fn)(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
);

typedef struct dcc_rest_call_options {
    size_t size;
    uint32_t version;
    dcc_rest_priority_t priority;
    dcc_rest_result_fn callback;
    void *user_data;
} dcc_rest_call_options_t;

typedef struct dcc_rest_request_desc {
    size_t size;
    uint32_t version;
    dcc_rest_method_t method;
    const char *path;
    const char *content_type;
    const void *body;
    size_t body_len;
    dcc_rest_call_options_t options;
} dcc_rest_request_desc_t;
```

Provide constant initializer macros and null-safe `void` initializer functions
for both versioned caller-owned types. Descriptor initialization also
initializes its nested call options. Defaults are GET, null payload fields,
normal priority, and no callback.

Add:

```c
dcc_status_t dcc_rest_submit(
    dcc_client_t *client,
    const dcc_rest_request_desc_t *description,
    dcc_rest_request_t **out_request
);
dcc_status_t dcc_rest_request_wait(
    dcc_rest_request_t *request,
    uint32_t timeout_ms,
    const dcc_rest_result_t **out_result
);
dcc_status_t dcc_rest_request_cancel(dcc_rest_request_t *request);
uint8_t dcc_rest_request_completed(const dcc_rest_request_t *request);
void dcc_rest_request_destroy(dcc_rest_request_t *request);
```

Validation requires the complete version-1 prefixes, accepts larger sizes,
rejects unsupported versions, invalid method/priority, null or empty path,
`body_len > 0 && body == NULL`, and `callback == NULL && user_data != NULL`.
Set `*out_request = NULL` before all validation when that output exists. Null
options are an internal endpoint default, but the nested raw descriptor options
must itself be valid.

`dcc_rest_request_wait()` requires non-null request and output, sets
`*out_result = NULL` first, treats zero timeout as infinite, and returns
`DCC_OK` after any terminal result. HTTP/transport outcome is solely in the
result. A timeout returns `DCC_ERR_TIMEOUT` and preserves the handle. Multiple
waiters and cancel concurrent with wait must work. Same-request wait from its
terminal delivery returns `DCC_ERR_STATE` instead of deadlocking. Do not permit
destroy to race another caller operation on the same handle; document this
caller synchronization boundary.

Cancel is nonblocking and idempotent. Null cancel/wait is invalid; completed on
null is false; destroy on null is a no-op. Destroy requests cancellation and
drops the caller reference without waiting or suppressing the callback.

#### Admission and ownership invariants

- A non-OK submission is not an accepted operation. It frees all copied state,
  leaves the output null, and invokes neither result callback nor Task 3 error
  observer.
- `DCC_OK` guarantees exactly one terminal result, including transport failure,
  non-2xx, cancellation, shutdown, and an interceptor that returns `DCC_OK`
  without one valid synchronous response.
- Copy NUL-terminated path/content-type strings and exact `body_len` bytes
  before admission. Binary bodies may contain NUL. No request-data pointer may
  be retained. Only callback/user-data values remain borrowed.
- New submission always goes through the async queue. In particular, never use
  the current synchronous interceptor shortcut in `rest_async_submit.c`.
- A requested output handle owns one caller reference. Accepted queued/active
  work owns one runtime reference. Runtime ownership lasts through callback,
  observers, waiter publication, and any callback-local handle destruction.
- Publish `*out_request` before terminal callback delivery becomes eligible.
  Delivery may race the tail of a successful `dcc_rest_submit()` return.
- If no output handle is requested, the runtime releases the object
  automatically. Callback-only and fire-and-observe requests must not leak.
- A handle remains valid after client destruction: teardown first cancels or
  completes accepted work. Client destruction itself is not required to be
  safe when raced against arbitrary client/request API entry.

#### Terminal ordering and result ownership

Atomically claim one terminal path under the appropriate request/client
synchronization. Construct an owned full-version `dcc_rest_result_t` before
user delivery. Preserve exact body bytes and retain decoded Discord metadata
for the handle lifetime. Then, with no queue/rate-limit/request/observer lock
held:

1. invoke the optional `dcc_rest_result_fn` once;
2. deliver Task 3 error observers once when `dcc_rest_result_status()` is not
   `DCC_OK`;
3. publish completed state and wake every waiter;
4. drop the runtime reference.

This makes successful wait a callback/observer lifetime barrier. The callback
gets a borrowed view valid only during the call; wait gets a borrowed view valid
until handle destruction. Clone retention uses
`dcc_rest_result_clone()`/`dcc_rest_result_free()`. Normal terminal delivery and
exact cancellation while work can be admitted run on an existing runtime
worker. Exact public cancel/destroy never invokes terminal user code on its own
stack. Bulk pending cancellation and teardown may finalize on the thread
performing that operation, so promise no fixed callback thread. Detect
same-request recursive waits with terminal-delivery context. Cancel/destroy
from the callback must be safe.

Refactor `rest_error_observer.c` enough to deliver observers from the same
already-built result rather than observing twice. Preserve the legacy
`dcc_rest_cb` path and callback-before-observer order.

#### Queue, cancel, and worker invariants

Represent queued, active, terminalizing, and completed state so cancellation
and worker completion have one winner. Exact request cancellation must:

- publish cancellation while keeping a pending/retry-queued request owned by
  its queue, then deliver it through an existing runtime worker when work can
  be admitted;
- bypass route, rate-limit, and retry-delay eligibility for a canceled entry
  while preserving priority and concurrency;
- record whether each job actually claimed its route so canceled same-route
  completion cannot release another active request's exclusion;
- restore the exact queue position after spawn failure and retain queue
  ownership during stopping for later bulk teardown;
- atomically mark active cancellation, exchange the fd, unlock, then close it;
- be a no-op success if already requested or terminal;
- never invoke terminal user code on the exact public cancel/destroy stack;
- never free memory still owned by a caller or runtime worker.

Bulk pending cancellation and shutdown must dispatch handle-backed requests
through the same finalizer and may deliver on the bulk-operation or teardown
caller. Admission rollback releases both possible refs and does not finalize.
Preserve route exclusion, priority, retry ordering, firewall, rate limits, and
active-worker ownership. Carry raw body length and content type in the async
job; never recover binary length with `strlen`.

For an interceptor, accept only its first valid synchronous response. Returning
success without a response becomes one `DCC_ERR_RUNTIME` terminal result.
Transient 429 retry attempts never callback or observe. Final 429, transport
failure, cancellation, and shutdown each do so once.

Keep legacy `dcc_rest_request_async*` and `dcc_rest_future_t` ABI/build behavior
available for transition tests. They may share finalization machinery, but the
new handle must not be implemented as the legacy future and no canonical
endpoint may gain a future overload.

#### Required RED/green evidence

Create `tests/rest_v2_request_smoke.c` and register
`dcc_rest_v2_request_smoke`. Before implementation, capture the expected
missing-symbol RED. The green target must cover:

- initializer and invalid metadata/pointer/output-invalidation matrix;
- delayed server/interceptor: submit under 100 ms for 250 ms completion;
- immediate mutation of copied path, content type, and binary body;
- timeout then successful wait; repeated wait; two concurrent waiters;
- pending and active cancellation, cancel/completion race, and idempotence;
- destroy while pending/active and destroy from callback;
- callback exactly once and clone surviving handle/client destruction;
- callback-only auto-release and fire-and-observe failure;
- injected worker-spawn rejection with no callback/observer;
- interceptor success-without-response becomes one runtime completion;
- client teardown completing/canceling a retained handle;
- transient 429-to-2xx versus terminal failure observation.

Use atomics/synchronization in test callbacks; do not introduce test data races.
Do not rely on `wait()` returning before callback delivery—it must return after
delivery.

Run at minimum:

- `dcc_rest_v2_request_smoke` and `dcc_http_smoke`;
- Task 3 error, App lifecycle/reentry, and package-consumer focused targets;
- transition surface audit and installed/public-header checks;
- standalone C11 and C++ compilation of `dcc/rest/request.h`;
- MinGW Windows include-order/compile check;
- focused ASan/UBSan and TSan for wait/cancel/destroy/callback/teardown races;
- the full enabled CTest suite if focused verification is green.

Commit implementation as `feat: unify asynchronous REST requests`. Write a
Task 4 implementer report beside this brief containing the RED evidence,
changed files, exact commands/results, sanitizer/platform evidence, known
pre-existing failures separated from regressions, and final commit hash. Commit
the report separately. Stop for review after both commits.
