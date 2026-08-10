# App Teardown Cleanup Fixes Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Eliminate the two confirmed App-destroy cleanup UAFs while making destroy's return value unambiguously indicate whether the App was consumed.

**Architecture:** After schedule reaping succeeds, destroy enters a private teardown state before releasing any App-owned registrations or user cleanup data. Stop observes that state under the App lock and becomes an idempotent no-mutation success; schedule storage is nulled before later callbacks. A private one-shot schedule cancel failpoint distinguishes a harmless stop-request error followed by successful reap from an ownership-preserving reap failure.

**Tech Stack:** C11, LLAM task groups, CMake/CTest, ASan/UBSan, TSan, MinGW.

## Global Constraints

- Start from clean commit `b8f1e80` and do not rewrite history.
- Fix only the two confirmed teardown/consumed-object defects.
- Do not edit the controller-owned ledger.
- Keep all failpoints private.
- A non-OK destroy result must always leave the App alive and retryable.

### Task 1: RED teardown callback coverage

**Files:**
- Modify: `tests/app_v2_listener_concurrency_smoke.c`

- [x] Register a public legacy schedule and a module cleanup that calls `dcc_app_stop`.
- [x] Register a separate public legacy schedule and state cleanup that calls `dcc_app_stop`.
- [x] Run both cases under ASan and witness the schedule-array UAF before implementation.
- [x] Assert each cleanup runs exactly once, callback stop returns `DCC_OK`, and destroy returns `DCC_OK`.

### Task 2: RED consumed-object result coverage

**Files:**
- Modify: `tests/app_v2_listener_concurrency_smoke.c`
- Modify: `src/internal/app/dcc_app_internal.h`

- [x] Add a private one-shot schedule cancel/request-stop failure flag used only by the focused test.
- [x] Start a real scheduled task, force the first request-stop failure, let owner reap succeed, and assert destroy consumes the App and returns `DCC_OK`.
- [x] Retain the existing reap-failure test proving non-OK leaves task-group/App ownership intact for a clean retry.

### Task 3: Implement teardown state and destroy result rule

**Files:**
- Modify: `src/app/app.c`
- Modify: `src/app/app_scheduler.c`
- Modify: `src/internal/app/dcc_app_internal.h`

- [x] Publish private teardown state under the App lock immediately after successful schedule reap and before listener/route/module/state cleanup.
- [x] Make `dcc_app_stop` return `DCC_OK` without reading or mutating other App state while teardown is active.
- [x] Null and reset schedule pointer/count/capacity immediately after freeing schedule storage and before user module/state cleanup callbacks.
- [x] Return non-OK only for pre-consumption reap failure; return `DCC_OK` after any path that reaches `free(app)`.

### Task 4: Verification, report, and commits

**Files:**
- Modify: `.superpowers/sdd/2026-08-11-dcc-2.0-api-redesign/task-2-implementer-report.md`

- [x] Run focused `4/4`, the full enabled CTest set, ASan/UBSan `4/4`, TSan `2/2`, strict C11/C++17/header/audit probes, MinGW C11, the Windows fixture, and `git diff --check`.
- [x] Commit implementation and report separately, append exact RED/GREEN evidence, and finish with a clean worktree.
