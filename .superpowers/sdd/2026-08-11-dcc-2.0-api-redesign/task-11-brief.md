# Task 11 brief — ordered interaction action queues

Implement only Task 11 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The
`Ordered Interaction Flow` section of the approved design, the final Task 10
tree, and this brief are authoritative. Work test-first, do not push, and do
not edit the ignored progress ledger. Do not begin Components v2, Bot-header,
legacy-removal, or release work from Tasks 12–15.

## Hard prerequisite and starting point

Task 11 starts only after Task 10 is complete on this implementation branch.
Before writing RED tests, verify all of the following:

- the branch contains the reviewed Task 10 implementation and reports;
- `tools/audit_rest_v2_endpoints.py` passes in strict mode with no progress
  allowance and reports the final 224 active endpoints plus 60 generic REST
  operations;
- all endpoint-mirroring public `dcc_app_*` REST declarations and
  `src/app/app_rest_shortcuts*.c` units are gone;
- the Task 6 interaction endpoints still have their one canonical typed
  signatures ending in `options, out_request`;
- the worktree is clean apart from the Task 11 RED fixture being written.

If Task 10 is not complete, stop. Do not implement this task on the current
Task 6 snapshot and do not recreate an App REST shortcut to bridge the gap.
Paths below name the expected post-Task-10 responsibilities; resolve line
numbers against that final tree.

## Outcome

Every `dcc_ctx_*` and `dcc_flow_*` interaction response action serializes its
borrowed input during the call, appends one fully owned action to a
per-interaction FIFO, and returns without waiting for HTTP completion. Only the
head action is admitted to REST. A later action is not submitted until the
preceding action's callback and error observers have returned.

The queue is keyed by interaction identity, survives the callback-scoped
`dcc_ctx_t`, and owns the interaction ID, application ID, escaped request data,
token, projected response state, and every prepared request. A second
state-aware reply accepted behind an initial reply is prepared as a followup,
but is not put on the network until the initial response reaches a successful
2xx terminal result. A reply accepted after a queued defer becomes an edit of
the original response.

This task also makes auto-defer use the same queue mutex as explicit actions,
adds honest queued public states, gives standalone flow objects an explicit
`deinit`, and drains queues before App-owned callback data or the App error sink
can be reclaimed.

## What Tasks 4, 6, and 10 already solved

Do not re-solve or weaken these contracts:

| Existing task | Already guaranteed | Task 11 may add |
| --- | --- | --- |
| Task 4 | request handle, copied raw descriptor data, nonblocking admission, exactly one terminal result, cancellation, callback-before-observer order, callback/observer delivery before successful wait | one private post-terminal hook and an internal take-owned admission path; no public signature or ordering regression |
| Task 6 | typed interaction payload validation, exact JSON/multipart serialization, path/query escaping, input copying before return, and the eight canonical interaction endpoints | a private prepare-without-submit seam shared by the existing public wrappers and the queue |
| Task 10 | all remaining REST domains migrated, strict endpoint manifest, composite consumers made asynchronous, App endpoint mirrors removed | context-local response queuing only; no endpoint aliases or App REST mirrors |

Task 11 is the cross-request sequencing layer. It must not add `_v2`, async,
future, builder, raw JSON, or callback endpoint overloads; change any public
Task 6 endpoint signature; expose the private queue through the REST API; or
make a worker call `dcc_rest_request_wait()`.

Ordering is promised only among actions submitted through the same App context
queue or the same standalone `dcc_interaction_flow_t`. Direct calls to
`dcc_rest_interaction_*` remain the low-level escape hatch. Mixing direct REST
callbacks with a flow/context for the same interaction is caller-managed and
must be documented as outside the ordered-flow guarantee.

## File boundaries

Create at minimum:

- `src/internal/app/dcc_app_interaction_queue_internal.h` — queue, action,
  registry, refcount, transition, and private App/flow integration contracts;
- `src/app/app_interaction_queue.c` — queue/registry construction, lookup,
  retirement, ownership, state queries, close, and drain;
- `src/app/app_interaction_queue_worker.c` — FIFO pump, REST admission,
  terminal commit, synthetic completion, failure cascade, and reentry guard;
- `src/internal/rest/dcc_rest_interaction_prepared_internal.h` — fully owned
  prepared interaction request and the four prepare operations used here;
- `src/rest/rest_interaction_prepared.c` — common owned-buffer lifecycle and
  private prepared-request submission support;
- `tests/app_v2_interaction_queue_smoke.c` — the primary ordered-flow RED/green
  fixture.

Modify as required:

- `include/dcc/app/context.h` for canonical context action/state ownership
  documentation in the post-Task-10 header layout;
- `include/dcc/interaction_flow.h` for queued states, the opaque tail field,
  and `dcc_flow_deinit()`;
- transition-only `include/dcc/sugar/flow.h` so every in-tree named flow is
  deinitialized; do not broaden this into Task 14 Sugar cleanup;
- `src/internal/app/dcc_app_internal.h`, `src/app/app.c`,
  `src/app/app_routes.c`, `src/app/app_context.c`, and
  `src/app/app_auto_defer.c` for App registry, context attachment, timer, and
  teardown integration;
