# App Listener Review Fixes Round 2 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [x]`) syntax for tracking.

**Goal:** Close the remaining App listener lifetime, callback-destruction, middleware-snapshot, and exact-transactionality gaps without regressing the focused DCC 2 API.

**Architecture:** Listener teardown becomes a serialized two-stage state machine: one owner completes source detachment, then one owner runs cleanup, while all external waiters observe final completion. Listener and route array growth plus command-registry additions are prepared in private storage and committed only after every fallible operation succeeds; route registration stays invisible behind the App lock until the final no-fail commit. Dispatch owns copied middleware arrays, and a TLS App callback frame rejects recursive destruction before mutation.

**Tech Stack:** C11, C++17 header probes, pthreads/Win32 synchronization, CMake/CTest, ASan, TSan.

## Global Constraints

- Start from clean commit `54bb77d`; do not rewrite prior commits.
- Do not edit the controller-owned ledger.
- Preserve all 174 previously enabled green tests and the focused public surface.
- Run every behavioral fix through a witnessed RED/GREEN cycle.
- Never invoke listener, route, middleware, or module user cleanup while holding the listener registry lock.

---

### Task 1: Deterministic remaining RED coverage

**Files:**
- Modify: `tests/app_v2_listener_concurrency_smoke.c`
- Modify: `tests/app_v2_listener_contract_smoke.c`

**Interfaces:**
- Consumes: internal route middleware registration, real App scheduler workers, public listener registration/destruction.
- Produces: deterministic barriers for detach completion, middleware reallocation, concurrent once claims, self-destroy, real worker destruction, and exact state snapshots.

- [x] Add a blocking route-middleware cleanup. Release an in-flight handler so release-side detachment enters that cleanup, then assert external unlisten and listener cleanup remain blocked until middleware cleanup exits; immediately re-register the same route after unlisten and dispatch it once.
- [x] Add a global middleware barrier followed by route middleware growth past capacity while dispatch is paused. Assert the active dispatch uses only its original copied middleware list.
- [x] Dispatch a once event from two threads released by one atomic gate and assert one handler claim and one cleanup.
- [x] Call `dcc_app_destroy` inside route, event, and task callbacks and assert each returns `DCC_ERR_STATE`, leaves the App usable, and permits owner-thread destruction later.
- [x] Start an actual one-millisecond canonical schedule, wait for its worker to enter, destroy concurrently, and prove cleanup waits for worker release. Add the equivalent event destruction case.
- [x] Snapshot listener/route/schedule pointers, counts, capacities, next IDs, and command-registry size/state/count/cap around every forced failure; assert exact restoration, no failed-listener cleanup, and a clean retry.
- [x] Add a literal command-kind/type table covering CHAT_INPUT, USER, MESSAGE, and default-type behavior for slash/subcommand/autocomplete/user-menu/message-menu.
- [x] Add a huge fallback-values count that must return `DCC_ERR_NOMEM` without dereferencing or publishing a partial destination.
- [x] Run the two focused tests and record each expected pre-fix failure.

### Task 2: Serialized detach and finalization

**Files:**
- Modify: `src/app/app_listener.c`
- Modify: `src/app/app_routes.c`
- Modify: `src/internal/app/dcc_app_internal.h`

**Interfaces:**
- Produces: `detach_started/detach_done/detach_status` and `finalize_started/finalize_done/finalize_status` state guarded by the App listener mutex.

- [x] Replace the early `detached` publication with a detach owner that marks completion only after route removal, event unregistration, or schedule cancellation returns.
- [x] Make the finalization owner wait for completed detachment before invoking cleanup; publish final completion after cleanup and wake all waiters.
- [x] Make external unlisten and destruction wait for final completion. Keep self-unlisten non-blocking and let the final release own deferred route detachment.
- [x] Refactor route removal to unlink under the registry lock, then run middleware/user cleanup outside it; the detach operation does not complete until those callbacks return.
- [x] Run the detach and existing unlisten/once/destruction tests GREEN.

### Task 3: Callback-safe destruction contract

**Files:**
- Modify: `include/dcc/app/lifecycle.h`
- Modify: `src/app/app.c`
- Modify: `src/app/app_listener.c`
- Modify: `src/app/app_routes.c`
- Modify: `src/app/app_events.c`
- Modify: `src/app/app_message_commands.c`
- Modify: `src/app/app_scheduler.c`
- Modify: `src/internal/app/dcc_app_internal.h`

