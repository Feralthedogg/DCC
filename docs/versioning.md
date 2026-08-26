# Versioning and compatibility

DCC uses semantic versioning for its public C API and installed package metadata.
The version declared by `project(dcc VERSION ...)` is the single source of truth.
CMake generates `<dcc/version_generated.h>` from it, and the runtime
`dcc_version_string()` returns the same value.

## Compatibility contract

- Patch releases fix defects without intentionally changing source or binary compatibility.
- Minor releases may add APIs. Existing public symbols remain available and retain their behavior.
- Major releases may remove or redesign APIs after a documented deprecation period.
- Size-versioned caller-initialized structs may append fields. Callers set
  `size`, `version`, and the authoritative `present` mask; implementations gate
  every read and write beyond the historical minimum. Versioned arrays use the
  first record's declared byte size as their common stride.
- Public declarations use `DCC_API`; deprecated APIs use `DCC_DEPRECATED("replacement")`.

The release check compiles every standalone public header as its own translation
unit (declaration fragments are compiled through their documented umbrella),
verifies every `DCC_API` function is present in the built library when the platform
provides `nm`, and rejects public API removals relative to `origin/main`.

See [Versioned Public Builders](concepts/versioned-builders.md) for the exact
record, presence, array-stride, ownership, and command-creation rules.

## Deprecation policy

An API scheduled for removal is marked deprecated for at least one minor release.
The header message names its replacement, and the change log records the migration.
Removal requires a major-version bump unless the declaration was never shipped in a
stable release.

## LLAM compatibility

DCC 2.0.1 requires LLAM 2.2.1 or newer with ABI major 2. Configure-time checks run
for package, source-subdirectory, and prebuilt-library consumption paths.
