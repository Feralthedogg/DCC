# App Listener Review Fixes Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the focused status-returning App listener API transactional, fail-closed, metadata-owning, and safe under concurrent dispatch, unlisten, once, and destruction.

**Architecture:** Add a portable App-local recursive mutex/condition abstraction and stable retired listener entries with per-entry in-flight references. Canonical route lookup acquires the entry while holding the same registry lock; unlisten deactivates and detaches before waiting, while self-unlisten and once defer final route removal/cleanup to the last release. Registration validates and owns all metadata before committing mutations, adds command schema last, and rolls back the route on any failure.

**Tech Stack:** C11, C++17 header probes, pthreads/Win32 synchronization, CMake/CTest, ASan/TSan.

## Global Constraints

- Preserve the Task 2 public listener contract and all behavior outside the review scope.
- Keep `include/dcc/app/legacy.h` as the sole transition aggregate and retain its exact layout exception.
- Do not edit the controller-owned ledger.
- Every bug fix follows a witnessed RED then GREEN test cycle.

---

### Task 1: Deterministic lifetime regression tests

**Files:**
- Create: `tests/app_v2_listener_concurrency_smoke.c`
- Modify: `CMakeLists.txt`
- Modify: `tests/app_v2_listener_smoke.c`

**Interfaces:**
- Consumes: `dcc_app_listen`, `dcc_app_unlisten`, event-bus dispatch, internal schedule adapter.
- Produces: deterministic route/event/task barriers covering external unlisten, self-unlisten, once, and destroy.

- [ ] Add portable test latch/thread helpers backed by pthread mutex/condition on POSIX and critical-section/event/thread primitives on Windows.
- [ ] Add a route handler that signals entry, blocks on a release latch, and proves cleanup cannot run while it is in flight.
- [ ] Add equivalent generic-event and task cases; assert unlisten/destroy block until release without sleep-based timing.
- [ ] Add self-unlisten and once cases that prove no deadlock, one dispatch, one detach, and one cleanup.
- [ ] Register the test and run `cmake --build build-v2-api --target dcc_app_v2_listener_concurrency_smoke -j8 && ctest --test-dir build-v2-api -R '^dcc_app_v2_listener_concurrency_smoke$' --output-on-failure`; expect a deterministic failure or sanitizer-detected invalid lifetime before production changes.

### Task 2: Portable registry synchronization and retirement

**Files:**
- Modify: `src/internal/app/dcc_app_internal.h`
- Modify: `src/app/app.c`
- Modify: `src/app/app_listener.c`
- Modify: `src/app/app_routes.c`

**Interfaces:**
- Produces: `dcc_app_listener_sync_init/deinit`, registry lock/wait/wake helpers, entry acquire/release, route snapshot pinning, shutdown then post-client reclaim.

- [ ] Add a recursive mutex plus condition variable using pthread mutex attributes on POSIX and `CRITICAL_SECTION`/`CONDITION_VARIABLE` on Windows.
- [ ] Keep only active entries in `app->listeners`; atomically move deactivated entries to a retired chain retained until client quiescence.
- [ ] Acquire an in-flight reference under the registry lock before canonical route/event/task use and release it after all status observation is complete.
- [ ] Deactivate and detach first; external unlisten waits for zero references, while a TLS dispatch frame makes self-unlisten defer cleanup/removal until release.
- [ ] Split listener shutdown from reclamation so event-bus snapshot user-data remains valid until after client destruction.
- [ ] Re-run the concurrency test and focused listener smoke; expect GREEN.

### Task 3: Schedule cancellation and once detachment

**Files:**
- Modify: `src/app/app_scheduler.c`
- Modify: `src/internal/app/dcc_app_internal.h`
- Modify: `src/app/app_listener.c`
- Test: `tests/app_v2_listener_concurrency_smoke.c`

**Interfaces:**
- Produces: canceled canonical schedules that are unlinked immediately and retained by the listener entry until worker quiescence.

- [ ] Add an atomic cancel flag checked by the interruptible sleep and loop.
- [ ] On task unlisten, mark canceled and remove the schedule from the App schedule array; never leave the sleeper running until its original deadline.
- [ ] Claim once under the registry lock, remove it from the active registry, unregister it before handler invocation, and cleanup after its sole reference releases.
- [ ] Verify task cancel, once, self-unlisten, and destruction deterministically.

### Task 4: Fail-closed validation and metadata ownership

**Files:**
- Modify: `src/app/app_listener.c`
- Test: `tests/app_v2_listener_smoke.c`

**Interfaces:**
- Produces: exact kind-field matrix validation and independently owned binding/validator metadata.

- [ ] Add table-driven RED cases for non-boolean values, contradictory DM/guild restrictions, inactive target fields, component command builders, and binding/validator inactive fields.
- [ ] Replace hardcoded nested version checks with each public `DCC_LISTENER_*_VERSION` constant.
- [ ] Deep-copy binding names/prefixes/fallback strings, fallback arrays and elements, plus validator field strings; make partial-copy cleanup exact.
- [ ] Overwrite/free stack or temporary source metadata after registration, dispatch, and assert copied values still drive binding/validation.
- [ ] Run the focused listener smoke and expect GREEN.

### Task 5: Transactional route/schema commit

**Files:**
- Modify: `src/app/app_listener.c`
- Test: `tests/app_v2_listener_smoke.c`

**Interfaces:**
- Produces: registration that mutates no observable registry/count/state on failure and permits a clean retry.

- [ ] Add RED cases using invalid/duplicate command builders and policy-stage failures; capture command, route, listener, and schedule counts before each attempt.
- [ ] Prepare and validate schema without mutation, add route and policy while the entry is inactive, then add command schema last; remove the route on schema failure and commit active registry/id only after all steps succeed.
- [ ] Assert failed registration preserves counts/IDs and retry succeeds exactly once.

### Task 6: Focused context header

**Files:**
- Modify: `include/dcc/app/context.h`
- Modify: `include/dcc/app/legacy.h`
- Test: `tests/app_v2_context_header_smoke.c`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Produces: standalone canonical context types/declarations with no legacy void callback or REST-mirror exposure.

- [ ] Move shared wait/binding/validator/date types from `legacy.h` into `context.h` and include only their direct dependencies.
- [ ] Make `legacy.h` include `context.h` and remove duplicate definitions.
- [ ] Compile a standalone C11/C++17 probe that includes only `context.h` and fails if a legacy callback/mirror sentinel is visible.

### Task 7: Final verification and report

**Files:**
- Modify: `.superpowers/sdd/2026-08-11-dcc-2.0-api-redesign/task-2-implementer-report.md`

- [ ] Run focused CTest, full configured build/CTest, standalone C11/C++17 header probes, strict warning compiles, ASan, supported TSan, and `git diff --check`.
- [ ] Append Fix Round 1 with exact commands/results, remaining platform limitation if TSan is unsupported, and new commit SHA(s).
- [ ] Commit without rewriting `2ea51ea`.
