# Task 5 implementer report — version core caller-owned builders

## Outcome

Implemented in `fcaf2254b098f129a40b69f3e790c1d302a54467`
(`feat: version public message builders`; 102 files, +5602/-1257). No push was
performed. The authoritative design/plan and ignored progress ledger were not
edited.

All nine in-scope values now begin, exactly, with `size_t size`, `uint32_t
version`, `uint64_t present`: message, poll, allowed mentions, embed, modal,
autocomplete choice/builder, and application-command option/builder. Each has
its own v1 constant, public presence bits, complete C/C++ initializer macro,
and null-safe allocation-free initializer. Setters are allocation-free and
validate the prefix/version plus their complete write set before writing;
failure is byte-for-byte non-mutating. The presence mask replaces redundant
`has_*` state and remains authoritative for explicit zero/false values.

The private shared ABI layer accepts covered v1/larger records, rejects
zero/short sizes, unsupported versions, unknown bits, and uncovered present
fields, and uses subtraction-based coverage checks. Optional uncovered fields
are never read. Fixed embedded leaf records and both component builder layouts
were left unchanged.

## Exact ABI layouts

Measured from the final headers with native Clang layout dumps and x86_64/i686
Windows target probes. Every listed type has alignment 8 on all three targets.

| Type | arm64 size | x64 MinGW | i686 MinGW | 64-bit min prefix | i686 min prefix |
| --- | ---: | ---: | ---: | ---: | ---: |
| message | 192 | 192 | 112 | 24 | 16 |
| poll | 96 | 96 | 80 | 24 | 16 |
| allowed mentions | 64 | 64 | 40 | 24 | 16 |
| embed | 144 | 144 | 80 | 24 | 16 |
| modal | 88 | 88 | 48 | 24 | 16 |
| autocomplete choice | 72 | 72 | 56 | 24 | 16 |
| autocomplete builder | 40 | 40 | 24 | 24 | 16 |
| command option | 168 | 168 | 112 | 24 | 16 |
| command | 120 | 120 | 72 | 24 | 16 |

Suffix field order after the common prefix is exact as follows:

- message: `content, nonce, message_reference_json, allowed_mentions_json,
  embeds_json, components_json, attachments_json, poll_json, embeds,
  embeds_count, components, components_count, components_v2,
  components_v2_count, components_v2_json, poll, allowed_mentions, sticker_ids,
  sticker_ids_count, flags, tts`
- poll: `question, answers, answer_count, duration_hours, layout_type,
  allow_multiselect`; mentions: `users, user_count, roles, role_count,
  parse_users, parse_roles, parse_everyone, replied_user`
- embed: `title, type, description, url, timestamp, footer, image, thumbnail,
  author, fields, field_count, color`; modal: `custom_id, title,
  components_json, components, components_count, components_v2,
  components_v2_count, components_v2_json`
- choice: `name, value_string, value_integer, value_number, value_type,
  abi_padding, name_localizations_json`; autocomplete: `choices, choices_count`
- option: `name, description, type, name_localizations_json,
  description_localizations_json, choices_json, choices, choices_count,
  options_json, options, options_count, channel_types, channel_types_count,
  min_integer_value, max_integer_value, min_number_value, max_number_value,
  required, autocomplete`
- command: `name, name_localizations_json, description,
  description_localizations_json, options_json, options, options_count,
  integration_types_json, contexts_json, type, default_member_permissions,
  dm_permission, nsfw, default_member_permissions_null`

`choice.abi_padding` is reserved: initializers zero it, callers must leave it
zero, and v1 validation/serialization ignores it and assigns no presence bit.
It preserves a smaller aligned i686 historical stride.

Versioned arrays now use the first element's declared `size` as checked byte
stride; they reject short/misaligned/overflowing spans and mismatched element
size/version, never `array[i]`. Historical choice/option/command/embed strides
are respectively 64/48/104/48 bytes on 64-bit and 48/32/56/32 on i686.

