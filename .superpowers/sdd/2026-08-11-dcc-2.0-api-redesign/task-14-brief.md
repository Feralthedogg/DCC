# Task 14 brief — remove DCC 1 and migrate every repository consumer

Implement only Task 14 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The final public
surface in the design, the completed Task 6–13 contracts, and this brief are
authoritative. Where an older plan or design snapshot says 231 REST endpoints
or seven focused App headers, this brief supersedes it: the final global REST
manifest contains exactly 224 canonical Discord endpoints, and `<dcc/app.h>`
aggregates exactly eight focused headers.

Work test-first. Do not push and do not edit the ignored progress ledger. Keep
preparation commits buildable. Task 14 and Task 15 share one atomic public DCC
2 cutover. While the project advertises ABI 1, do not physically apply any
Task 14 public removal, signature replacement, or incompatible record/layout
replacement. The held set includes Sugar and `app/legacy.h`, the 361 frozen
App/context exports, the 61-to-35 generic REST cut, OAuth2 and firewall
replacements, response-helper replacement, component-builder/header removal,
message/modal raw-field removal, the command-registry/component-session/replay/
store opaque-state conversions, the complete bounded-writer record cut, and
every same-name ABI replacement below.
Coexisting additions, private canonical engines, migration tooling, and
repository-consumer migrations may land as preparation. Task 15 owns the
version, SOVERSION, final baselines, and release metadata that land with the
entire held set in one cutover commit. Tasks 6–13 already on the unreleased 2.0
development branch are outside this cutover boundary.

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
targets. Prerequisite tasks may relocate a retained symbol or isolate its
private implementation, but every one of the frozen 361 removal names,
including all 196 App REST mirrors, must still be declared and exported
immediately before the shared cutover. Account for every relocation by name in
the implementer report; never weaken an audit or silently change the total.

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
owned by Tasks 6–10 in counts 41/35/47/57/44. The Task 14 cut leaves exactly
**35 externally exported generic REST symbols**, so the installed DCC 2 shared
library exports exactly **259 public `dcc_rest_*` symbols**. Three additional
public callables—`dcc_rest_call_options_init`,
`dcc_rest_request_desc_init`, and `dcc_rest_firewall_options_init`—are
same-name `static inline` initializers and are deliberately not external
symbols or manifest generic operations.

The exact final generic external set is:

```text
dcc_rest_firewall_attach
dcc_rest_firewall_check
dcc_rest_firewall_create
dcc_rest_firewall_decision_string
dcc_rest_firewall_destroy
dcc_rest_firewall_detach
dcc_rest_firewall_hard_limit_action_string
dcc_rest_firewall_options_validate
dcc_rest_firewall_reason_string
dcc_rest_firewall_record_response
dcc_rest_firewall_request_is_critical
dcc_rest_firewall_snapshot
dcc_rest_firewall_snapshot_json
dcc_rest_firewall_state_snapshot
dcc_rest_firewall_status_is_invalid
dcc_rest_method_name
dcc_rest_request_cancel
dcc_rest_request_completed
dcc_rest_request_destroy
dcc_rest_request_wait
dcc_rest_result_bool_field
dcc_rest_result_channel_id
dcc_rest_result_clone
dcc_rest_result_free
dcc_rest_result_guild_id
dcc_rest_result_i64_field
dcc_rest_result_message_id
dcc_rest_result_ok
dcc_rest_result_snowflake_field
dcc_rest_result_status
dcc_rest_result_string_field
dcc_rest_result_string_field_len
dcc_rest_result_u64_field
dcc_rest_runtime_stats
dcc_rest_submit
```

Generic operations are not endpoints. Transition helpers, composites, aliases
collapsed into canonical operations, and removed App mirrors are not extra
endpoints. Strict audit rejects 34, 36, the old transition count 60, any
missing/extra exact name, or an inline initializer misclassified as an export.

Keep the seven audited unavailable operations in a removal ledger, outside the
active endpoint records: Create Guild, Delete Guild, Modify Guild Integration,
Sync Guild Integration, Bulk Edit Guild Command Permissions, Create Guild From
Template, and the obsolete bot-facing Get Current User DMs. The first four came
from the Task 8 candidate inventory, the next two from Task 9, and the final one
from Task 10. `tools/audit_rest_v2_endpoints.py` must reject 223, 225, any
generic count other than 35, an unclassified public endpoint, a duplicate operation, a stale
unavailable operation, or a legacy exception in strict mode.

## Final generic REST and result-only cut

Remove these exact transition families from external ABI and installed
headers: the six `dcc_rest_async_{cancel_active,cancel_all,cancel_pending,
pending,status,wait}` controls; `dcc_rest_future_{completed,destroy,wait}`;
the 14 `dcc_rest_request*` raw/method/multipart callback, async, and future
overloads below; and the external
`dcc_rest_call_options_init`, `dcc_rest_request_desc_init`, and
`dcc_rest_firewall_options_init` definitions. The three initializers retain the
same public source spelling only as caller-header `static inline` functions so
they write the caller's compiled layout rather than a newer library `sizeof`.

```text
dcc_rest_request
dcc_rest_request_async
dcc_rest_request_async_priority
dcc_rest_request_future
dcc_rest_request_future_priority
dcc_rest_request_method
dcc_rest_request_method_async
dcc_rest_request_method_async_priority
dcc_rest_request_method_future
dcc_rest_request_method_future_priority
dcc_rest_request_method_multipart
dcc_rest_request_method_raw
dcc_rest_request_multipart
dcc_rest_request_raw
```

