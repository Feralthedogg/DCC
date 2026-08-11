# Task 14 brief — remove DCC 1 and migrate every repository consumer

Implement only Task 14 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The final public
surface in the design, the completed Task 6–13 contracts, and this brief are
authoritative. Where an older plan or design snapshot says 231 REST endpoints
or seven focused App headers, this brief supersedes it: the final global REST
manifest contains exactly 224 canonical Discord endpoints, and `<dcc/app.h>`
aggregates exactly eight focused headers.

Work test-first. Do not push and do not edit the ignored progress ledger. Keep
preparation commits buildable. Task 14 and Task 15 share one atomic ABI cutover;
do not commit an installed-header or exported-symbol removal while the project
still advertises ABI 1. Task 15 owns the version, SOVERSION, baseline, and
release metadata that must land in that same cutover commit.

## Outcome

Remove DCC 1 as an installed and exported API, migrate every in-tree consumer
to the completed DCC 2 surface, and leave one obvious path for each layer:

- normal bots include `<dcc/bot.h>` and use status-returning handlers, flat
  listeners, canonical context actions, and `DCC_UI_*`;
- advanced applications include the focused App headers and use the canonical
  listener registry;
- low-level code includes focused Core/REST headers and uses the request-based
  global REST surface of exactly 224 canonical endpoints;
- arbitrary REST payloads use `dcc_rest_submit()` rather than a new overload;
- no Sugar header, transition App header, App REST mirror, void-handler adapter,
  endpoint overload, or DCC 1 alias is installed or exported.

The migration guide and migration tool may name DCC 1 constructs because their
purpose is to help users leave them. Such references must be explicitly scoped
out of the banned-name documentation audit. Primary documentation, examples,
generated projects, installed headers, and package samples receive no such
exception.

## Frozen removal inventory

Treat these pre-cutover counts as a reconciliation ledger, not approximate
targets. If a prerequisite task has already removed or relocated an item,
account for it by name in the implementer report; never weaken an audit or
silently change the total.

| Surface | Frozen inventory | Final disposition |
| --- | ---: | --- |
| `include/dcc/sugar/` | 204 headers, 26,513 lines, about 1.6 MiB | Delete the entire directory. |
| Sugar macro definitions | 6,665 definition lines, 6,198 unique names | Delete; no installed compatibility aliases. |
| `<dcc/sugar.h>` preprocessed `DCC_` surface | 6,584 macros | Delete the umbrella and remove it from every aggregate/install/package path. |
| `include/dcc/app/legacy.h` | 2,899 lines, 445 unique `DCC_API` function names | Delete after the split below is complete. |
| Legacy-header `dcc_app_*` declarations | 360 | Remove 359 from the public ABI and relocate `dcc_app_run_with_signals` unchanged into `<dcc/app/lifecycle.h>`. |
| Legacy-header `dcc_ctx_*` declarations | 85 | Relocate 83 unchanged; remove exactly two. |
| Actual function removals from `legacy.h` | 361 | The 359 removed legacy `dcc_app_*` functions plus `dcc_ctx_state` and `dcc_ctx_show_modal_flow`. |
| ABI-preserving App lifecycle relocation | 1 | Keep the existing `dcc_app_run_with_signals` symbol and implementation; only its declaring header changes. |
| ABI-preserving context relocations | 83 | Declare from `<dcc/app/context.h>` with the same symbol, type, and implementation. |
| Legacy-only public types | 35 | Remove or replace consumers with the canonical focused type. |
| Legacy callback typedefs | 10 | Remove with the void-callback adapters. |
| Legacy enum/macro IDs | 47, excluding the header guard | Remove; no deprecated aliases in installed headers. |

The REST manifest is a global operation inventory, not a per-task sum copied
from an earlier baseline. Its strict final count is **224 canonical endpoints**,
owned by Tasks 6–10 in counts 41/35/47/57/44. Together with exactly 60 generic
REST operations, the installed DCC 2 surface exports 284 public `dcc_rest_*`
operations. Generic request/result/configuration operations are not endpoints.
Transition helpers, composites, aliases collapsed into canonical operations,
and removed App mirrors are not extra endpoints.

Keep the seven audited unavailable operations in a removal ledger, outside the
active endpoint records: Create Guild, Delete Guild, Modify Guild Integration,
Sync Guild Integration, Bulk Edit Guild Command Permissions, Create Guild From
Template, and the obsolete bot-facing Get Current User DMs. The first four came
from the Task 8 candidate inventory, the next two from Task 9, and the final one
from Task 10. `tools/audit_rest_v2_endpoints.py` must reject 223, 225, 61 generic
operations, an unclassified public endpoint, a duplicate operation, a stale
unavailable operation, or a legacy exception in strict mode.

