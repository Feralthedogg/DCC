# Task 14 implementer report — DCC 2 public cutover

## Result

Task 14 is implemented on `codex/v2-api-impl`. Commit `46194b9` makes DCC 2
the only installed and packaged public surface, migrates every repository-owned
consumer, and records the breaking major-version boundary.

## Migration and cutover

- The migration inventory and `dcc-migrate-v2` tooling were defined and tested
  before the cutover.
- All 14 examples compile against the canonical App, Bot, REST, and Components
  v2 APIs without including Sugar.
- `<dcc/dcc.h>` now exposes the canonical Bot layer.
- `<dcc/app.h>` contains exactly the eight focused App leaves: base, options,
  context, lifecycle, env, modules, listeners, and store.
- DCC 1 Sugar, App legacy adapters, and REST compatibility helpers are not
  installed or included in binary or source packages. Internal-only adapters
  retain hidden visibility where the implementation still needs them.
- The package consumers are standalone C11 and C++17 DCC 2 programs.

## Public contract evidence

- Canonical public API baseline: **1,239 declarations**.
- Shared-library export inventory: **1,239 symbols**, with zero missing and zero
  extra exports.
- REST contract: **224 endpoint functions + 35 generic functions = 259**.
- The REST firewall is opaque and uses create/destroy ownership.
- Result access is exposed through the nine typed `dcc_rest_result_*_field`
  helpers.
- The installed App directory contains exactly eight headers; installed and
  packaged trees contain no Sugar or legacy path.

## Verification

- Native release suite: **121/121 passed**, including every hot-reload process
  integration test.
- Installed C11/C++17 CMake consumer: configured, built, and ran.
- Installed pkg-config consumer: compiled, linked, and ran.
- i686 and x86_64 MinGW umbrella-header probes: passed.
- C11 and C++17 umbrella-header probes: passed.
- API/reference generators report all 10 contract artifacts and 175 reference
  pages current.
- `git diff --check`: passed.
