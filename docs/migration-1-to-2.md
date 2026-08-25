# Migrating from DCC 1 to DCC 2

DCC 2 removes the compatibility Sugar layer and makes status propagation,
request ownership, listener configuration, and component placement explicit.
Run `python3 tools/dcc_migrate_v2.py path/to/source` for a dry run. Add
`--write` to apply only the one mechanically safe edit.

## Automatic edit

| DCC 1 | DCC 2 |
| --- | --- |
| `DCC_RESPOND_TEXT(ctx, text)` | `DCC_CTX_REPLY_TEXT(ctx, text)` |

The tool changes only complete ordinary-code identifiers. It does not touch
comments, strings, larger names, macro definitions, or conditional macro use.

## Headers and bot entrypoints

Normal bots now include `<dcc/bot.h>`. Low-level users include focused App,
REST, Gateway, voice, cluster, or hot-reload headers directly. Replacing
`<dcc/sugar.h>` cannot be automated because Sugar previously exposed many
unrelated subsystems transitively.

Choose `DCC_DEV_BOT_MAIN` when local `.env` loading and development command
synchronization are wanted. Choose `DCC_BOT_MAIN` for production; it never
loads `.env` and disables command mutation on READY. Advanced programs use the
corresponding `_WITH` form and a `dcc_bot_config_t`.

## Handlers and listeners

Handlers return `dcc_status_t`; propagate each context/action failure rather
than discarding it. Replace route/decorator suffix matrices with one of the 19
flat `DCC_LISTEN_*` constructor pairs. Put middleware, ownership checks,
cooldowns, bindings, validators, user data, cleanup, and once behavior in one
`dcc_listener_config_t`, then pass it to the `_WITH` constructor. Registration
deep-copies descriptor metadata synchronously; callback user data remains
borrowed until unlisten or App destruction.

## Interactions and components

Use `DCC_CTX_REPLY*` for state-aware initial/edit/followup behavior,
`DCC_CTX_UPDATE*` only for component-source updates, `DCC_CTX_FOLLOWUP*` to
force a followup, and `DCC_CTX_SEND*` for unrelated channel messages. Each
function returns queue-admission status and serializes borrowed builder input
before returning.

Components are tagged `dcc_component_v2_builder_t` values made with
`DCC_UI_*`. A former `DCC_V2_*` compound literal may have been a file-scope
constant; a `DCC_UI_*` function call is not, so move such construction to block
scope. Message and modal placement is validated by the canonical tagged
component grammar.

## REST ownership

Endpoint calls return an owned `dcc_rest_request_t *`. Completion is delivered
as `dcc_rest_result_t`; callers cancel, wait, and destroy individual requests.
Use `dcc_rest_submit()` for arbitrary routes. App REST mirrors, raw JSON
overloads, callback/future suffix matrices, global bulk-cancel/wait controls,
and `dcc_rest_response_t` are removed.

## Builders and records

Initialize public records with their same-name header inline initializer or
constant initializer, set presence through typed setters, and keep borrowed
input alive only through the documented synchronous admission call. Opaque
runtime state uses create/open plus destroy/close. Owning output records must
be initialized empty, are move-only while populated, and must be deinitialized
before reuse.