Delete `dcc_rest_response_t`, `dcc_rest_cb`, `dcc_rest_future_t`, and
`dcc_rest_async_status_t`, plus the legacy core async/future/request headers and
their source units. Every retained completion consumes `dcc_rest_result_t` via
`dcc_rest_result_fn` or a documented domain-specific callback containing that
same result. The Task 9 registry, Task 10 managed-message composite, Task 11
context/flow/helpers, and the OAuth2 migration below are exact negative-audit
owners; no public header may keep the old types alive transitively.

Rename and retarget all nine response accessors to the `dcc_rest_result_*`
names in the exact generic list. Their parameter lists remain otherwise
equivalent, including `int *` for the boolean helper. One common validator
requires a covered version-1 result/body prefix, rejects invalid pointer/length
pairs, embedded-NUL or trailing JSON, duplicate top-level keys, and a nonobject
root. It initializes numeric/snowflake outputs to zero, string output to an
empty string, and `out_len` to zero before parsing and on every failure. String
capacity includes the terminator. Sensitive results remain readable to the
explicit callback/wait owner, while clone/free and observer redaction obey Task
7's secure-body contract.

The standalone firewall becomes opaque:

```c
typedef struct dcc_rest_firewall dcc_rest_firewall_t;
DCC_API dcc_status_t dcc_rest_firewall_create(
    const dcc_rest_firewall_options_t *options,
    dcc_rest_firewall_t **out_firewall);
DCC_API void dcc_rest_firewall_destroy(dcc_rest_firewall_t *firewall);
```

Remove `dcc_rest_firewall_init`/`deinit`; create clears the output on failure
and destroy consumes one owner. The firewall snapshot no longer embeds the
removed extensible `dcc_rest_async_status_t` by value. It reports only
firewall-owned counters/policy; callers obtain the coherent request/queue
snapshot through Task 11's `dcc_rest_runtime_stats()`. This deliberately
replaces the old global wait/bulk-cancel/status surface with per-request
cancel/wait, client stop/destroy, and read-only runtime telemetry; document the
operational tradeoff in the migration guide.

The firewall snapshot is this exact version-1, size-gated output; policy values
are flattened rather than embedding evolvable options:

```c
typedef struct dcc_rest_firewall_snapshot {
    size_t size;
    uint32_t version;
    uint32_t invalid_request_soft_limit;
    uint32_t invalid_request_hard_limit;
    uint64_t invalid_request_window_ms;
    uint32_t soft_limit_delay_ms;
    dcc_rest_firewall_hard_limit_action_t on_hard_limit;
    uint64_t window_started_at_ms;
    uint64_t window_ends_at_ms;
    uint64_t invalid_request_count;
    uint64_t total_requests;
    uint64_t total_invalid_requests;
    uint64_t total_delayed_requests;
    uint64_t total_rejected_requests;
    uint8_t soft_limit_exceeded;
    uint8_t hard_limit_exceeded;
    uint8_t attached;
} dcc_rest_firewall_snapshot_t;
```

Its VERSION value is an enum and it has one complete constant initializer
macro, not a fourth REST initializer function. Snapshot writes are bounded by
the caller's size and include only fully covered fields from one firewall lock
snapshot. `invalid_request_count` becomes fixed-width `uint64_t`; there is no
embedded options/status record and no pointer lifetime.

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

## Builder and Components cutover

Delete `include/dcc/component.h`, its private header, all 13
`src/objects/component_builder_*.c` units, and these exact 21 external symbols:

```text
dcc_component_builder_build_array_json
dcc_component_builder_build_json
dcc_component_builder_init
dcc_component_builder_json_free
dcc_component_builder_set_children
dcc_component_builder_set_custom_id
dcc_component_builder_set_disabled
dcc_component_builder_set_emoji
dcc_component_builder_set_label
dcc_component_builder_set_max_length
dcc_component_builder_set_max_values
dcc_component_builder_set_min_length
dcc_component_builder_set_min_values
dcc_component_builder_set_options
dcc_component_builder_set_placeholder
dcc_component_builder_set_required
dcc_component_builder_set_sku_id
dcc_component_builder_set_style
dcc_component_builder_set_text_input_style
dcc_component_builder_set_url
dcc_component_builder_set_value
```

Shared button/text-input/emoji/select leaf types remain owned only by Task 12's
`<dcc/component_types.h>`. No final message, modal, component-session, aggregate,
package, or source includes `component.h`.

ABI-replace `dcc_component_session_button` so its final output is
`dcc_component_v2_builder_t *`; remove `dcc_component_session_button_v2`
instead of retaining a suffix alias. The returned tagged Button is mode-neutral:
placing it in a normal message does not force the Components-v2 flag.

The final message builder has this exact version-1 member order after
`size, version, present`:

```c
const char *content;
const char *nonce;
const dcc_message_reference_t *message_reference;
const dcc_allowed_mentions_builder_t *allowed_mentions;
const dcc_embed_builder_t *embeds;
size_t embed_count;
const dcc_component_v2_builder_t *components;
size_t component_count;
dcc_message_component_mode_t component_mode;
const dcc_message_attachment_t *attachments;
size_t attachment_count;
const dcc_poll_builder_t *poll;
const dcc_snowflake_t *sticker_ids;
size_t sticker_id_count;
uint64_t flags;
uint8_t tts;
```

