# App Runtime

`dcc_app_t` owns the Gateway client, command registry, routing, interaction
contexts, and scheduled tasks. Normal bots use `<dcc/bot.h>`; advanced
integrations use focused `<dcc/app/*.h>` headers.

## Start with the Bot API

```c
#include <dcc/bot.h>

DCC_SLASH_FN(ping) {
    (void)user_data;
    return DCC_CTX_REPLY_TEXT(ctx, "pong");
}

DCC_DEV_BOT_MAIN(
    DCC_LISTEN_SLASH("ping", "Reply with pong", ping)
)
```

Handlers return `dcc_status_t`: propagate reply or validation failures.
`DCC_DEV_BOT_MAIN` opts into local dotenv loading and READY command
synchronization. Production `DCC_BOT_MAIN` does neither; apply command changes
explicitly with the command-sync tool. Use the matching `_WITH` runner with a
`dcc_bot_config_t` for configuration.

The [CLI reference](../reference/cli.md) describes project scaffolding.
The [Bot reference](../reference/api/bot/main.md) defines runner signatures.

## Explicit lifecycle

Advanced applications initialize `dcc_app_options_t`, call `dcc_app_create`,
register listeners, run with `dcc_app_run`, and release the App with
`dcc_app_destroy`. Read the [lifecycle declarations](../reference/api/app/lifecycle.md)
and [listener declarations](../reference/api/app/listeners.md).
Registration copies descriptor metadata synchronously; callback user data
remains borrowed for the registered lifetime.

## Interaction replies and completion

Context helpers share the ordered interaction queue. Reply selects the
state-appropriate response; update targets a component source, followup forces
a followup, and send targets an unrelated channel. Successful return means
queue admission, not successful delivery.

General Context callbacks use `dcc_rest_result_fn`:

```c
static void on_complete(dcc_client_t *client,
                        const dcc_rest_result_t *result, void *user_data) {
    (void)client;
    (void)user_data;
    /* result is borrowed; inspect transport_status and http_status here. */
    (void)result;
}
```

DCC 2.1 adds `dcc_ctx_reply_ex`, `dcc_ctx_defer_ex`,
`dcc_ctx_edit_original_ex`, and `dcc_ctx_followup_ex`. They take
`dcc_rest_call_options_t` and an optional owned request-handle output for
cancellation, waiting, and completion. They preserve queue ordering. A null
output auto-releases the handle; an owned handle must be destroyed by its caller.

See [Context declarations](../reference/api/app/context.md),
[choose an API layer](../concepts/api-layers.md), and
[ownership and async](../concepts/ownership-and-async.md) before retaining data.

For live bots, fetch the remote snapshot through `dcc_command_registry_fetch_remote()`
and pass the returned body to `dcc_app_sync_commands_from_json()`.