- `src/internal/interaction_flow/dcc_interaction_flow_internal.h` and the
  `src/interaction_flow/interaction_flow_{core,messages,defer,modal,auto_defer}.c`
  units so standalone flow uses the same queue engine;
- `src/internal/rest/dcc_rest_request_handle_internal.h`,
  `src/internal/rest/dcc_rest_error_observer_internal.h`,
  `src/internal/rest/dcc_rest_async_request_internal.h`,
  `src/rest/rest_request_handle.c`, `src/rest/rest_error_observer.c`,
  `src/rest/rest_submit.c`, and `src/rest/rest_async_request_new.c` for the
  private post hook and take-owned admission;
- the Task 6 initial-response, original-edit/delete, and followup-create
  implementation units so their public wrappers share the private prepare
  path;
- `tests/interaction_flow_smoke.c`, the flow layout C++ fixture, App lifecycle
  fixtures, package/header probes, and `CMakeLists.txt`.

Keep files focused. There is no permanent thread or LLAM task per interaction
queue. The pump runs on the action-admission caller, REST terminal delivery,
or the single auto-defer timer task. If a support fixture would push
`app_v2_interaction_queue_smoke.c` beyond the repository translation-unit size
limit, split only the server/barrier/failpoint support into `tests/support/`.

## Public states and ABI

Preserve every existing numeric value and append exactly these values:

```c
typedef enum dcc_interaction_flow_state {
    DCC_INTERACTION_FLOW_READY = 0,
    DCC_INTERACTION_FLOW_DEFERRED = 1,
    DCC_INTERACTION_FLOW_DEFERRED_EPHEMERAL = 2,
    DCC_INTERACTION_FLOW_DEFERRED_UPDATE = 3,
    DCC_INTERACTION_FLOW_REPLIED = 4,
    DCC_INTERACTION_FLOW_ORIGINAL_EDITED = 5,
    DCC_INTERACTION_FLOW_FOLLOWED_UP = 6,
    DCC_INTERACTION_FLOW_MODAL = 7,
    DCC_INTERACTION_FLOW_FAILED = 8,
    DCC_INTERACTION_FLOW_INITIAL_QUEUED = 9,
    DCC_INTERACTION_FLOW_DEFERRED_QUEUED = 10
} dcc_interaction_flow_state_t;
```

`dcc_flow_state_string()` returns `"initial_queued"` and
`"deferred_queued"` for the new values. Do not insert the values beside their
semantic neighbors and renumber the historical enum.

Append exactly one `void *internal_queue` queue-state pointer at the end of
`dcc_interaction_flow_t`; do not repurpose `reserved`, `response_flags`, or old
tail padding. Add:

```c
DCC_API void dcc_flow_deinit(dcc_interaction_flow_t *flow);
```

The member is opaque to callers and its ownership comment is mandatory.
`dcc_flow_init()` initializes the full current layout without
retaining caller input beyond the normal pre-action lifetime. Queue creation
may be lazy so the existing `void` initializer remains valid; its first action
must report allocation failure without mutating the flow.

A current-layout flow becomes non-copyable once its private pointer is
attached. It must be deinitialized before its storage is reused or leaves
scope; null and repeated deinit calls are safe no-ops. The first current-layout
deinit detaches `internal_queue`, drops only the caller/flow owner
reference, clears borrowed context pointers, sets the state snapshot to
`FAILED`, and sets `size` to zero as the deinitialized marker. It does not
cancel an action that already returned `DCC_OK`, suppress its callback, or
shorten callback `user_data` lifetime. Reinitialization through
`dcc_flow_init()` is permitted only after that deinit.

For a historical prefix whose declared `size` does not cover the new pointer:

- `dcc_flow_state()` and state-string inspection retain the bounded old-state
  behavior;
- `dcc_flow_deinit()` is a bounded no-op;
- every action that would enqueue work returns `DCC_ERR_STATE`, invokes no
  callback or observer, performs no network request, and leaves all covered
  bytes and following canaries unchanged.

Do not hide this with an old coarse optimistic path: a historical object cannot
hold the owner reference required for safe asynchronous sequencing.

The public `.state` field is a compatibility snapshot. Once a queue exists,
only `dcc_flow_state()` and `dcc_ctx_response_state()` are synchronized state
authorities; direct concurrent field reads have no terminal-completion
guarantee. Document this in both headers.

## Queue identity, registry, and ownership

### App registry

Each App owns a private collision-safe hash registry. Its exact key is the pair
`(interaction_id, interaction_token bytes)`. The application ID and client
must also match the queue found under that key; a mismatch is rejected rather
than silently sharing state. Copy the token once when creating the queue and
never retain `dcc_interaction_t *` or any event/parser view.

Registry lookup must be average O(1), use checked capacity arithmetic, and
compare full key bytes after hashing. A growth/allocation failure leaves the
registry unchanged and returns `DCC_ERR_NOMEM`. Two concurrent deliveries of
the same interaction acquire the same queue, so they cannot emit two initial
responses. Distinct token bytes remain distinct even if IDs collide.

