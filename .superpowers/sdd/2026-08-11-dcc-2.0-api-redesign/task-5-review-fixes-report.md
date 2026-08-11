# Task 5 review-fixes report — autocomplete validation

## Outcome

Both Important findings from the fresh Task 5 review are fixed in
`3e224a0f1d5140708b13af5f8f5c0a23e569e479` (`fix: align autocomplete filter
validation`). No push was performed.

## Fixes

- `dcc_ctx_autocomplete_filter_choices()` now clears required `out_count`
  before validation and validates every source record before the
  zero-output-capacity success path. A valid empty source plus zero capacity
  still returns `DCC_OK`; future/short/unknown/malformed source records cannot
  bypass validation because no output was requested.
- One private autocomplete-choice semantic validator is shared by the
  autocomplete serializer, filter preflight, and command-option choice
  serializer. It validates ABI metadata, required NAME/VALUE presence, non-NULL
  name, supported value type, non-NULL string values, finite number values, and
  non-NULL localization JSON when its presence bit is set. Command-option type
  matching remains a separate command-specific rule.
- Filter validation completes before any output record write. Invalid input
  leaves every output byte and surrounding canary unchanged; the only permitted
  observable update is documented `*out_count = 0`.
- The public context header and versioned-builder guide now state the
  zero-capacity/source-validation and required-output-count contract.

## TDD evidence

Regression cases were first added to
`tests/builder_v2_abi_smoke.c::check_autocomplete_filter_stride_and_atomicity`.
Before production changes, the focused test failed as expected:

```text
builder v2 ABI smoke: zero-capacity filter validates future-version source without mutation
0% tests passed, 1 tests failed out of 1
```

The final fixture covers valid empty source with zero capacity; future-version
first source; malformed later source; NaN, positive infinity, negative
infinity; and present-but-NULL localizations. Every invalid case requires
`DCC_ERR_INVALID_ARG`, `out_count == 0`, and an exact output/canary snapshot.
The existing limited-capacity success case remains covered.

## Files changed

- `src/app/app_context.c`
- `src/internal/objects/dcc_autocomplete_builder_internal.h`
- `src/objects/autocomplete_builder_choice_json.c`
- `src/objects/application_command_option_builder_json.c`
- `include/dcc/app/context.h`
- `docs/concepts/versioned-builders.md`
- `tests/builder_v2_abi_smoke.c`

## Verification

- Normal affected matrix: 7/7 passed in 1.77 seconds.
- Full enabled suite: 178/178 passed in 22.45 seconds.
- Clean ASan/UBSan builder ABI test: 1/1 passed in 0.06 seconds, no diagnostics.
- TSan builder ABI and listener-concurrency tests: ten runs each, 20 total,
  passed in 16.87 seconds with no report.
- Strict public-header compilation passed as C11 and C++17 with
  `-Wall -Wextra -Wpedantic -Werror -Wmissing-field-initializers`.
- x86_64 and i686 MinGW warning-as-error compilation passed for the actual
  regression fixture, App filter source, and both shared-validator serializer
  sources.
- `python3 -m mkdocs build --strict` and `git diff --check` passed.

Known Task 5 boundaries are unchanged: transition raw-JSON/legacy-component
members and component builder layout work remain owned by Tasks 10/14; the
pre-existing event-waiter TSan race documented in Tasks 3/4 is outside this
change. No new baseline failure was introduced. The authoritative design,
plan, and ignored progress ledger were not edited.