`dcc_message_component_mode_t` has exact values NORMAL=0 and V2=1. The final
presence enum has, in the field order above, CONTENT, NONCE,
MESSAGE_REFERENCE, ALLOWED_MENTIONS, EMBEDS, COMPONENTS, ATTACHMENTS, POLL,
STICKER_IDS, FLAGS, and TTS. `dcc_message_builder_set_components()` takes the
tagged pointer/count, selects NORMAL, validates
`DCC_COMPONENT_V2_CONTEXT_MESSAGE_LEGACY`, and atomically clears only
`DCC_MESSAGE_FLAG_IS_COMPONENTS_V2`. It accepts only Action Row roots containing
buttons or exactly one select. `dcc_message_builder_set_components_v2()` stores
the same pointer/count, selects V2, validates the V2 grammar, and atomically
sets the flag. Both preserve other flags and support a null/zero explicit empty
array. `dcc_message_builder_set_flags()` rejects a value inconsistent with an
already present component mode. Final build/preflight revalidates mode, root
grammar, flag, and Discord content/embed/poll/sticker exclusions through Task
12's one traversal; mixed or ambiguous mode never serializes.

The final modal builder contains only `size, version, present, custom_id,
title, components, component_count`, where components are tagged Task 12
values. ABI-replace `dcc_modal_builder_set_components` with that type and
remove `dcc_modal_builder_set_components_v2`; modal serialization always uses
`DCC_COMPONENT_V2_CONTEXT_MODAL`.

Delete exactly 19 transition raw-JSON members, their 19 presence names, and
the 15 exported raw setters:

- message (7): `message_reference_json`, `allowed_mentions_json`,
  `embeds_json`, `components_json`, `attachments_json`, `poll_json`, and
  `components_v2_json`, plus all seven matching setters;
- modal (2): `components_json` and `components_v2_json`, plus both setters;
- autocomplete choice (1): `name_localizations_json` and its setter;
- application-command option (4): `name_localizations_json`,
  `description_localizations_json`, `choices_json`, and `options_json` (these
  have no exported setters); and
- application-command builder (5): `name_localizations_json`,
  `description_localizations_json`, `options_json`, `integration_types_json`,
  and `contexts_json`, plus all five matching setters.

Task 7 supplies typed message reference/attachment replacements, Task 9
supplies typed localization/integration/context replacements, and existing
typed mentions/embeds/poll/choices/options remain. JSON output build/free
functions are retained. For autocomplete/application-command records, remove
the raw members while preserving the relative order of every remaining field
and the exact typed suffix defined by Task 9. Convert message/modal VERSION,
PRESENT, and integral flag constants to enums; retain only constant initializer
macros. Required tests cover content+button, embed+select, V2 layout+content
rejection, flag/mode switching, empty arrays, types 9+ in normal mode, modal
grammar, and every raw-name negative seed.

## OAuth2 joins the request/result model

The five existing OAuth2 helpers are synchronous `dcc_http_perform()` calls
with an inline `dcc_oauth2_cb(dcc_rest_response_t *)`; changing only the
callback typedef would leave a second execution model. Remove
`dcc_oauth2_cb` and replace the five signatures exactly:

```c
dcc_status_t dcc_oauth2_get_current_authorization(
    dcc_client_t *client,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);
dcc_status_t dcc_oauth2_get_current_bot_application(
    dcc_client_t *client,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);
dcc_status_t dcc_oauth2_exchange_code(
    dcc_client_t *client,
    const dcc_oauth2_exchange_code_options_t *input,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);
dcc_status_t dcc_oauth2_refresh_token(
    dcc_client_t *client,
    const dcc_oauth2_refresh_token_options_t *input,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);
dcc_status_t dcc_oauth2_revoke_token(
    dcc_client_t *client,
    const dcc_oauth2_revoke_token_options_t *input,
    const dcc_rest_call_options_t *options,
    dcc_rest_request_t **out_request);
```

The three input records use the exact `size, version, present` prefix and
version 1. Exchange then contains `client_id, client_secret, code,
redirect_uri, code_verifier`; refresh contains `client_id, client_secret,
refresh_token`; revoke contains `client_id, client_secret, token,
token_type_hint`. The exchange presence enum has exactly
`DCC_OAUTH2_EXCHANGE_CODE_PRESENT_CODE_VERIFIER` at bit 0. A present verifier
is encoded as `code_verifier`, is 43..128 ASCII characters from
`ALPHA / DIGIT / "-" / "." / "_" / "~"`, and is otherwise omitted. This
helper remains the confidential-client flow, so its other exchange fields,
including `client_secret`, are required and nonempty. Only revoke's optional
hint has its own presence bit and it is exactly `access_token` or
`refresh_token`. VERSION/PRESENT values are enums; each type has a constant
initializer macro and same-name null-safe static-inline initializer.

Current Authorization requires a covered BEARER mode/token. Current Bot
Application accepts DEFAULT/BOT and uses the client's bot token. Token,
refresh, and revoke endpoints accept DEFAULT/NONE, reject audit reason and
auth-token override, and send exact form-urlencoded bodies. A client created
with an empty bot token is a supported OAuth-only runtime owner, so no second
runtime type is introduced.

