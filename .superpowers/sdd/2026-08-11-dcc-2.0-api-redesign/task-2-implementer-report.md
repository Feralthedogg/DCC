# Task 2 implementer report — focused status-returning App API

## What I implemented

- Replaced the four declaration fragments with focused public App headers for
  base types, options, lifecycle, listeners, context, modules, and store APIs.
- Added the versioned `dcc_listener_t` registration model with canonical
  `dcc_listener_init`, `dcc_app_listen`, and `dcc_app_unlisten` entry points.
- Made canonical route, typed-route, event, ready, message, message-command,
  and scheduled-task callbacks return `dcc_status_t`; retained the 1.x void
  callback surface only in the explicitly transitional `legacy.h` header.
- Added kind-specific listener metadata validation, typed binding and validator
  bounds checks, subcommand autocomplete targeting, returned-status reporting,
  and exactly-once cleanup semantics.
- Added a focused listener smoke test and kept existing 1.x examples, tools,
  and tests source-compatible through the transition layer.

## TDD evidence

- RED: the initial focused fixture failed on missing `dcc_listener_t`,
  `dcc_listener_init`, `DCC_LISTENER_SLASH`, and `dcc_app_listen`; a status
  callback was incompatible with the old void-returning registration API.
- GREEN: implementing the focused surface made route, typed-route, event, and
  task registration/dispatch tests pass.
- Expanded cleanup coverage exposed a real App-destruction use-after-free: a
  one-pass destructor freed an early listener while a later `unlisten` still
  scanned it.  App destruction now unregisters/cleans every entry first and
  frees entries in a separate second pass.

## Tests and exact results

- Full configured build with tests, examples, and tools enabled: passed.
- Full CTest: `171/171` enabled tests passed in 86.02s; 24 LLAM tests were
  disabled by the current configuration.
- Focused CTest (`dcc_v2_surface_audit`, `dcc_app_smoke`,
  `dcc_app_v2_listener_smoke`, `dcc_project_layout_audit`): `4/4` passed.
- ASan rebuild and `dcc_app_v2_listener_smoke`: `1/1` passed in 0.08s.
- Strict `-Wall -Wextra -Wpedantic -Wconversion -Wshadow -fsyntax-only`
  checks for `src/app/app_listener.c` and `tests/app_v2_listener_smoke.c`:
  passed with no diagnostics.
- `git diff --check`: passed.

## Self-review findings

- Scheduled tasks store the internal listener adapter and stable listener
  entry, check active state before execution, roll back insertion on spawn
  failure, and run cleanup exactly once after unlisten or destruction.
- Typed binding failures never expose an uninitialized validation error; the
  structure is zeroed and generic validation/autocomplete fallbacks are used.
- `dcc_listener_init` initializes the selected target union member's own size
  and version for event, message-command, schedule, and route kinds.
- Canonical route dispatch separates internal adapter state from public context
  user data, so middleware and error observers see the listener's user data.
- Listener destruction uses two phases to prevent cross-entry scans from
  touching freed storage.

## Transition debt and Task 14 obligation

- `include/dcc/app/legacy.h` intentionally consolidates the 1.x App surface so
  existing examples and tools continue to compile during the redesign.
- `tools/audit_project_layout.py` contains one exact
  `TRANSITION_LARGE_FILE_LIMITS` exception for that file.  Task 14 must delete
  both `include/dcc/app/legacy.h` and the matching audit exception; neither is
  intended to survive the 2.0 surface cleanup.

## Files of interest

- `include/dcc/app.h`
- `include/dcc/app/{base,options,lifecycle,listeners,context,modules,store}.h`
- `include/dcc/app/legacy.h`
- `src/app/app_listener.c`
- `src/internal/app/dcc_app_internal.h`
- `tests/app_v2_listener_smoke.c`