The registry owns one strong queue reference. Acquisition increments the
strong reference while holding the registry mutex, before publishing it to the
context. Normal retirement occurs only when that registry reference is the
sole remaining reference. Under the registry mutex, either a new acquire
retains the existing entry first or retirement unlinks it first; it must never
be possible for an active old queue and a new registry queue for the same key
to overlap.

Use a separate live-queue count/condition for App teardown. Closing the
registry detaches its entries and prevents new acquisition, but the live count
reaches zero only after context owners, timer tasks, active terminal hooks, and
queued work have released their references.

Registry close takes one temporary drain reference for every entry before
unlinking it under the registry mutex. After unlocking, it closes/cancels each
queue, drops the old registry reference, then drops the drain reference. It
must never walk or call into a queue through a pointer whose registry reference
was already released.

### Strong references

Use an atomic refcount, but do not use atomics as a second response-state
machine. Account explicitly for:

- one App-registry reference while discoverable;
- one owner reference for each attached App context or standalone flow;
- one work reference for a nonempty/active queue epoch;
- one reference for an armed/running auto-defer timer;
- short-lived pump/terminal/retirement references.

The queue owns all linked action nodes. Each node owns its prepared request and
its callback function/user-data values; only the values are copied, not the
object behind `user_data`. Free a queue only after it is unlinked and the last
strong reference is released. Queue destruction decrements the owning App's
live-queue count before the App's registry synchronization primitives are
destroyed.

This refcount shape also supplies a lock-safe retirement rule: a release that
leaves only the registry reference attempts unlink under the registry mutex.
No queue mutex is needed while the registry mutex is held.

### Standalone flows

A standalone flow creates one unregistered queue on first mutating action and
keeps one owner reference until `dcc_flow_deinit()`. Two separately initialized
flow objects do not share state even when they point at the same interaction;
callers that need ordering must reuse one flow. App-created flows are attached
to the App registry before middleware/handler execution and never create a
second standalone queue.

Because `dcc_flow_init()` cannot report allocation, a standalone flow's first
lazy mutating action is caller-serialized. Once `internal_queue` is attached,
action/state calls may come from multiple producers and are serialized by its
queue mutex. The caller must serialize `dcc_flow_deinit()` and reinitialization
against other entries on the same flow object; accepted terminal callbacks may
continue afterward from their independent queue references.

The client must outlive the standalone flow owner. Deinitialize the flow before
final client destruction; accepted work is still allowed to finish/cancel
during the client's Task 4 drain. Keeping the caller-owned flow and invoking a
new action after its client has been destroyed is invalid caller behavior.

## Authoritative state model

One queue mutex owns all mutable response state and list topology. Maintain two
different stable states:

- `committed_state`: the state after the last successful 2xx terminal action;
- `projected_state`: the tail state assuming every currently accepted action
  succeeds in FIFO order.

Also maintain private projected/committed `original_exists` flags and an
`initial_admitted` bit. `initial_admitted` becomes true only after Task 4 has
accepted the initial request and remains true across its later HTTP,
transport, or cancellation failure. It is the authority used by the App's
fixed generic-error fallback to avoid a second initial response.

Response visibility is fixed when the action is prepared: normal versus
ephemeral defer is represented by its projected defer state, and message flags
are already present in the owned serialized bytes. No queue node reads a
builder again or tries to recompute visibility at dispatch time.

Planning uses `projected_state`; terminal success advances
`committed_state`. A local rejection changes neither. A terminal failure sets
both states to `FAILED` and prevents further admission.

The reported public state is:

1. `FAILED` after any accepted terminal failure or queue cancellation;
2. `DEFERRED_QUEUED` while the first accepted initial action is one of the
   three defer variants and has not completed;
3. `INITIAL_QUEUED` while any other first initial action has not completed;
4. otherwise the current `committed_state`.

A very fast worker may complete before the submitting call returns, so callers
are not promised that they will observe an intermediate queued state.

### Exact transition and endpoint table

All message/modal/autocomplete builders in this table are validated and fully
serialized during the action call.

| Projected state/capability before call | Public action | Prepared REST operation | Projected state after admission | State committed on 2xx |
| --- | --- | --- | --- | --- |
| `READY` | state-aware reply | initial callback type 4 | `REPLIED` | `REPLIED` |
| `READY` | defer | initial callback type 5 | `DEFERRED` | `DEFERRED` |
| `READY` | ephemeral defer | initial callback type 5 with only flag 64 | `DEFERRED_EPHEMERAL` | `DEFERRED_EPHEMERAL` |
| `READY` | defer update | initial callback type 6 | `DEFERRED_UPDATE` | `DEFERRED_UPDATE` |
| `READY` | update source message | initial callback type 7 | `REPLIED` | `REPLIED` |
| `READY` | autocomplete | initial callback type 8 | `REPLIED` | `REPLIED` |
| `READY` | show modal | initial callback type 9 | `MODAL` | `MODAL` |
| projected defer state with `original_exists` | state-aware reply | `PATCH` original response | `ORIGINAL_EDITED` | `ORIGINAL_EDITED` |
| any other projected state after an initial has been planned | state-aware reply | `POST` followup | `FOLLOWED_UP` | `FOLLOWED_UP` |
| projected initial with `original_exists` | explicit edit original | `PATCH` original response | `ORIGINAL_EDITED` | `ORIGINAL_EDITED` |
| any projected state after an initial has been planned | explicit followup | `POST` followup | `FOLLOWED_UP` | `FOLLOWED_UP` |
| projected initial with `original_exists` | delete original | `DELETE` original response | state unchanged, projected `original_exists = false` | state unchanged, committed `original_exists = false` |