All five submit through Task 4's queue and use the ordinary
`dcc_rest_result_fn` from call options: local rejection clears the handle and
calls nobody; `DCC_OK` returns before a delayed response and guarantees one
terminal result with cancel/wait/destroy behavior. Copy every ID, secret, code,
redirect URI, refresh/access/revoke token, and form byte before return. Force
sensitive-request handling for all secret-bearing forms/headers and
sensitive-result handling for exchange/refresh token responses. Task 7's bit
must reach URL parts, headers, wire request/response, parsed result, clones,
observer redaction, retry, interceptor, and secure-free paths.
`code_verifier` is secret-bearing input under that same rule: copy it before
return, percent-encode it exactly once, include it only when present, and wipe
the source copy plus every retired/final form allocation.

Focused tests use a delayed local server and cover stack mutation, exact form
encoding, invalid/embedded input, callback/output pairing, return-before-
response, cancel/destroy/shutdown exactly once, HTTP/transport errors, App and
public observer redaction, interceptor behavior, clone/free, every forced
buffer move, ASan, and allocator-hook proof that retired as well as final
secret blocks were zeroed. Exchange cases include verifier absent, exact
43/128-character bounds, 42/129 and invalid-character rejection, covered-but-
absent omission, present-but-uncovered rejection, and mutation/free immediately
after successful admission.

## Complete public-record and initializer audit

Create the hand-reviewed `tools/api_v2_records.json`. The old mixed classification
is forbidden because layout, call direction, and ownership are independent.
Every public struct/union has exactly one value on each of these axes:

```text
layout         opaque | fixed | size_extensible | versioned
direction      input | output | inout
lifetime       trivial | borrowed | owning | state_handle
writer_policy  none | full_fixed | covered_atomic | opaque_publish
append_policy  not_applicable | forbidden | suffix_same_layout | new_version_only
```

Each row also names owner header, complete stable-prefix fields/types/order,
initializer, cleanup/consume operation, initializer-empty predicate when
owning, by-value child edges, and array stride policy (`not_arrayed`,
`fixed_sizeof`, or `declared_stride`). Every extensible owning producer also
names an `ownership_anchor`, `minimum_success_size`, and the complete logical
view/count group that must be covered before success. `state_handle` requires `layout=opaque`;
`full_fixed` requires `layout=fixed`; `covered_atomic` requires
`size_extensible` or `versioned`; and `opaque_publish` requires `opaque` plus
`state_handle`. The generator rejects an impossible combination. The preflight
inventory contains 177 size-first, non-versioned records; none may remain
unclassified merely because an earlier task did not touch its feature.

All of that metadata is compatibility-significant. For an existing record in a
later ABI-2 release, `direction`, `lifetime`, `writer_policy`,
`append_policy`, array-stride policy, initializer identity, initializer-empty
predicate, cleanup/consume identity, ownership anchor, minimum success size,
and logical view/count group remain exact. Existing field names, types, order,
offsets, semantic meaning, units/ranges, ownership, and producer validity
boundary also remain exact even when suffix growth is permitted. Changing any
lifecycle axis requires a separately named new public type/API; regenerating
the current manifest cannot bless the change.

DCC 2 deliberately supports two extensibility models:

- a simple size-only record has an implicit layout generation 1. Every covered
  prefix field's name, type, order, and meaning is permanent for ABI major 2;
  only suffix append is allowed, and implementations read/write only fully
  covered fields;
- a semantic multi-layout record starts `size, version` and follows its
  documented presence/null rules. Existing version prefixes remain immutable;
  a new version is explicit rather than inferred from `sizeof`.

This replaces the overbroad claim that every evolvable record must contain a
version field. `layout=fixed` records never grow. `lifetime=borrowed` rows
document their producer lifetime; `lifetime=owning` rows name exact cleanup;
opaque state handles use create/destroy. Lifetime never decides layout
compatibility. Audit fails an unclassified aggregate, a whole-
struct caller read/copy, `size >= sizeof(current)` where a historical prefix is
valid, a partial logical-field read, or a serializer using current `sizeof` as
array stride.

No external allocation-free initializer may write the current library's
`sizeof(type)` into caller storage. Convert such initializers to same-name
public `static inline` functions (caller-header size), add an explicit storage
size only where inline initialization is impossible, or make stateful storage
opaque create/destroy. At minimum this removes the nine external Task 5
initializers for message, poll, allowed mentions, embed, modal, autocomplete
choice/builder, and application-command option/builder; Task 11 already makes
flow opaque, Task 12 removes external component initializers, and the final
REST section handles request/firewall options. The manifest/audit discovers
and forces a disposition for every other public `_init` rather than relying on
that minimum list.

The writer audit is broader than `_init`. Inventory every external or public
static-inline function that initializes, mutates, parses into, builds into,
snapshots into, opens into, resets, deinitializes, or closes a public record.
Every such writer follows its manifest `writer_policy`:

1. `full_fixed`: a `layout=fixed` record whose complete layout and alignment are
   frozen for ABI major 2, so a full-layout write is valid regardless of
   lifetime;
2. `covered_atomic`: a `size_extensible`/`versioned` record whose caller first initializes the header
   with its own compile-time `sizeof`; the library captures that declared size
   and version before any mutation, reads or writes only fields whose complete
   byte range is covered, validates logical field groups atomically, constructs
   in private temporaries, and commits only covered fields; or