Registry insertion validates create semantics, normalizes into current-layout
records, and recursively deep-copies all retained strings, choices, nested
options, and channel types. Partial-OOM cleanup walks only normalized
destinations. General command serialization remains PATCH-compatible; registry
and REST create/bulk boundaries enforce required create fields. Autocomplete
validates every input and empty output slot before its first write.

## TDD and coverage

Initial RED: the new fixture could not compile because the old layouts lacked
the common prefix, constants, presence bits, and missing initializers. After
scaffolding, poisoned historical suffixes exposed direct reads, legacy `has_*`
authority, and incomplete rejection before payload access. The first broad run
was 174/178: existing mentions/listener fixtures lacked presence bits, an
autocomplete fixture violated the empty-output contract, and strict generated
compilation found incomplete initializer macros. Call sites/macros were fixed;
no contract was relaxed.

The first i686 source probe then failed three historical-stride assertions
(choice, option, embed). Align-up plus reserved choice padding fixed them. The
historical fixture poisons that padding with `0xA5A5A5A5`, proving v1 ignores
it.

`tests/builder_v2_abi_smoke.c` covers all nine initializers/layouts; larger,
short, zero, bad-version, and unknown-bit records; poisoned aligned historical
prefixes/canaries; uncovered-bit rejection without payload reads; every setter
family and no-mutation failure; zero/null/empty semantics; two-element
historical embed/choice/command/nested-option arrays and mismatch/overflow;
registry lifetime plus nested allocation-failure rollback; and App/REST/Sugar
integration.

## Changed files

The exact manifest is `git show --format= --name-only fcaf2254...`. Groups:

- public: `include/dcc/{message,embed,modal,autocomplete,application_command}.h`,
  `include/dcc/app/context.h`, and 24 migrated `include/dcc/sugar/**` headers;
- core: `src/internal/objects/dcc_builder_abi_internal.h`,
  `src/objects/builder_abi.c`, related `src/objects/*builder*` families,
  App/REST integrations, and command-registry add/identity/deep-copy sources;
- docs/examples: `docs/concepts/versioned-builders.md`, API/versioning/Sugar
  pages, `mkdocs.yml`, and two examples;
- tests/build: `tests/builder_v2_abi_smoke.c`, affected App/Sugar/HTTP/package
  fixtures, and `CMakeLists.txt`.

## Verification

- Normal configure/build passed. Final focused builder/component/registry/HTTP/
  App/Sugar/interaction/package matrix: 19/19 in 1.68 s. Full command
  `ctest --test-dir build-task5 -j4 --output-on-failure`: 178/178 in 28.71 s.
- Clean ASan/UBSan rebuild and `^dcc_builder_v2_abi_smoke$`: 1/1 in 0.09 s,
  no diagnostics.
- TSan with `--repeat until-fail:10` for builder ABI plus App listener
  concurrency: 20 executions passed in 21.42 s, no report.
- Standalone all-five-header/all-nine-initializer probes passed C11 and C++17
  with `-Wall -Wextra -Wpedantic -Werror -Wmissing-field-initializers`.
- Actual `tests/builder_v2_abi_smoke.c` passed warning-as-error compilation with
  both `x86_64-w64-mingw32-gcc` and `i686-w64-mingw32-gcc`; separate initializer
  and reversed-include-order probes also passed.
- `mkdocs build --strict`, final install to
  `/tmp/dcc-task5-install-fcaf225`, independent package-consumer configure/build/
  run, public/transition/source-package/release audits, no-target-`{0}` search,
  and `git diff --check` all passed.

Known boundaries: transition raw-JSON/legacy-component fields remain size-gated
for Task 14; Tasks 10/14 own component layouts. Task 3/4 previously documented
an out-of-scope event-waiter TSan race reached by `dcc_http_smoke`; the Task-5
TSan matrix is clean. MkDocs emits only informational notices for ignored
superpowers pages. No Task 5 failure remains.
