# Task 13 implementer report — canonical Bot API

## Result

Task 13 is implemented on `codex/v2-api-impl`. The canonical high-level
surface is `<dcc/bot.h>` with exactly five focused leaves: handlers, listeners,
replies, UI, and main. `<dcc/dcc.h>` intentionally does not include it until
Task 14 removes the colliding transition Sugar surface.

## RED evidence

Before production headers existed, the three unregistered fixtures were added
in commit `4ad29a3`. Direct C11 compilation of both the surface and main
fixtures failed at `#include <dcc/bot.h>` with `file not found`; the transition
surface audit likewise reported `<dcc/bot.h> is missing`. The existing build
remained green.

## App and listener normalization

- Commit `7e3322d` promotes `dcc_app_env_load_file`,
  `dcc_app_env_load_dotenv`, `dcc_app_env_validate_requirements`, and the
  existing `dcc_app_run_with_signals` declaration to focused owners. The three
  old environment names remain exported transition wrappers.
- Commit `8814a4c` replaces non-final by-value listener children with borrowed
  pointers, makes cooldown a pointer child, and makes `dcc_listener_init` the
  required public static inline. Registration still validates and deep-copies
  metadata before returning.
- The eleven listener version macros became enum constants. This reduced the
  dependency macro baseline sufficiently to satisfy the hard Bot budget
  without weakening the exact Bot-owned delta.

## Canonical inventory

Commit `98cec6e` adds:

- 9 handler declaration macros;
- 19 listener stems with one short and one `_WITH` function each (38 total);
- one versioned listener config and typed handler/schedule value helpers;
- 27 state-aware context functions;
- 44 fixed/array Components v2 UI functions and exactly 9 C-only variadic UI
  macros;
- one versioned Bot config, one exported blocking runner, and exactly 4 main
  macros.

The runner normalizes historical config/options prefixes, applies development
versus production command-sync policy, resolves tokens without logging them,
registers listeners in source order, selects signal or plain run mode, and
retains ownership until App destruction eventually succeeds.

## Macro and topology evidence

Canonical POSIX Clang/static counts are:

- focused dependency baseline: 266;
- complete C11 `<dcc/bot.h>`: 296 (`266 + 30` exactly, below 300);
- complete C++17 `<dcc/bot.h>`: 287 (the exact nine variadic UI macros absent).

The complete normalized name/kind/arity/expansion records are checked in as
the three `tools/bot_v2_*macros_posix_clang*` artifacts. The surface audit
checks the five-leaf include graph, forbidden edges, exact owned C inventory,
artifacts, C++ exclusion, sentinel, arithmetic, and ceiling.

## Verification

- Fresh native build: passed.
- Complete enabled native CTest: **112/112 passed**.
- Focused tests: API surface, Bot surface, Bot runner, listener smoke,
  listener concurrency/contract, interaction queue, and Components v2 all
  passed.
- Transition surface audit: passed; remaining legacy/Sugar debt is owned by
  Task 14.
- Public API audit: passed after preserving the three exported environment
  wrappers.
- Strict standalone C11 and C++17 probes for all six Bot headers: passed.
- Four main modes compiled separately in C11 and C++17: passed.
- i686 and x86_64 MinGW `<dcc/bot.h>` probes: passed.
- ASan/UBSan focused set: 4/4 passed with leak detection disabled because the
  macOS runtime reports it unsupported.
- TSan focused set: 3/3 passed.
- `git diff --check`: passed.

Task 14 must preserve `dcc_app_run_with_signals` and remove only the three
App-definition signal wrappers named by the corrected ledger.