`original_exists` is initially false. Project it true for initial type 4, 5,
5-with-flag-64, 6, and 7 actions, and false for autocomplete type 8 and modal
type 9. Commit that projected value only on 2xx. It remains true across
followups and becomes false after a successful queued delete. This makes
edit-after-followup possible while correctly rejecting an edit queued after a
projected delete. Do not infer original existence merely from “the last action
was FOLLOWED_UP.”

Explicit defer, defer-update, update, autocomplete, and modal operations are
initial-only. Once any initial action exists in the projected sequence, a
second such explicit initial-only action returns `DCC_ERR_STATE` with no
callback, observer, request, or state mutation. The state-aware reply is the
intentional exception: it maps to edit or followup as shown above.

Do not add new event-type restrictions in this sequencing task. Preserve the
post-Task-10 action validation surface, let the shared Task 6 serializers reject
malformed builders, and let Discord/transport outcomes become terminal queue
results. The RED fixture must nevertheless prove that modal, autocomplete,
update, and all defer variants contend for exactly one initial slot.

### Context helper truth

Make helper behavior explicit and test it:

- `dcc_ctx_response_state()` returns the synchronized reported state above;
- `dcc_ctx_response_sent()`/the private initial-admitted query are true after
  Task 4 accepts an initial request, including while queued and after a later
  terminal failure, and false after local rollback;
- `dcc_ctx_deferred()` is true for `DEFERRED_QUEUED` and the three committed
  defer states;
- `dcc_ctx_replied()` is false for both queued states and true only when the
  committed state is `REPLIED`, `ORIGINAL_EDITED`, `FOLLOWED_UP`, or `MODAL`;
- `dcc_ctx_followed_up()` is true only for committed `FOLLOWED_UP`, and
  `dcc_ctx_response_failed()` is true only for reported `FAILED`;
- `dcc_ctx_can_followup()` and `dcc_ctx_can_edit_original()` use projected
  state/capabilities so callers may enqueue a dependent operation immediately;
- state access after the callback-scoped `dcc_ctx_t` expires remains invalid;
  the queue's independent lifetime does not extend the context view.

Do not retain the current `dcc_app_response_state_t`, `response_state` atomic,
`initial_response_admitted` atomic, and flow `response_flags` as competing
authorities. Current-layout App contexts and flows derive all of these answers
from the queue mutex. Keep only bounded historical-layout compatibility reads
where required.

## Admission and pump algorithm

The following is the required linearization model:

1. Validate the flow/context identity and acquire its queue owner reference.
2. Lock the queue and reject closed, deinitialized, failed, or illegal
   projected transitions without mutation.
3. Use the projected transition to choose the exact concrete endpoint.
4. Run the Task 6 shared prepare function while still serialized by the queue.
   Preparation is pure with respect to network/global REST state. It may
   allocate, validate, escape, and serialize, but cannot call user code.
5. Allocate the node and prepare all ownership before linking it. On any
   error, restore the byte-for-byte state/list snapshot, free temporary data,
   unlock, and return the local status. No callback or observer runs.
6. Append the node at the tail, assign its monotonic sequence number, update
   projected state/capabilities, and acquire the nonempty work reference if
   this was the empty-to-nonempty transition.
7. If another node is active, unlock and return `DCC_OK` immediately.
8. If this is the idle head, mark it submitting/active and call the private
   nonblocking Task 4 admission while holding the queue mutex. Task 4 must not
   run terminal callbacks while holding its REST/client locks; a racing worker
   may block briefly on the queue mutex.
9. If this first admission returns non-OK before the public action returns,
   unlink it, restore the pre-call projected snapshot, drop the work reference,
   and return that status with no callback/observer. If it returns `DCC_OK`,
   mark the initial-admitted bit when applicable, unlock, and return `DCC_OK`.

Keeping the queue mutex across the short, nonblocking first REST admission is
intentional: no second caller can append a dependent action between a failed
head admission and its exact rollback. Never hold the App registry mutex at
the same time.

After a predecessor terminal result, the pump submits at most the next head.
It never waits on a request and never submits a later node merely because the
earlier request was queued, sent, or received a transient 429. A retry remains
the same active Task 4 request; the dependent head stays local until the final
2xx or final failure.

If REST admission for a later node fails after that node's public action
already returned `DCC_OK`, it is an accepted terminal failure, not a local
rollback. Synthesize that exact status for its callback/observer and fail its
dependents according to the rules below.

Use a `pumping`/`pump_again` guard under the queue mutex so synchronous
synthetic completions and callback/observer reentry cannot recursively consume
unbounded stack. User delivery always happens outside the queue lock; the
outermost pump iterates FIFO.

## Fully owned prepared requests

Do not retain builders in queue nodes and do not duplicate the Task 6
serializers or path formatters. Introduce this private, move-only value (the
body pointer may be typed `unsigned char *` instead of `void *`):