## Final public header topology

`<dcc/app.h>` includes exactly these eight focused public headers, in dependency
safe order, and nothing under `include/dcc/app/` is a declaration fragment:

1. `<dcc/app/base.h>`
2. `<dcc/app/options.h>`
3. `<dcc/app/lifecycle.h>`
4. `<dcc/app/listeners.h>`
5. `<dcc/app/context.h>`
6. `<dcc/app/modules.h>`
7. `<dcc/app/store.h>`
8. `<dcc/app/env.h>`

This explicitly supersedes the earlier “exactly seven” wording. Keep `env.h`
public and rename its three inconsistent functions everywhere, with no aliases:

| Removed spelling | Canonical spelling |
| --- | --- |
| `dcc_app_load_env_file` | `dcc_app_env_load_file` |
| `dcc_app_load_dotenv` | `dcc_app_env_load_dotenv` |
| `dcc_app_validate_env_requirements` | `dcc_app_env_validate_requirements` |

Move all 83 surviving context declarations and every type they require into
the focused owner before deleting `legacy.h`. A move is not an ABI removal:
there remains exactly one public declaration and one external definition with
the original canonical name. Remove only `dcc_ctx_state` and
`dcc_ctx_show_modal_flow`; migrate their consumers to store access and the
ordered canonical modal/context action respectively.

The wait-result object owns memory but its only deinitializer currently lives
in `legacy.h`. Before deletion, add or confirm the focused canonical lifecycle
operation `dcc_ctx_wait_result_deinit()` beside the result type, migrate every
caller, and test null, zero, populated, and repeated caller-side cleanup as the
documented contract permits. Do not strand an owning value without a public
cleanup function.

Sugar currently provides ten intent groups that Core and internal code also
use. Move the canonical definitions to `<dcc/intents.h>` before deleting Sugar:
`NONE`, `DEFAULT`, `MESSAGES`, `GUILD_MESSAGES`, `DIRECT_MESSAGES`,
`MESSAGE_REACTIONS`, `MESSAGE_TYPING`, `MESSAGE_POLLS`, `PRIVILEGED`, and
`UNPRIVILEGED`. There must be one definition of each and no dependency from a
Core/App header or source file back into Sugar.

`<dcc/dcc.h>` remains the complete aggregate for framework authors and includes
the canonical `<dcc/bot.h>` surface, but it must not include `<dcc/sugar.h>` or
`app/legacy.h`. Normal examples and generated bots include `<dcc/bot.h>`
directly instead of using the broad aggregate as a shortcut.

## Isolate canonical internals before deleting adapters

Do not delete implementation files until canonical behavior has a private
owner. `src/internal/app/dcc_app_internal.h` currently depends on public legacy
route IDs, void handler/task typedefs, route policy, and route-group types.
Replace those dependencies with private App types, status-returning callback
types, and `dcc_listener_route_policy_t`. The private header must compile after
`legacy.h` is physically absent and must not redeclare a public compatibility
surface.

After moving `dcc_app_env_validate_requirements()` into `src/app/app_env.c`,
the following 12 implementation units are whole-file deletions. Together they
contain 243 legacy entry points in the frozen inventory:

| Files | Legacy entry points |
| --- | ---: |
| `app_rest_shortcuts.c`, `app_rest_shortcuts_guilds.c`, `app_rest_shortcuts_resources.c`, `app_rest_shortcuts_webhooks.c` | 196 |
| `app_route_group.c` | 15 |
| `app_messages.c` | 10 |
| `app_definition.c` | 7 |
| `app_typed_routes.c` | 7 |
| `app_state.c` | 4 |
| `app_component_sessions.c` | 2 |
| `app_extensions.c` | 1 |
| `app_view.c` | 1 |

The remaining App units need selective refactoring rather than blind deletion:

- `app_signal.c`: preserve `dcc_app_run_with_signals` and its platform signal
  implementation as the canonical lifecycle operation; remove only
  `dcc_app_run_defined_with_signals`,
  `dcc_app_run_from_env_defined_with_signals`, and
  `dcc_app_run_dotenv_defined_with_signals`;
- `app_context.c`: relocate 71 context functions and delete the one modal-flow
  adapter;
- `app_context_shortcuts.c`: relocate nine context declarations;
- `app_subcommands.c`: preserve three relocated context operations and remove
  two legacy App wrappers;
- `app_wait.c`: internalize or rename its 28 public legacy wait entry points,
  while preserving the focused wait-result lifecycle;
