# Task 4 fresh review round 2 — contract and teardown fixture

## Fix commit

- `f1a9e2a` — `fix: align REST cancellation contract`

## Changes

- Aligned the authoritative design, Task 4 plan Step 6, and Task 4 brief with
  the implemented exact-cancellation contract. Pending exact cancel/destroy
  remains queue-owned, never delivers terminal user code on the public call
  stack, and uses an existing runtime worker when work can be admitted.
  Canceled jobs bypass route/rate/retry eligibility while preserving
  priority/concurrency; per-job route claims preserve same-route exclusion;
  stopping or spawn failure retains queue ownership for possible bulk teardown
  delivery on the teardown caller. Task checkboxes remain unchanged.
- Reworked the client-teardown regression to stay inside the supported client
  lifetime contract. An active gated interceptor holds the client, stop occurs,
  exact cancel runs and joins alone, and the test proves cancel returned while
  the pending callback remained undelivered. Only then does client destruction
  start. Teardown delivers the pending result exactly once and remains blocked
  on the active worker until its gate is released. Both retained handles are
  checked for canceled results and safe cancel/destroy after client reclamation.
- All coordination uses atomics and bounded gate waits. No production runtime
  source changed.

## Verification

- Focused normal request fixture: 10 consecutive passes in `17.49s`.
- ASan/UBSan: request, HTTP, and App error fixtures passed `3/3` in `4.80s`.
- TSan request fixture: 10 consecutive passes in `20.14s`, no race report.
- Strict installed-header C11 and C++17 compilation passed.
- MinGW x86_64 public-header and changed-fixture compilation passed with
  `-Wall -Wextra -Werror`.
- Full normal build and CTest passed `97/97` in `76.77s`.
- `git diff --check` and staged diff checks passed. No push was performed.