```c
typedef struct dcc_rest_interaction_prepared_request {
    dcc_rest_method_t method;
    char *path;
    char *content_type;
    void *body;
    size_t body_len;
} dcc_rest_interaction_prepared_request_t;
```

`path` is always an owned nonempty string after successful preparation;
`content_type` is either null or owned; `body` owns exactly `body_len` bytes
and may contain NUL. Its zero initializer and deinitializer are internal.
Binary body length is authoritative; never recover it with `strlen`.

Expose private prepare operations with these responsibilities and parameter
order for exactly the network operations needed by the queue:

- `dcc_rest_interaction_prepare_response_create(interaction_id, token,
  response, out)` for initial response types 4–9;
- `dcc_rest_interaction_prepare_original_edit(application_id, token, payload,
  out)`;
- `dcc_rest_interaction_prepare_original_delete(application_id, token, out)`;
- `dcc_rest_interaction_prepare_followup_create(application_id, token,
  payload, out)`.

Every prepare function zeroes `out` before validation, takes no client/call
options/request handle, performs no REST admission, and leaves `out` empty on
non-OK return.

The existing public Task 6 wrappers call these same prepare operations and then
submit immediately. The queue calls them and holds the result until its node is
head. The shared seam must preserve all Task 6 validation, historical-prefix,
presence-bit, multipart, escaping, allocation-overflow, and explicit-false
behavior. Public wrappers retain their current `*out_request = NULL` local
rejection and exactly-one-terminal guarantees.

Add `dcc_rest_submit_interaction_prepared()`, a private Task 4 admission
function taking client, mutable prepared request, resolved call options,
private post hook/user data, and optional output handle in that order. It
consumes and zeroes the prepared request on every return and can install the
post-terminal hook described below. Public
`dcc_rest_submit()` retains its documented copy-in behavior and uses the same
common admission core after making its copy. Successful take-owned admission
must transfer buffers into the async job without copying them again. A
non-OK take-owned admission frees them, publishes no handle, callback,
observer, or hook, and leaves no queued REST work.

Because this runs after Task 10, take-owned admission must still pass through
Task 7's one common call-options/authentication/audit transport normalization.
It must enforce each interaction endpoint's manifest capabilities, copy and
later wipe any covered credential material, and preserve historical call-
options prefixes. Do not construct an async job through a shortcut that bypasses
that policy. Queue-generated calls use the current default call-options
initializer plus the private result callback/post hook.

The strict endpoint manifest remains authoritative. If moving route formatting
behind a private prepare helper makes the existing audit lose sight of the
route token, extend the audit with an exact one-hop `prepare_symbol` contract
and malformed-helper self-test. Do not suppress the check, accept arbitrary
call graphs, or leave duplicate formatters. Endpoint counts, public symbols,
methods, normalized routes, and owning headers do not change in Task 11.

## Terminal ordering and the private post hook

Extend the opaque Task 4 request object with a private hook; do not add it to
`dcc_rest_call_options_t` or any installed header. Its conceptual signature is:

```c
typedef void (*dcc_rest_request_post_terminal_fn)(
    dcc_client_t *client,
    const dcc_rest_result_t *result,
    void *user_data
);
```

The result view is borrowed through the hook call only. The hook and its user
data are copied into the opaque request before REST admission and are never
publicly replaceable.

For a queue-backed request, terminal delivery order is exactly:

1. atomically claim/build the one Task 4 terminal result;
2. run the queue's result callback, which commits queue state under the queue
   mutex, releases it, and invokes the action's public `dcc_rest_cb` once;
3. run the existing App and public REST error observers once for a non-OK
   result;
4. invoke the private post hook once, with no REST/request/observer/registry or
   queue lock held; the hook removes/frees the completed head and pumps the
   next node or the next synthetic dependent failure;
5. publish request completion/wake waiters and drop Task 4 runtime ownership as
   required by the existing contract.

The post hook runs for success, transport error, non-2xx, final 429,
cancellation, shutdown, and the Task 4 fallback OOM result. It never runs for a
non-OK local admission. Callback-local handle destruction remains safe, and
public requests with no hook preserve Task 4 behavior exactly.

Synthetic dependent/cancellation terminals use the same callback → observer →
post-pump sequence and terminal-lifetime guard. A synthetic legacy response
has HTTP status 0, no body, and `.error` equal to its synthetic status. Do not
call the App observer manually in addition to the common delivery path.

Keeping the next submission after the prior observer is deliberate. It makes
action callbacks, observer side effects, and network dispatch share one FIFO
order, not merely the HTTP writes.

## Failure, cancellation, and reentry

### Local rejection

Invalid context/flow, malformed builder, illegal projected transition,
allocation failure before append, closed registry, and first-head Task 4
admission failure return non-OK synchronously. They leave committed/projected
state, original-existence flags, queue order, callback count, observer count,
and server request count unchanged. `FAILED` is not a validation sentinel and
must not poison a retry.

### Accepted failure

Success means `dcc_rest_result_status(result) == DCC_OK`, hence transport
success plus HTTP 2xx. On any other actual head result:

