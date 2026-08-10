# Task 3 fresh review round 2 — verification report

## Implementation commit

- `bff97a7` — `fix: close structured error lifetime edge cases`

## RED evidence

The regression fixtures were written and run before the implementation:

- Flow layout compilation failed because `dcc_interaction_flow_t` had no
  `response_flags` member and the assertion requiring the current object to be
  larger than the historical 56-byte object failed.
- The composed-DM fixture failed with
  `composed DM inner failure delivery duplicated or escaped its frame`.
- The App success-completion fixture failed with
  `App success terminal did not hold owned state through callback drain`.
- Same-context synchronous reply re-entry failed with
  `same-context synchronous reply reentry did not fail fast`.
- The real timer-versus-handler claim race failed with
  `fast=0 status=0 elapsed=100 requests=2 state=2`, proving that the handler
  waited on `CLAIMED` and submitted a second request instead of failing fast.

## Exact verification

Focused normal verification, including the layout audit after splitting the
lifetime fixture:

```sh
ctest --test-dir build-task3 --output-on-failure \
  -R '^(dcc_project_layout_audit|dcc_app_v2_error_smoke|dcc_app_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_interaction_flow_smoke|dcc_interaction_flow_layout_smoke)$'
```

- `6/6` passed in `2.29s`.

Canonical build and complete test matrix:

```sh
cmake --build build-v2-api -j8
ctest --test-dir build-v2-api --output-on-failure
```

- Build exited `0`.
- `176/176` enabled tests passed in `75.16s`.
- CTest enumerated `200` tests; `24` LLAM subdirectory tests were intentionally
  disabled.

Installed-package consumer:

```sh
cmake --install build-v2-api --prefix build-task3-install
cmake -S tests/package_consumer -B build-task3-package-consumer \
  -DCMAKE_PREFIX_PATH="$PWD/build-task3-install" \
  -DDCC_LLAM_ROOT=/Users/feral/Desktop/Programming/LLAM \
  -DDCC_LLAM_LIBRARY="$PWD/build-v2-api/_deps/llam/libllam_runtime.a"
cmake --build build-task3-package-consumer -j8
./build-task3-package-consumer/dcc_package_consumer
```

- Install, configure, build, and executable each exited `0`.

ASan/UBSan focused verification:

```sh
cmake --build build-v2-api-asan -j8
ctest --test-dir build-v2-api-asan --output-on-failure \
  -R '^(dcc_http_smoke|dcc_app_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_interaction_flow_smoke|dcc_interaction_flow_layout_smoke)$'
```

- Build exited `0`.
- `7/7` passed in `4.22s` with no sanitizer finding.

TSan focused verification:

```sh
cmake --build build-v2-api-tsan -j8
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^(dcc_app_smoke|dcc_app_v2_listener_smoke|dcc_app_v2_error_smoke|dcc_app_v2_listener_concurrency_smoke|dcc_interaction_flow_smoke|dcc_interaction_flow_layout_smoke)$'
```

- Build exited `0`.
- `6/6` passed in `3.43s` with no race report.

## Flow layout migration

- On the canonical arm64 build, `dcc_interaction_flow_t` grows honestly from
  the historical `56` bytes to `64` bytes. `response_flags` starts at offset
  `56`; it no longer aliases tail padding.
- This is a binary-layout migration. Programs built against the smaller public
  definition must be recompiled before using this DCC version.
- `dcc_flow_init()` clears and writes the current
  `sizeof(dcc_interaction_flow_t)`. The public `size` member only permits safe
  inspection of a manually supplied historical prefix; it cannot enlarge an
  old binary allocation.
- C11 coverage uses a poisoned, aligned 56-byte historical prefix followed by
  a canary. The helper takes the state-based compatibility path without reading
  padding or modifying the canary. A separate C++17 layout target asserts the
  same offset and size contract.

## Remaining event-wait TSan debt

```sh
ctest --test-dir build-v2-api-tsan --output-on-failure \
  -R '^dcc_http_smoke$'
```

- CTest exited `8`; `0/1` passed in `2.19s`.
- TSan reported exactly `1` warning in `run_public_event_wait_smoke`:
  `dcc_event_bus_signal_waiters_locked` writes `waiter->completed` at
  `src/events/event_waiter_match.c:89` while
  `dcc_event_waiter_wait_channel` reads it at
  `src/events/event_waiter_runtime.c:74`.
- This remains the known event-wait race. The Task 3 focused TSan paths above
  emitted no additional report.