**Interfaces:**
- Produces: `dcc_status_t dcc_app_destroy(dcc_app_t *app)` and TLS callback-frame enter/leave/current helpers.

- [x] Move the callback frame type to the internal App header and track both App and optional listener entry.
- [x] Enclose canonical/legacy route middleware+handler, canonical/legacy event, task, and cleanup callbacks in frames.
- [x] Return `DCC_ERR_STATE` before any mutation when destruction is requested from a frame for the same App; return `DCC_OK` for null and successful owner destruction.
- [x] Document that callbacks call `dcc_app_stop()` and the owner destroys after dispatch.
- [x] Run route/event/task self-destroy tests GREEN and verify ignored return values remain source-compatible.

### Task 4: Owned middleware snapshots

**Files:**
- Modify: `src/app/app_routes.c`
- Modify: `src/internal/app/dcc_app_internal.h`

**Interfaces:**
- Produces: lock-protected copies of App-global and route middleware entries owned for one dispatch.

- [x] Guard global middleware mutation with the App listener lock.
- [x] Copy route middleware while route lookup and listener pinning hold the lock; on allocation failure release the pin without invoking stale pointers.
- [x] Copy global middleware under the same lock before invoking callbacks; pass both snapshots to dispatch and free them afterward.
- [x] Keep the listener reference through all middleware, handler, error observer, and snapshot lifetime.
- [x] Run the deterministic mutation test under regular, ASan, and TSan builds.

### Task 5: Exact no-fail transaction commit

**Files:**
- Modify: `src/command_registry/command_registry_add.c`
- Modify: `src/internal/command_registry/dcc_command_registry_internal.h`
- Modify: `src/app/app_listener.c`
- Modify: `src/app/app_scheduler.c`
- Modify: `src/internal/app/dcc_app_internal.h`

**Interfaces:**
- Produces: `dcc_command_registry_add_prepare/commit/abort` plus private App array-growth stages.

- [x] Deep-copy and validate a command builder before creating registry state or allocating/publishing entry capacity.
- [x] Prepare new registry state or entry storage privately; commit by pointer swap and one entry write only after all fallible work succeeds. Make public `dcc_command_registry_add_builder` use the same transaction.
- [x] Stage listener and route array growth behind the held App lock. On failure restore the exact original pointer/capacity/count/IDs; on commit free the replaced storage.
- [x] Prepare command schema before route mutation, apply policy to the staged route, then commit command, route, and listener state through no-fail operations.
- [x] Make canonical schedule allocation/growth/spawn preserve the exact App schedule pointer/capacity/count on failure.
- [x] Add narrow private failpoints for deterministic policy-allocation and registry-growth failure tests; failpoints affect no public ABI and reset after one use.
- [x] Run exact snapshot/retry tests GREEN.

### Task 6: Command semantics and partial ownership

**Files:**
- Modify: `src/app/app_listener.c`
- Test: `tests/app_v2_listener_contract_smoke.c`

**Interfaces:**
- Produces: exact listener-kind/command-type validation and self-consistent partial-copy cleanup.

- [x] Resolve omitted builder type to CHAT_INPUT. Require CHAT_INPUT for slash/subcommand/autocomplete, USER for user context menu, and MESSAGE for message context menu.
- [x] Require a name and chat-input description; reject description/options payload on USER/MESSAGE builders and reject every incompatible kind/type pair before mutation.
- [x] Keep form/component fallback pointer and count zero until the full nested array copy succeeds, then publish them together.
- [x] Reject allocation-size overflow before validation loops can dereference user arrays; return `DCC_ERR_NOMEM` with exact state preservation.
- [x] Audit every partial-copy branch for pointer/count consistency and run the command matrix plus overflow tests GREEN under ASan.

### Task 7: Full verification, report, and commits

**Files:**
- Modify: `.superpowers/sdd/2026-08-11-dcc-2.0-api-redesign/task-2-implementer-report.md`

- [x] Run focused CTest, full configured build and CTest, standalone C11/C++17 header probes, strict warnings, surface/public/layout audits, ASan, TSan, and `git diff --check`.
- [x] Append Fix Round 2 with exact RED/GREEN commands, results, platform notes, and new commit SHA(s).
- [x] Create new implementation and report commits without rewriting `54bb77d` or earlier history.