- deliver the head's real terminal result once;
- set committed/projected state to `FAILED`;
- make no later network request;
- deliver every already accepted dependent node in original FIFO order with
  `DCC_ERR_STATE`, callback once then observer once;
- reject callback/observer reentrant action attempts locally with
  `DCC_ERR_STATE`.

If the head failed with `DCC_ERR_CANCELED`, or the queue was closed for App or
client teardown, dependent nodes receive `DCC_ERR_CANCELED` instead of
`DCC_ERR_STATE`. A cancel/completion race has one winner and one terminal per
node. A later-node private REST admission failure is that node's real terminal
status; only nodes after it receive the dependent status. The exception is an
admission rejection caused by an already-closing/stopping client, which is
normalized to `DCC_ERR_CANCELED` for that accepted node and its dependents.

### Reentry

Do not hold queue, registry, App, REST, request, or observer locks while calling
user code. Before invoking a successful head callback, commit its stable state
but leave the head marked terminalizing until the post hook. Existing pending
nodes remain linked. An action appended from that callback or its observer is
planned from the existing projected tail and is placed after all preexisting
nodes. The post hook then resumes at the old next node.

A callback may call `dcc_flow_deinit()` on its standalone flow. The accepted
queue work continues from its work/terminal references. A callback may not use
an expired `dcc_ctx_t`; deinit does not make such use valid. Existing App/client
guards continue to reject destroy/wait from managed or terminal callback
frames before mutation.

There is intentionally no new public per-context-action request handle or
cancel function. Accepted actions run to a terminal result; App/client teardown
is the cancellation boundary. This keeps one canonical context vocabulary and
avoids a second request abstraction.

The callback function and `user_data` are borrowed until that node's eventual
terminal callback, which may be delayed behind earlier actions. State this
beside every affected public callback declaration. A non-OK action return means
they are no longer borrowed because no callback can occur.

## Auto-defer linearization

Delete the independent atomic `DCC_APP_RESPONSE_*` state machine. The explicit
reply/defer path, auto-defer timer, `dcc_flow_maybe_auto_defer()`, handler
finish, and teardown all linearize under the same interaction queue mutex.

For App auto-defer:

- attach the registry queue before arming the timer;
- store timer armed/finished state in the queue and give the timer one strong
  queue reference;
- use a checked nanosecond deadline and a cancellable LLAM timer/wait. Reject
  `after_ms` values whose conversion would overflow; never evaluate unchecked
  `after_ms * 1000000`;
- publish/create the wait object before spawning so handler finish cannot miss
  cancellation;
- call the potentially waking LLAM cancel operation outside the queue mutex;
- have the timer task destroy its wait object and release its reference after
  wake/cancel; neither `dcc_ctx_t` nor `dcc_interaction_t *` is retained.

Required race outcomes:

- **reply wins:** it reserves/adopts the initial slot, disarms the timer, and
  submits exactly one type-4 initial response; the timer wakes as a no-op;
- **timer wins:** it enqueues one type-5 defer (`flags: 64` for ephemeral), sets
  projected defer state, and a concurrent state-aware reply appends an original
  edit and returns `DCC_OK` rather than `DCC_ERR_STATE`;
- **handler finish wins:** it disarms/cancels the timer; no defer is submitted;
- **explicit defer versus timer:** exactly one initial defer is admitted; the
  loser is a no-op for the internal timer or a local `DCC_ERR_STATE` for a
  second explicit initial action;
- **timer preparation/first admission fails locally:** roll back to `READY`,
  report no accepted callback, and permit an explicit handler reply to retry;
- **accepted timer request fails terminally:** observe it once and move the
  queue to `FAILED`.

`dcc_flow_maybe_auto_defer()` performs the same locked check-and-enqueue using
its supplied `now_ms`; it must not read/write `.state` concurrently outside the
queue. App timer finish and `dcc_flow_deinit()` are bounded even for a timer
configured many hours ahead.

## App teardown and lock ordering

Preserve the existing managed-task/callback-frame rejection and successful
App stop/schedule-reap behavior. Once teardown mutation is allowed, order it as
follows:

1. set `app->tearing_down` under the existing App lifecycle/listener lock;
2. close the interaction registry to new acquisitions;
3. mark every detached queue closed, cancel its auto-defer timer, and mark
   local pending nodes canceled. If a REST head is active, do not deliver a
   later callback ahead of it; let its cancellation terminal trigger the FIFO
   cascade. If no head is active, start the guarded synthetic-cancel pump;
4. call `dcc_client_prepare_destroy()` so REST admission closes, queued/active
   Task 4 requests cancel or complete, and their callback/observer/post hooks
   finish;
5. wait for the App interaction live-queue count to reach zero with no App,
   registry, queue, REST, or callback lock held;
6. detach the App error sink and permanently close REST terminal entry;
7. reclaim listeners, routes, modules, state/store, client, registry
   synchronization, and App storage in the existing safe order.

The App error sink remains installed through queue cancellation and dependent
synthetic failures. Closing it earlier loses required observations and can free
App-owned observer data while the queue still uses it.

App stop without destroy does not discard already accepted actions and does
not permanently close the registry. App destroy may return an existing
teardown error according to the Task 3 lifecycle contract, but a retry must
continue safely from the already-closed queue/REST state; never reopen
admission during destroy recovery.