3. `opaque_publish`: an opaque state handle published through
   `create/open(..., T **out)` and consumed by its documented `destroy/close`
   operation; or
4. `none`: no public function writes the record.

No public writer may use whole-current-layout assignment, `memcpy`, or
`memset(sizeof(current_type))` for `covered_atomic`. Except for pointer outputs, which
are cleared before validation, failure leaves the caller's initialized record
byte-for-byte unchanged. A covered owning deinitializer captures `size` and `version`
when present before releasing ownership, obtains suffix allocation metadata from the
covered private state rather than an uncovered public field, wipes/zeros only
`min(declared_size, sizeof(current_type))`, then restores the captured header;
it is safe to call again. This rule covers the concrete `_init`, setter,
parse/snapshot/build output, reset, `_deinit`, `open`, and `close` families, not
just names matching one suffix. Separately compiled synthetic smaller-DCC2-
prefix/new-library canaries cover
at least one initializer, mutator, successful and failing parser/builder,
snapshot, owning deinitializer, opaque open/create, and opaque close/destroy
from every affected header family.
For each extensible owning output, the successful short-prefix canary covers at
least `minimum_success_size`; canaries ending before or inside the ownership
anchor or required view/count group fail before allocation and remain bytewise
unchanged.

`lifetime=owning` plus `writer_policy=covered_atomic` output producers never
perform implicit replacement.
Their manifest entry defines an initializer-empty predicate: every covered
private ownership pointer is null and every covered owned view/count is the
initializer value. A producer accepts only a valid initialized-empty output.
Success additionally requires complete coverage of the row's ownership anchor
and its logical view/count group. A shorter size or partially covered anchor/
group returns `DCC_ERR_INVALID_ARG` before allocation, parsing, traversal, or
stream consumption and leaves the record bytewise unchanged.
If ownership is already populated it returns `DCC_ERR_STATE` before allocation,
input parsing, registry traversal, or stream consumption and leaves every caller
byte unchanged. On any other failure the empty output also remains bytewise
unchanged; only full success atomically commits the temporary ownership and
covered views. Reuse requires the caller to run the exact idempotent deinitializer
and then the caller-compiled initializer. No API silently frees/replaces an old
value.

This rule applies explicitly to
`dcc_command_registry_remote_snapshot_parse_json()`,
`dcc_command_registry_build_plan()`, and `dcc_replay_player_next()`. For replay,
populated-output rejection occurs before reading a byte so it cannot skip a
record; EOF likewise leaves an initialized-empty output unchanged. RED/GREEN
tests for all three cover initialized-empty success, populated rejection,
deinit+reinitialize reuse, parse/build/EOF failure, allocation failpoints, and
surrounding old-prefix canaries. The manifest generates the same matrix for any
other owning-output producer it discovers.

Build the by-value dependency graph. An append-extensible child may not be
embedded before any later parent field: growing it would shift the parent's
offsets even though both records pass their own size check. A by-value child is
legal only when fixed-layout/fixed-alignment for ABI 2 or when it is the
parent's frozen final tail under an explicit alignment contract. Otherwise
replace it in this major cut with a borrowed pointer, flatten the stable
fields, or define explicit per-version offset normalization. Audit the known
App/client, listener/config/policy/target, command-sync/registry,
cluster/client, hot-reload/client, firewall/snapshot, supervisor/result, store,
voice, module, and interaction parents instead of granting exceptions. The
listener graph must match Task 13 exactly: evolvable non-final children are
borrowed pointers copied synchronously by `dcc_app_listen()`, fixed array
leaves have frozen stride, and the fixed target union is the listener's final
tail.

### Exact disposition of the known aggregate graphs and state owners

The manifest, tests, and final headers implement these decisions; none is
deferred to the audit author:

1. **App options and command sync.** Change
   `dcc_app_command_sync_options_t::command_registry` to
   `const dcc_command_registry_options_t *`. In `dcc_app_options_t`, change
   `client`, `command_registry`, and `command_sync` to pointers to their
   corresponding const records, preserving their relative field positions and
   every following scalar/string field. Null command-registry/sync pointers
   select documented disabled/default values. Direct App creation requires a
   usable client-options pointer; the Bot runner supplies a normalized local
   client record after token resolution. `dcc_app_create()` validates each
   independent prefix and deep-copies every covered nested option/string before
   returning. Both App option initializers become same-name public static-inline
   functions.
2. **Cluster.** Change `dcc_cluster_options_t::client_options` to
   `const dcc_client_options_t *` and
   `dcc_cluster_remediation_loop_options_t::remediation` to
   `const dcc_cluster_remediation_options_t *`; creation/loop entry validates
   and synchronously copies them. Freeze, for ABI major 2, the exact layout and
   alignment of `dcc_cluster_health_summary_t`, `dcc_cluster_shard_info_t`,
   `dcc_cluster_restart_policy_t`, `dcc_cluster_supervisor_action_t`,
   `dcc_cluster_supervisor_status_t`, `dcc_cluster_recovery_plan_t`, and
   `dcc_cluster_remediation_result_t`. Their existing `size` members remain
   validation metadata but do not permit same-major growth. Top-level
   supervisor event/decision-event and remediation-loop result may append only
   after their current final field because every embedded child is frozen.
