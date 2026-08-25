# Task 12 implementer report

Implementation commit: `585e1df` (`feat: tag and version Components v2`)

## RED evidence

- `dcc_task12_component_v2_red_contract` proved the flat builder had no
  `size/version/present`, tagged union, or structured validator.
- `dcc_component_v2_abi_smoke` then reproduced current-`sizeof` array walking
  with two 40-byte historical Text Display records in a poison-filled buffer.
- The same fixture reproduced noncanonical boolean mutation, inactive
  cross-kind setter mutation, and the removed Checkbox-label initializer.
- The historical records failed JSON construction until validator and
  serializer both adopted the caller-declared stride.

## Public ABI

- Exact common prefix: `size`, `version`, `present`, `type`, `id`, then `as`.
- Version: `DCC_COMPONENT_V2_BUILDER_VERSION_1 == 1`.
- Presence: 31 contiguous logical field indices in one `uint64_t` mask.
- The builder uses the specified Button, Select, Layout, Media, Text Input,
  Modal, Text Display, and Separator named union values.
- The parser-owned `dcc_component_v2_t` remains separate and unchanged.
- `component_types.h` now owns Button/Text Input styles, emoji, and String
  Select option leaves; `component_v2.h` no longer includes legacy
  `component.h`.

The minimum common prefix ends after `id`. Field coverage is type-specific;
the historical Text Display fixture ends after `as.text_display.content` and
is accepted without reading its poisoned suffix. Recursive arrays use their
own first element's `size`, require consistent size/version/alignment, and use
checked byte arithmetic.

## Validation and schema corrections

The public size/versioned validation error reports stable JSON-oriented paths
for prefix, presence, inactive, uncovered, and nested fields. One allowed and
required mask is enforced per active type. The implementation also enforces:

- ordinary/link/premium Button target and visible-content rules;
- String Select versus entity-select default separation;
- context-specific `required`/`disabled` use;
- Label/Text Display modal roots and tagged Label children;
- distinct Gallery choice/media leaves without group emoji state;
- File `attachment://` inputs and no File description;
- File Upload file types and count limits;
- canonical booleans, min/max relationships, duplicate IDs/custom IDs,
  placement grammar, and current component-count limits;
- no artificial channel-type count-16 ceiling.

Build and allocation-free measure share the same normalized tagged traversal.
Public setters validate tag, presence, field coverage, and arguments before
writing, including historical covered-field mutation with canary protection.

## Macro budget

The exact Task 13 dependency sentinel measured:

- Task 11 baseline (`67fd464`): 282 visible `DCC_` macros.
- Post-Task-12: 277 visible `DCC_` macros.
- Exact delta: `-5`.

The eight old `DCC_COMPONENT_V2_MAX_*` macros are now enums, including removal
of `MAX_CHANNEL_TYPES`. The final visible additions are exactly
`DCC_COMPONENT_TYPES_H`, `DCC_COMPONENT_V2_BUILDER_INIT(type_)`, and
`DCC_COMPONENT_V2_VALIDATION_ERROR_INIT`.

## Verification

- Native full build: passed.
- Full enabled CTest: 189/189 passed; 24 vendored LLAM tests disabled by the
  parent configuration.
- Focused ASan/UBSan: Components v2 ABI/smoke, REST endpoint, and ordered
  interaction queue: 4/4 passed.
- Focused TSan: Components v2 ABI and ordered interaction queue: 2/2 passed.
- Standalone C11 and C++17 probes for `component_types.h`, `component.h`, and
  `component_v2.h`: passed.
- i686 and x86_64 MinGW tagged-header/layout probes: passed.
- Public/V2/REST/source audits in the full suite: passed.
- `git diff --check`: passed.

Fresh installed-package configuration found LLAM 3.0.0/ABI 2 successfully,
but the repository's package-consumer target still contains pre-existing
Task 10/11 transition calls and opaque-flow stack construction. Components v2
field reads in that consumer were migrated here; the remaining canonical REST
and opaque ownership migration is explicitly owned by Tasks 13–14.

The unrelated user-owned
`tests/support/hot_reload_isolated_last_good_retry.c` change and
`.codex-build/` were not staged.