Use these lock rules literally:

- registry mutex `R` and per-queue mutex `Q` are never nested;
- obtain a strong queue reference under `R`, release `R`, then acquire `Q`;
- do not acquire the App listener/lifecycle lock while holding `R` or `Q`;
- preparation and the nonblocking Task 4 admission may run under `Q`;
- Task 4 must never invoke callbacks or hooks while holding its client/REST,
  rate-limit, route, request, wait, or observer locks;
- release `Q` before callbacks, observers, LLAM timer cancel/destroy, request
  cancel/destroy/wait, App stop/destroy, registry retirement, or condition
  waits;
- atomics own only refcounts and simple wake/closed publication. `Q` is the
  sole authority for state, list links, active node, timer arbitration, and
  pump flags.

Document these rules in the internal header next to the structs, not only in
the implementer report.

## Required RED contract

Register `dcc_app_v2_interaction_queue_smoke` before production changes. Use a
local interceptor/HTTP server, deterministic gates, atomics, and condition
variables; never call live Discord. Capture RED against the post-Task-10
baseline. At minimum, the RED/green suite must cover all cases below.

### Ordered dispatch and state

- Gate an initial reply for at least 250 ms, call state-aware reply twice in
  under 100 ms each, and prove only one request reaches the server before the
  gate opens. Assert exact order: type-4 callback path/body, then followup POST;
  callback sequence 1 then 2; states `INITIAL_QUEUED`, `REPLIED`, then
  `FOLLOWED_UP`.
- Queue defer → reply → reply and assert type 5 → original PATCH → followup
  POST, never two initial callbacks. Repeat for ephemeral defer with only flag
  64 and defer-update type 6.
- Queue reply → explicit edit → explicit followup and assert all three literal
  methods, paths, bodies, callback order, and terminal states.
- Queue initial reply → delete original → edit original. The edit is rejected
  locally from projected `original_exists = false`; no third request/callback
  occurs. Prove a later state-aware reply is still a followup.
- Hold a transient 429 retry and prove the dependent request is not submitted
  between attempts; release to 2xx and then observe the next request. A final
  429 follows the failure cascade instead.
- Exhaustively assert state strings, preserved numeric enum values, queued
  states, committed/projected helper truth, and the initial-admitted bit after
  local rollback versus terminal failure.

### Ownership and admission rollback

- In separate legal sequences, use a gated predecessor or an unrelated
  single-worker blocker to delay message, modal, and autocomplete dispatch;
  then immediately mutate/free every caller-owned string, nested array, and
  builder storage after each action returns. Free the parser/event interaction
  and original token after handler return. The server must receive the original
  serialized bytes.
- Cover empty strings, historical builder prefixes, unknown presence bits,
  malformed nested builders, and serialization overflow through the existing
  Task 6 validator. Every local rejection preserves queue/state/request/
  callback/observer snapshots.
- Iterate deterministic allocation failpoints across registry growth, token
  copy, queue/node creation, route escaping, JSON/multipart preparation, and
  first Task 4 admission. Each failure is atomic and a subsequent valid reply
  succeeds.
- Inject a later-node Task 4 admission failure after its public call returned
  `DCC_OK`; deliver that exact failure once, observe it once, and fail only its
  remaining dependents locally.

### Failure, cancellation, and delivery order

- Fail the initial head with HTTP error, transport error, and terminal 429.
  Its callback sees the real result; every dependent sees `DCC_ERR_STATE` in
  FIFO order; each non-OK action reaches both App and public observers once;
  no dependent network request is sent.
- Fail a middle action after an initial success. Preserve successful callbacks
  before it, deliver the middle real result, and cascade only later nodes.
- Record delivery events and prove for each failure:
  `action callback end < App observer < public observer < next dependent
  callback/network dispatch`, using the Task 3 observer order where both
  observers are installed.
- Destroy an App with one active and multiple local pending actions. Assert
  active completion/cancellation first, then pending `DCC_ERR_CANCELED`
  callbacks/observers FIFO, bounded destroy, no second network request, no
  callback after App-owned cleanup, and no duplicate under a completion versus
  destroy race.
- Cover callback-only actions, null callbacks, callback/user-data pairing,
  callback cleanup performed by the test owner, and exact once behavior after
  `dcc_flow_deinit()`.

### Reentry, concurrency, and registry sharing

- Prequeue A then B; from A's callback enqueue C. B remains before C. Repeat
  from the App observer. No recursive stack growth or lock-held callback is
  permitted.
- From a terminal callback deinitialize the standalone flow. B and C already
  accepted still finish; a later action on the deinitialized flow returns
  `DCC_ERR_STATE` silently.
- Run multiple producer tasks/threads against one already attached queue.
  Record the mutex linearization sequence and assert request/callback order
  matches it with at most one REST request active for that interaction.
- Dispatch two concurrent App contexts with identical `(id, token)` and prove
  one registry queue/one initial response. Different tokens remain independent;
  a same-key application/client mismatch is rejected.