3. **Hot reload.** Change
   `dcc_hot_reload_options_t::worker_canary_options` to a nullable
   `const dcc_hot_reload_canary_options_t *`; null selects the standard policy.
   Change both children of `dcc_hot_reload_run_options_t` to
   `const dcc_client_options_t *client_options` and
   `const dcc_hot_reload_options_t *hot_reload_options`. Entry points validate
   and deep-copy both graphs. Remove the embedded options record from
   `dcc_hot_reload_canary_state_t`; after `size`, flatten the policy fields in
   their original order (`canary_percent`, `promote_after_ms`,
   `max_error_rate`, `max_consecutive_failures`) followed by the existing
   candidate/counter/decision/active fields. State functions gate every write
   by caller size. The canary-options initializer becomes same-name static
   inline.
4. **Firewall.** Make the firewall opaque and use the exact flattened snapshot
   above. Options remain size-extensible input and use the one public
   static-inline REST firewall-options initializer.
5. **Store.** Make `dcc_store_t` opaque. Replace stack init/deinit with
   `dcc_store_create(options, &out)` and `dcc_store_destroy(store)`; change
   `dcc_store_file_open` and `_options` outputs to `dcc_store_t **`, clearing
   them on failure. App store access remains a borrowed opaque pointer. Custom
   callback and user-data values are copied, and cleanup runs once.
   `dcc_store_managed_message_binding_t` keeps a borrowed store pointer and its
   initializer becomes same-name static inline. Delete the old external
   `dcc_store_init`/`deinit` signatures and migrate every stack store owner.
6. **Wait, voice, and interaction telemetry.** Freeze
   `dcc_event_snapshot_t` for ABI major 2 because it is embedded and arrayed.
   Freeze `dcc_voice_session_info_t`, `dcc_voice_client_stats_t`,
   `dcc_voice_dave_stats_t`, and `dcc_voice_health_snapshot_t`; the connect-wait
   result may append only after its current final health field. Freeze
   `dcc_interaction_server_state_t`, `dcc_interaction_server_stats_t`, and
   `dcc_interaction_server_protection_stats_t`; the health snapshot may append
   only after its current final protection field. Fixed telemetry may gain a
   separately named/versioned API in ABI 2, but its existing layout never
   grows. `dcc_ctx_wait_result_deinit()` remains the exact cleanup for the
   owning App wait result.
7. **Auto moderation.** Change
   `dcc_auto_moderation_rule_params_t::trigger_metadata` to
   `const dcc_auto_moderation_metadata_params_t *`; presence/trigger rules
   decide whether null is valid, and endpoint submission serializes it before
   return. Action arrays retain Task 9's fixed element-stride contract.
8. **Intentional tails.** Task 7 replaces
   `dcc_rest_request_desc_t::options` with a pointer, so no extensible call-
   options record remains embedded. `dcc_rest_interaction_response_t::data` and
   `dcc_listener_t::target` are likewise frozen final unions. A same-major
   variant may not enlarge or raise the alignment of either union.
9. **Parser/object graphs.** Existing by-value user/member/emoji/location and
   other parser-owned object leaves have fixed ABI-2 layout. Enclosing parser
   rows use `layout=fixed` and the accurate `borrowed`/`owning`/`trivial`
   lifetime, never a mixed label or silent append-extensible array. Task 5's
   embed/poll/component emoji leaves
   remain fixed as already specified.
10. **Modules and other named graphs.** `dcc_app_module_t` has no by-value
    extensible child; registration deep-copies its name and pointer values and
    it remains size-extensible input. App store access, voice client, cluster,
    interaction server, flow, firewall, store, command registry, component
    session/store/listener, and replay recorder/player runtime objects are
    opaque.
    The record generator must find no remaining unlisted by-value edge to an
    append-extensible child; discovery is a failing design defect, not an
    allowlist update.
11. **Command registry state.** Remove the public
    `dcc_command_registry_entry_t`; its by-value command-builder tail is an
    internal deep-copied entry and no public declaration consumes that type.
    Forward-declare `dcc_command_registry_t` and replace init/deinit with
    `dcc_command_registry_create(dcc_command_registry_t **out)` and
    `dcc_command_registry_destroy(dcc_command_registry_t *registry)`. Create
    clears `*out` before validation and publishes one handle only on `DCC_OK`;
    destroy is null-safe and consuming. App owns an opaque registry pointer.
    Registry options use `layout=size_extensible`, `direction=input`, their
    accurate trivial/borrowed lifetime, `writer_policy=covered_atomic`, and
    `append_policy=suffix_same_layout`;
    `dcc_command_registry_options_init()` becomes static inline. Remote snapshot
    and plan remain public inspection records, not runtime handles: add caller-compiled
    `dcc_command_registry_remote_snapshot_init()` and
    `dcc_command_registry_plan_init()`, require them before parse/build, write
    only covered fields, and retain their exact bounded, null-safe
    deinitializers. Parse/build accept only initializer-empty outputs under the
    no-implicit-replacement rule. Successful Task 9 apply still deep-copies executable data,
    so either owning value may be deinitialized immediately after admission.