- `app_guards.c`: remove 35 public wrapper entry points but retain the canonical
  private policy engine;
- `app_routes.c`: remove 24 typed/void route wrappers but retain the canonical
  route registry and dispatch engine;
- `app_scheduler.c`, `app_events.c`, `app_commands.c`, `app.c`, `app_env.c`,
  `app_options.c`, and `app_message_commands.c`: strip the remaining 6, 6, 5,
  4, 2, 1, and 1 legacy wrappers respectively without deleting canonical
  behavior.

Use compiler/linker evidence to reconcile these counts. An unused compatibility
function is still an ABI export and must be removed; a private helper with the
same behavior is allowed only in an internal header, without `DCC_API`, and
without a banned public name.

## Migration tool contract

Create `tools/dcc_migrate_v2.py` and focused fixtures before implementing it.
It is a conservative lexical migration assistant, not a C rewriter. It must
recognize identifiers and include directives while preserving comments,
ordinary/string/character literals, raw user text, file mode, CRLF versus LF,
and final-newline state. Never perform an unrestricted regular-expression
replacement over a source file.

The only automatic mappings are exact tokens:

| DCC 1 | DCC 2 |
| --- | --- |
| `<dcc/sugar.h>` | `<dcc/bot.h>` |
| `DCC_SIMPLE_BOT_MAIN` | `DCC_BOT_MAIN` |
| `DCC_RESPOND_TEXT` | `DCC_CTX_REPLY_TEXT` |
| `DCC_V2_TEXT` | `DCC_UI_TEXT` |

Rewrite an identifier only as a complete token in ordinary code. Do not rewrite
its appearance in comments, strings, a larger identifier, `#define`, `#undef`,
or conditional-macro expressions such as `defined(...)`; report user-owned
macro definitions and conditional use for review. Includes must match the
exact header spelling rather than a substring in a path.

All other DCC 1 constructs are manual conversions. Emit deterministic
`path:line:column` diagnostics, with the old token and a short reason, for at
least:

- `DCC_ON_*`, `DCC_ROUTE_*`, and `DCC_DECORATE_*`, including combinatorial
  route suffixes and typed-route forms;
- response families other than exact `DCC_RESPOND_TEXT`, including
  `DCC_PUBLIC*`, `DCC_PRIVATE*`, `DCC_DONE*`, and status-discarding response
  statements; `PRIVATE` requires an explicit ephemeral-policy decision;
- every `DCC_V2_*` spelling other than exact `DCC_V2_TEXT` and all legacy
  component Sugar whose data must become a tagged `DCC_UI_*` value;
- Cog, feature, extension, view, route-group, state, decorator, and legacy
  App-definition constructs;
- App REST mirrors and endpoint-specific raw JSON, params, builder, text,
  callback, future, multipart, sync, async, and convenience overloads;
- void handlers, legacy task callbacks, and builder-to-tagged-union conversions;
- old generator string templates, which must be migrated at their source rather
  than rewritten only in generated output.

Dry-run is the default and prints a stable unified diff for safe changes. An
explicit `--write` applies only safe edits through an atomic replacement.
Manual findings remain diagnostics even when safe edits are applied. Define and
test distinct exit states for clean, safe changes pending, manual conversion
required, and I/O/parse failure. A second `--write` run must be byte-for-byte
idempotent. Never claim full migration when a canonical response status is
discarded; warn at that call site so a human can add propagation or handling.

## Repository consumer migration

Migrate source templates, checked examples, tests, and build registrations in
the same preparation series so the final header deletion is mechanical:

- all 14 example `.c` files remain in the warnings-as-errors build; ten example
  `.c` files plus `examples/discord_game_internal.h` directly include Sugar in
  the frozen tree. Normal bots move to Bot-only includes. Operational Gateway,
  voice, cluster, or hot-reload internals may use focused Core/App headers when
  they genuinely need lower layers, but never Sugar;
- all 34 `tools/dcc_new_app_*.c` units remain accounted for. Thirty currently
  emit DCC 1 source and must emit canonical DCC 2 source. The four mechanical
  units `dcc_new_app_fs.c`, `dcc_new_app_options.c`,
  `dcc_new_app_preset_dispatch.c`, and `dcc_new_app_usage.c` still participate
  in generator build and end-to-end verification;
- 37 test/support sources directly include Sugar. Preserve useful behavior in
  Bot/App/REST/component tests, then delete alias-expansion-only assertions.
  Remove or replace all 21 Sugar executable targets and all 20 registered Sugar
  CTests; do not merely mark them disabled;
- rewrite `tests/generated_app_contract.py` around generated DCC 2 projects.
  Replace `legacy_abi_layout_smoke` with a historical-prefix DCC 2 ABI fixture;