- Race success, cancel, post hook, reentrant enqueue, registry retirement, and
  a new registry acquire. Prove no ABA, duplicate queue, lost callback, UAF,
  deadlock, or leak.

### Auto-defer and flow ABI

- Use barriers on both sides of the timer deadline for reply-wins,
  timer-wins, finish-wins, and explicit-defer races. Timer-wins reply must
  return `DCC_OK` and produce defer then original edit.
- Repeat normal and ephemeral timer races enough times to exercise both
  winners. Assert exactly one initial response in every run.
- Configure an hours-long timer, finish/destroy immediately, and require a
  bounded teardown. Cover the largest valid conversion and the first
  overflowing `after_ms` without wraparound.
- Test current layout init/state/action/deinit, deinit before terminal, repeated
  null/deinitialized deinit, and action after deinit.
- Construct every supported historical prefix with poisoned tail bytes and
  canaries. State reads remain bounded; every action returns `DCC_ERR_STATE`
  without a write or callback.
- Compile and execute the transition Sugar flow macros only as named lvalues
  followed by deinit. Audit all in-tree `DCC_FLOW*` values so Task 11 introduces
  no owner leak before Task 14 removes Sugar.
- Exercise modal, autocomplete, update, defer, and defer-update competing for
  the initial slot. Exactly the first projected initial action is accepted;
  each later explicit initial-only operation is a silent local state error.

## Implementation order

Use small test-first increments, keeping each intermediate implementation
commit buildable if intermediate commits are necessary:

1. Add and register the RED queue/state/ownership fixture; capture the delayed
   double-reply failure against Task 10.
2. Add the shared fully owned interaction prepare seam and prove all affected
   Task 6 endpoint byte snapshots and strict manifest checks remain green.
3. Add the private Task 4 post hook/take-owned admission and focused ordering,
   local-rejection, cancellation, callback-destroy, and public-no-hook
   regression tests.
4. Implement the refcounted queue, committed/projected state engine, FIFO pump,
   synthetic terminal path, failure cascade, and reentry guard.
5. Append the public states/private flow tail, add `dcc_flow_deinit()`, migrate
   standalone flow operations, and pass historical-prefix/layout tests.
6. Add the App hash registry, attach/release it on every dispatch exit,
   migrate all context response actions, and share duplicate contexts.
7. Replace auto-defer atomics with same-lock timer arbitration; add checked,
   cancellable timing and the full race fixture.
8. Integrate close/cancel/drain into App destroy before error-sink detach; pass
   lifecycle and callback-owner cleanup tests.
9. Run the complete verification matrix, update affected public ownership
   comments and focused docs, and write the implementer report.

Do not preserve the old optimistic state path alongside the queue “for
compatibility.” Current-layout callers use one authority. Do not weaken a RED
assertion to admit either request order.

## Verification gates

Run and record exact commands, test counts, and elapsed bounds in the report.
At minimum require:

- `dcc_app_v2_interaction_queue_smoke`, `dcc_interaction_flow_smoke`, flow
  layout, App context/error/lifecycle/listener-concurrency, interaction server,
  Task 4 request/result, and Task 6 endpoint smoke targets;
- the complete enabled CTest suite from a clean normal build;
- strict endpoint manifest/self-tests after Task 10, v2 transition/public
  surface, project-layout/translation-unit, source-package, release-contract,
  and documentation audits;
- standalone changed public headers as strict C11 and C++17, including both
  include orders with `<dcc/app.h>`, `<dcc/interaction_flow.h>`, and
  `<dcc/rest.h>`;
- x86_64 and i686 MinGW header plus every changed production-source probe; run
  the Windows runtime smoke fixture when the repository's runner is available;
- focused ASan/UBSan for prepared-input lifetime, every allocation rollback,
  historical canaries, callback deinit/reentry, failure cascade, and App
  destroy with active/pending/timer work;
- focused TSan, repeated at least 20 times, for reply/timer, explicit
  defer/timer, multi-producer FIFO, callback/observer reentry,
  completion/destroy, and registry acquire/retire races;
- installed-package configure/build/run, MkDocs strict, and
  `git diff --check`.

The known baseline event-waiter TSan race recorded during Task 3 is separate
release debt. It may be reported separately if a broad sanitizer run reaches
it, but it does not waive any focused Task 11 TSan failure. Likewise, an
unavailable Windows runtime runner is reported as unavailable only after both
MinGW architectures compile successfully; it is not described as a runtime
pass.

## Commit and handoff

When all focused and full gates are green, commit the implementation, tests,
public comments, shared private REST seam, and build updates with exact message:

```text
feat: queue interaction responses asynchronously
```

Then create `task-11-implementer-report.md` beside this brief. Include:

- Task 10 base hash and implementation hash;
- the original RED output and why the baseline violated ordering;
- final file list and public ABI delta;
- the complete transition/state/helper matrix;
- ownership, callback order, ref sources, retirement, and lock-order summary;
- auto-defer race counts by winner;
- allocation/failure/cancellation/destroy evidence;
- normal, full CTest, ASan/UBSan, repeated TSan, header, MinGW/Windows,
  package, docs/audit, and diff-check results;
- pre-existing failures separated from Task 11 regressions.

Commit the report separately and stop for fresh review. Do not push.