12. **Component-session state.** Forward-declare
    `dcc_component_session_t`, `dcc_component_session_store_t`, and
    `dcc_component_session_listener_t`. Change session creation to
    `dcc_component_session_create(options, dcc_component_session_t **out)` and
    replace deinit with null-safe consuming `dcc_component_session_destroy()`.
    Add `dcc_component_session_store_create(dcc_component_session_store_t
    **out)` and consuming `dcc_component_session_store_destroy()`; remove the
    stack init/deinit pair. Both client registration functions publish
    `dcc_component_session_listener_t **out_listener`, clear it on failure, and
    `dcc_client_off_component_session(client, listener)` consumes the listener
    only on `DCC_OK` so a failure remains retryable. Options, check, result, and
    listener-options records stay size-extensible input/output records;
    `dcc_component_session_options_init()`,
    `dcc_component_session_check_init()`,
    `dcc_component_session_result_init()`, and
    `dcc_component_session_listener_options_init()` are caller-compiled static
    inline. Setters/verifiers obey the complete public-writer rule above. App registrations borrow the
    opaque session or store through their documented registration lifetime;
    they never copy or free the handle. Task 14 removes the legacy component
    button output and makes `dcc_component_session_button()` emit the final
    tagged Button type; `_button_v2` is deleted rather than aliased.
13. **Replay state.** Forward-declare `dcc_replay_recorder_t` and
    `dcc_replay_player_t`. The exact open signatures become
    `dcc_replay_recorder_open(const char *path, dcc_replay_recorder_t **out)` and
    `dcc_replay_player_open(const char *path, dcc_replay_player_t **out)`, with
    cleared failure output; their existing null-safe `close(handle)` names
    become consuming opaque destructors. `dcc_replay_record_t` remains a
    bounded public owning record with caller-compiled
    `dcc_replay_record_init()` and bounded, idempotent
    `dcc_replay_record_deinit`; `dcc_replay_validation_result_t` remains a
    size-extensible output initialized by static-inline
    `dcc_replay_validation_result_init()`. Player-next and file-validation
    require initialized outputs and commit only after a fully successful
    parse/validation result. Player-next additionally requires an empty replay
    record and never consumes input on populated-output rejection.

The three public owning-output rows are exact and remove all checker discretion:

| Record | layout | direction | lifetime | writer | append | stable prefix, in order | ownership anchor / minimum success | initializer-empty | cleanup |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `dcc_command_registry_remote_snapshot_t` | `size_extensible` | `output` | `owning` | `covered_atomic` | `suffix_same_layout` | `size, state, entries, entry_count` | complete `state`; size through `entry_count`, with `entries + entry_count` fully covered | `state == NULL && entries == NULL && entry_count == 0` | `dcc_command_registry_remote_snapshot_deinit` |
| `dcc_command_registry_plan_t` | `size_extensible` | `inout` | `owning` | `covered_atomic` | `suffix_same_layout` | `size, state, diffs, diff_count, create_count, update_count, delete_stale_count, noop_count, guild_id, delete_stale, dry_run` | complete `state`; size through `dry_run`, with `diffs`, every count, ID, and flag fully covered | `state == NULL && diffs == NULL`, every count/ID/flag zero | `dcc_command_registry_plan_deinit` |
| `dcc_replay_record_t` | `size_extensible` | `inout` | `owning` | `covered_atomic` | `suffix_same_layout` | `size, state, kind, ts_ms, event, payload, payload_len` | complete `state`; size through `payload_len`, with kind/time/event and `payload + payload_len` fully covered | `state == NULL && kind == DCC_REPLAY_GATEWAY && ts_ms == 0 && event == NULL && payload == NULL && payload_len == 0` | `dcc_replay_record_deinit` |

All three use `array_stride=not_arrayed`. Their `lifetime=owning` does not make
them fixed; same-major compatibility follows `layout=size_extensible` and the
prefix/append policy. The replay validation result is
`layout=size_extensible, direction=output, lifetime=trivial,
writer_policy=covered_atomic, append_policy=suffix_same_layout`; it has no
cleanup or owning-empty predicate.

Replay uses `state` as its exact ownership discriminator. Recorder input may be
a borrowed literal/stack-backed record only when `state == NULL`; recorder
write borrows it for the call and never takes ownership. Player-next succeeds
only into the initialized-empty form and returns `event`/`payload` backed by one
private owned allocation named by non-null `state`. Deinit frees only a
player-produced non-null state, never borrowed bytes, and resets covered fields
to the initialized-empty form; a second deinit is harmless. An owning replay
record is move-only: raw struct copies of non-null `state` are a documented
contract violation, while copying borrowed `state == NULL` input views is safe.
Tests cover literal and stack borrowed inputs, owned player output, populated
output rejection without stream consumption, double deinit, and the documented
non-copyable owning case.

Convert the remaining allocation-free external initializers in these families
to same-name public static-inline functions: cluster health-wait, remediation-
loop, supervisor-action, hot-reload canary options, voice connect-wait options,
App wait-result zero initialization where exposed, and store managed-message
binding. Each stateful/owning exception uses the opaque create/destroy contract
above. The inline API and macro profile matrices track every converted name.

Tests compile genuinely smaller synthetic DCC 2 prefix-header fixtures into
separate objects and link them against the new library. Canary bytes must prove bounded
initialization/read/write; nested-parent fixtures prove following offsets are
unchanged. Current-layout objects with only a reduced `.size` are insufficient,
because their allocation can hide an overflow. Run the probes as C11/C++17 on
ILP32, LP64, and Windows ABIs and include the record manifest in source and
binary packages.

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

