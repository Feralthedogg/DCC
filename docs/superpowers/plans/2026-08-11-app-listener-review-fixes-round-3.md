# App Listener Review Fixes Round 3 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make implicit command schemas valid and atomic, make App stop requests safe from every App-owned callback, and preserve LLAM's Windows include-order contract.

**Architecture:** Listener validation rejects missing implicit CHAT_INPUT descriptions before allocating or mutating App state, while schema preparation uses one root-command derivation path for slash, subcommand, and autocomplete. Shutdown is split into a callback-safe request phase and a serialized owner-only reap phase: stop only publishes cancellation and requests runtime/task-group stop, while wait/destroy claim, join, destroy, and clear the schedule task group exactly once. App internals include the shared Windows policy header before any direct Win32 dependency.

**Tech Stack:** C11, C++17 header probes, LLAM task groups, pthreads/Win32 synchronization, CMake/CTest, ASan/UBSan, TSan, MinGW.

## Global Constraints

- Start from clean commit `88f9789`; do not rewrite earlier commits.
- Do not edit the controller-owned ledger.
- Preserve all 174 enabled tests and the focused V2 public surface.
- Witness each behavioral defect RED before implementation and GREEN after it.
- Never wait, join, destroy, or clear `app->tasks` from an App-owned callback.

### Task 1: RED coverage for implicit schemas and exact transactions

**Files:**
- Modify: `tests/app_v2_listener_contract_smoke.c`

- [x] Add exact-snapshot rejection cases for null and empty implicit descriptions across slash, subcommand, root autocomplete, and subcommand autocomplete.
- [x] Retry each rejected listener with a non-empty description, assert exactly one non-empty CHAT_INPUT schema is committed, and unlisten cleanly.
- [x] Seed non-empty listener, route, schedule, and registry arrays before forced failures.
- [x] Exercise direct public registry-add failure with exact registry pointer/count/capacity preservation.
- [x] Prove each private one-shot failpoint resets by retrying the same operation successfully.

### Task 2: RED coverage for callback-safe stop

**Files:**
- Modify: `tests/app_v2_listener_concurrency_smoke.c`
- Modify: `include/dcc/app/lifecycle.h`

- [x] Run real one-millisecond canonical and legacy schedules whose callbacks call `dcc_app_stop`.
- [x] Assert stop returns promptly, preserves the task group for the owner, and leaves the App valid.
- [x] Assert callback-side `dcc_app_wait` is rejected before mutation.
- [x] Assert owner wait/destroy reaps the task group, and repeated stop plus no-schedule stop are idempotent.
- [x] Document stop as a nonblocking request that is valid in every App-owned callback.

### Task 3: Implement implicit CHAT_INPUT schema validation

**Files:**
- Modify: `src/app/app_listener.c`

- [x] Derive the implicit root command name for slash, subcommand, and autocomplete.
- [x] Require a non-null, non-empty route description whenever those kinds omit a builder.
- [x] Preserve supplied-builder kind/type/description validation exactly.
- [x] Prepare one implicit CHAT_INPUT schema with the validated name and description before all App mutation.

### Task 4: Split shutdown request from owner reap

**Files:**
- Modify: `src/app/app.c`
- Modify: `src/app/app_scheduler.c`
- Modify: `src/internal/app/dcc_app_internal.h`

- [x] Make stop atomically publish App/schedule cancellation and issue only nonblocking task-group/client stop requests.
- [x] Add a serialized owner reaper that claims `app->tasks`, joins/cancels, destroys, clears it exactly once, and preserves the first failure status.
- [x] Restore exact task-group ownership after a reap failure so wait/destroy can retry without partial teardown.
- [x] Reject owner wait from an App callback before client or schedule mutation.
- [x] Make wait and destroy invoke the reaper, including start-failure cleanup, without double join/destroy.

### Task 5: Preserve Windows include ordering

**Files:**
- Modify: `src/internal/app/dcc_app_internal.h`
- Add: `tests/app_windows_include_order_smoke.c`
- Modify: `CMakeLists.txt`

- [x] Replace App internal direct `windows.h` inclusion with `internal/dcc_windows_internal.h`.
- [x] Add a compile fixture that includes App internals followed by LLAM and fails if the lean/nominmax policy or winsock2 ordering regresses.
- [x] Compile the fixture as strict MinGW C11 when the compiler is available, without weakening LLAM's guard.

### Task 6: Verification, report, and commits

**Files:**
- Modify: `.superpowers/sdd/2026-08-11-dcc-2.0-api-redesign/task-2-implementer-report.md`

- [x] Run focused `4/4`, full configured build and `174/174` CTest, ASan/UBSan, TSan, strict C11/C++17/header/audit probes, MinGW C11 syntax, and `git diff --check`.
- [ ] Append Fix Round 3 with exact RED/GREEN evidence, platform results, and commit SHAs.
- [ ] Commit implementation and report separately; finish with a clean worktree.
