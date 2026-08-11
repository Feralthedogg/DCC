# Tasks 7–15 design-lock report

Date: 2026-08-11

Base commit: `22a8999c8bbb690ccdd0e791216376d69de47213`

This report freezes the remaining DCC 2.0 implementation sequence before
production work resumes. The central design, implementation plan, and the nine
task briefs listed below are one contract. A later task may extend a private
implementation detail, but it may not silently change a count, ownership rule,
ABI prefix, endpoint capability, or atomic release boundary recorded here.

## Authoritative inputs

- `docs/superpowers/specs/2026-08-11-dcc-2.0-api-redesign-design.md`
- `docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`
- `task-7-brief.md` through `task-15-brief.md`
- `tools/rest_v2_endpoints.json`, whose candidate inventory is corrected by
  Tasks 7–10 and becomes strict in Task 10
- the completed Task 4 request lifecycle and Task 6 exact-preflight contracts

The Discord endpoint correction was rechecked against the current official
[Guild](https://docs.discord.com/developers/resources/guild),
[Guild Template](https://docs.discord.com/developers/resources/guild-template),
[User](https://docs.discord.com/developers/resources/user),
[Invite](https://docs.discord.com/developers/resources/invite), and
[Application Command](https://docs.discord.com/developers/interactions/application-commands)
documentation.
The disabled batch command-permission endpoint stays in the removal ledger;
the current guild resource exposes list/delete integration but not modify/sync;
Create Guild From Template and Get Current User DMs are absent; and Modify
Guild Channel Positions does not itself advertise an audit-reason header.

## Frozen surface arithmetic

| Task | Domain slice | Active endpoints | Handoff total |
| --- | --- | ---: | ---: |
| 6 | messages, interactions, webhooks | 41 | 41 |
| 7 | channels, threads, invites | 35 | 76 |
| 8 | guilds, members, roles | 47 | 123 |
| 9 | commands, moderation, guild resources | 57 | 180 |
| 10 | remaining Discord resources | 44 | 224 |

The final REST surface is exactly 224 canonical endpoints plus 60 generic
request/result/configuration operations, or 284 public `dcc_rest_*`
operations. The seven unavailable candidates are never counted as active:
Create Guild, Delete Guild, Modify Guild Integration, Sync Guild Integration,
Bulk Edit Guild Command Permissions, Create Guild From Template, and Get
Current User DMs.

The final App aggregate contains exactly eight focused headers: base, options,
lifecycle, listeners, context, modules, store, and env. The final Bot aggregate
contains exactly five leaves and is projected at 291 preprocessed `DCC_`
macros against a hard limit of 300.

## Cross-task contracts

### Request transport

Task 7 appends `audit_log_reason`, `auth_mode`, and `auth_token` after the
historical `dcc_rest_call_options_t::user_data` prefix without changing
version 1 or adding a presence mask. All later endpoint tasks use that one
normalizer and private endpoint capability policy.

Audit reasons are unencoded valid UTF-8 containing 1–512 Unicode scalar values.
The validated UTF-8 bytes are RFC 3986 percent-encoded exactly once; encoded
length is not capped at 512. Bearer tokens are copied before return, never
logged or surfaced, and wiped before free. Absolute URLs reject every Discord
auth override and audit reason and never receive a client credential.

All endpoint input is validated and completely copied or serialized before
return. Local rejection allocates/admit nothing and delivers no callback or
observer. `DCC_OK` guarantees one terminal result. Multipart validation and
exact aggregate measurement finish before the first payload allocation.

### ABI evolution

Every new caller-owned record starts with `size`, `version`, and, where fields
are optional, presence/null state. Reads are bounded by caller size. Unknown
bits, partial fields, present-but-uncovered fields, invalid pointer/count pairs,
bad versions, and checked-arithmetic overflow fail locally. Arrays of
evolvable records use the validated historical element stride rather than the
library's current `sizeof`.

Components v2 makes a deliberate DCC 2 hard cut to
`size/version/present/type/id` followed by a named tagged union. It does not
auto-detect the DCC 1 flat layout. Validation, JSON building, and allocation-
free measurement share one traversal and one structured field-path error.

### Ordered interactions

Task 11 adds one refcounted App registry keyed by interaction ID and token, a
per-interaction FIFO, committed and projected state, and head-only REST
admission. Queue nodes hold fully owned prepared requests created by the same
Task 6 serializers used by public endpoints. Terminal order is callback,
observer, private post-terminal hook, then next-head pump. Auto-defer uses the
same queue lock and is cancellable; App close drains active and pending work in
the documented lock order. `dcc_flow_deinit()` releases the flow owner without
suppressing an accepted terminal callback.

### Bot entrypoints and signals

`dcc_bot_config_t` is exactly `size`, `version`, `name`, `app_options`,
`token_env`, `install_default_error_handler`, and `handle_signals`.
The public static-inline `DCC_BOT_CONFIG(name)` constructor initializes the
full current layout, the default token name `DISCORD_TOKEN`, and both toggles
to one.

The four main macros call one documented public `static inline dcc_bot_run`
with a flat listener pointer/count and a zero-or-one development flag. It
normalizes covered options, gives a covered nonempty supplied client token
precedence, loads dotenv only for development, forces development command sync
on or production READY mutation off, registers listeners in order, selects
`dcc_app_run_with_signals` or `dcc_app_run`, and always attempts owner-thread
destruction. Macro arguments are evaluated once; only `DCC_OK` maps to exit 0.

`dcc_app_run_with_signals` is retained with its existing symbol and
cross-platform implementation and moves to `app/lifecycle.h`. Only the three
definition-oriented `*_defined_with_signals` wrappers are removed.

### Final removal and release boundary

Task 14 deletes 204 Sugar headers and `app/legacy.h`, relocates 83 context
symbols and `dcc_app_run_with_signals`, and removes 361 functions: 359 legacy
`dcc_app_*` symbols plus `dcc_ctx_state` and `dcc_ctx_show_modal_flow`.
`app_signal.c` is not a whole-file deletion.

The migration tool performs only four proven token-aware rewrites. Everything
semantic reports `path:line:column` for manual conversion. Repository examples,
generators, fixtures, installed consumers, CMake targets, package manifests,
and audits migrate before physical compatibility deletion.

The public deletion, project 2.0.0, SOVERSION 2, final symbol baseline, strict
same-major gate, and clean package negative assertions land in one Task 14/15
atomic commit. LLAM remains exactly 2.2.0. The final release is `v2.0.0`, Stable,
and non-prerelease; pushing, tagging, or publishing is outside an implementation
task until independent final review approves the exact tree.

## Task briefs and execution order

1. Task 7: 35 channel/thread/invite endpoints and the shared auth/audit layer.
2. Task 8: 47 guild/member/role endpoints and four stale removals.
3. Task 9: 57 command/resource endpoints and sequential registry aggregate.
4. Task 10: 44 remaining endpoints, 196 App mirror removals, strict manifest.
5. Task 11: FIFO interaction actions, auto-defer, flow lifecycle, App drain.
6. Task 12: tagged/versioned Components v2 with shared traversal.
7. Task 13: canonical five-leaf Bot API and eight-leaf App integration.
8. Task 14: repository migration and physical DCC 1 removal preparation.
9. Task 15: atomic ABI-2 cutover, generated reference, packages, release gates.

Each task follows RED contract commit, production implementation commit,
normal/sanitizer/platform/package verification, implementer report commit, and
independent review. A task does not begin its successor's domain, and no
progress allowance survives Task 10 strict mode.

## Design-lock verification

The lock is accepted only after:

- all nine briefs exist with no placeholder or unresolved decision;
- endpoint counts, auth/audit/multipart counts, removal counts, and handoff
  arithmetic agree across the design, plan, and briefs;
- historical 231/51/59/45 figures appear only as explicitly superseded
  discovery history;
- the 361-removal signal correction appears in Tasks 13–15;
- `git diff --check`, strict MkDocs, the Task 6 endpoint progress audit, and the
  existing complete CTest suite pass from this base;
- an independent reviewer reports no Critical or Important finding.

No production source or public header is changed by the design-lock commit.