The only automatic mapping is this exact ordinary-code token:

| DCC 1 | DCC 2 |
| --- | --- |
| `DCC_RESPOND_TEXT` | `DCC_CTX_REPLY_TEXT` |

Rewrite an identifier only as a complete token in ordinary code. Do not rewrite
its appearance in comments, strings, a larger identifier, `#define`, `#undef`,
or conditional-macro expressions such as `defined(...)`; report user-owned
macro definitions and conditional use for review. Includes must match the
exact header spelling rather than a substring in a path.

These three superficially direct mappings are explicitly manual:

| DCC 1 construct | Candidate DCC 2 construct | Why automatic rewrite is unsafe |
| --- | --- | --- |
| `<dcc/sugar.h>` | `<dcc/bot.h>` plus focused headers | Sugar transitively exposed Core, REST, hot-reload, intents, replay, and firewall declarations that the exact five-leaf Bot aggregate intentionally omits. |
| `DCC_SIMPLE_BOT_MAIN` | `DCC_BOT_MAIN` or `DCC_DEV_BOT_MAIN` | The old production preset still loaded dotenv; the new production main never does, while the development main also changes command-sync behavior. |
| `DCC_V2_TEXT` | `DCC_UI_TEXT` | The old compound literal is valid in a file-scope static initializer; the new static-inline constructor is not a constant expression. |

Emit `path:line:column` plus the specific reason above. Do not choose extra
includes, development versus production mode, token-loading policy, command-
sync policy, or storage duration on the user's behalf.

All other DCC 1 constructs are manual conversions. Emit deterministic
`path:line:column` diagnostics, with the old token and a short reason, for at
least:

- `DCC_ON_*`, `DCC_ROUTE_*`, and `DCC_DECORATE_*`, including combinatorial
  route suffixes and typed-route forms;
- response families other than exact `DCC_RESPOND_TEXT`, including
  `DCC_PUBLIC*`, `DCC_PRIVATE*`, `DCC_DONE*`, and status-discarding response
  statements; `PRIVATE` requires an explicit ephemeral-policy decision;
- every `DCC_V2_*` spelling, including exact `DCC_V2_TEXT`, and all legacy
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
- create `docs/migration-1-to-2.md` with the one safe mapping table plus
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
2. Produce the reviewed draft entries for Task 15's
   `tools/api_v2_major_reset_1_to_2.json`: every removed or same-name-changed
   external/inline/type/macro key has its reconstructed v1.5.0 owner/
   fingerprint, owning task, and reason, with no wildcard. The strict diff stays
   RED until the atomic cut and rejects any unlisted loss.
3. Add migration-tool fixtures for the one safe mapping, all three explicitly
   unsafe direct-looking mappings, every lexical preservation rule, manual
   diagnostics, newline/mode preservation, dry-run, write, exit states, and
   idempotence. Capture RED before the tool exists.
4. Move the 83 context declarations, wait-result cleanup, intent groups, env
   functions, and private route/listener types to their final owners. Add
   standalone C11/C++17 and focused runtime tests before removing an adapter.
5. Migrate examples, generators, generated-project fixtures, package consumers,
   and behavioral tests. Build every generated preset from a clean output tree
   with warnings as errors. Remove only tests whose sole contract was an alias.
6. Prove the repository and private App implementation build without including
   `sugar.h` or `legacy.h`, then delete the 204-header Sugar tree, both umbrella
   headers, legacy sources/wrappers, and stale CMake registrations by patch.
7. In strict mode require zero removed declarations/definitions/exports, exactly
   eight App aggregate headers, the three canonical env names, 224 canonical
   REST endpoints, no transition manifest allowance, and no forbidden package
   member. Inventory every installed-header FunctionDecl and reject plain
   external linkage, ambiguous storage, or duplicate ownership before the
   shared cut.

Do not satisfy a negative audit by excluding a directory, skipping Python,
allowing a missing symbol tool, disabling a target, or treating a missing
previous-Stable compat-base/reset-ledger comparison as success. Every configured
test build requires Python and all manifest inputs.

## Atomic remaining-compatibility cutover and commit boundary

Preparation may be split into these reviewable, buildable commits:

1. `chore: add DCC 2 migration tooling`
2. `refactor: migrate repository consumers to DCC 2`
3. `refactor: isolate DCC 1 app adapters`

Those commits retain the installed legacy headers and exported compatibility
wrappers until the coordinated cutover. The next commit is shared with Task 15:

4. `feat!: remove DCC 1 public API`

That one commit must contain every held Task 14 public cut listed at the top of
this brief, including the physical Sugar/`legacy.h` and component-builder
deletions, the 361 App/context removals, generic REST 61-to-35 reduction,
OAuth2/firewall/response/builder ABI replacements, final eight-header App
aggregate and env renames, project version 2.0.0, SOVERSION 2, all final DCC 2
baselines, the exact pinned 1→2 reset ledger, strict same-major audit, and
package/archive negative assertions.
There must not be a commit where any removal advertises 1.5.x, nor a commit
where a checked baseline describes a different header tree. If Task 15's half
is not ready, stop before the cut and hand off the prepared branch rather than
creating a knowingly invalid ABI state.

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