- update CMake source lists, target dependencies, install manifests, the BSD
  workflow's explicit `dcc_sugar_smoke` build, and any package consumer that
  names a deleted target or header;
- update `tools/audit_official_surface.py`, `tools/audit_source_package.py`,
  `tools/release_check.sh`, and `tools/package_release.sh` atomically with the
  deletion. None may continue to require a Sugar path;
- create `docs/migration-1-to-2.md` with the exact safe mapping table plus
  semantic migrations for status handlers, listener declarations, request
  ownership/completion, builder presence and tags, ordered interactions, and
  removal of App REST mirrors.

The install rule currently copies `include/` broadly. Verify from a newly empty
install prefix that none of `dcc/sugar.h`, `dcc/sugar/`, or
`dcc/app/legacy.h` exists. Check both binary and source archives for the same
negative set; a stale file left by a prior install is not evidence of a package
failure or success, which is why the prefix and staging directory must be
deleted before each check.

## Required RED tests and implementation sequence

1. Extend the strict surface audit with immutable seeds for every removed
   family, all 361 removed functions, the three old env spellings, App REST
   mirrors, declaration fragments, forbidden install paths, and the exact
   224-endpoint manifest count. Capture RED against the transition tree.
2. Add migration-tool fixtures for all four safe mappings, every lexical
   preservation rule, manual diagnostics, newline/mode preservation, dry-run,
   write, exit states, and idempotence. Capture RED before the tool exists.
3. Move the 83 context declarations, wait-result cleanup, intent groups, env
   functions, and private route/listener types to their final owners. Add
   standalone C11/C++17 and focused runtime tests before removing an adapter.
4. Migrate examples, generators, generated-project fixtures, package consumers,
   and behavioral tests. Build every generated preset from a clean output tree
   with warnings as errors. Remove only tests whose sole contract was an alias.
5. Prove the repository and private App implementation build without including
   `sugar.h` or `legacy.h`, then delete the 204-header Sugar tree, both umbrella
   headers, legacy sources/wrappers, and stale CMake registrations by patch.
6. In strict mode require zero removed declarations/definitions/exports, exactly
   eight App aggregate headers, the three canonical env names, 224 canonical
   REST endpoints, no transition manifest allowance, and no forbidden package
   member.

Do not satisfy a negative audit by excluding a directory, skipping Python,
allowing a missing symbol tool, disabling a target, or treating a missing
origin/main comparison as success. Every configured test build requires Python
and all manifest inputs.

## Atomic ABI cutover and commit boundary

Preparation may be split into these reviewable, buildable commits:

1. `chore: add DCC 2 migration tooling`
2. `refactor: migrate repository consumers to DCC 2`
3. `refactor: isolate DCC 1 app adapters`

Those commits retain the installed legacy headers and exported compatibility
wrappers until the coordinated cutover. The next commit is shared with Task 15:

4. `feat!: remove DCC 1 public API`

That one commit must contain the physical public-header deletion, all 361 ABI
removals, final eight-header App aggregate and env renames, project version
2.0.0, SOVERSION 2, the final DCC 2 symbol baseline, strict same-major audit,
and package/archive negative assertions. There must not be a commit where the
removals advertise 1.5.x, nor a commit where `api_v2_symbols.txt` describes a
different header tree. If Task 15's half is not ready, stop before the deletion
and hand off the prepared branch rather than creating a knowingly invalid ABI
state.

## Verification and handoff

Run at minimum from clean build, install, and package directories:

- migration-tool unit/fixture tests twice, including an idempotent `--write`;
- strict DCC 2 surface, public API, project-layout, source-package, official
  surface, REST-manifest, and release-contract audits;
- standalone compilation of every installed header as C11 and C++17;
- focused Bot/App/context/wait/env/interaction/component/REST tests;
- every example, tool, generator, generated preset, and installed package
  consumer with warnings treated as errors;
- the complete enabled CTest suite and the Task 14 focused ASan/UBSan suite;
- shared-library export inspection proving the 361 removals and all old env
  names are absent while every relocated context symbol remains;
- clean install plus binary/source package positive checks for Bot/focused
  headers and negative checks for Sugar and `app/legacy.h`.

Write `task-14-implementer-report.md` beside this brief with the RED evidence,
reconciled inventory, safe/manual migration results, moved-versus-removed
symbol lists, exact generated-project coverage, clean install/archive members,
verification commands/results, sanitizer/platform evidence, and preparation
commit hashes. Do not claim Task 14 complete until the shared cutover commit and
Task 15 release gates are green.
