# Hot Reload

DCC supports two hot reload backends:

- in-process `dlopen`
- isolated worker process

Use isolated worker mode for production. The parent process keeps Discord
Gateway, cache, REST, and Voice ownership. The worker loads the bot module and
executes handlers.

## Worker Model

```text
dcc_hot_reload_host parent
  - owns Discord client
  - watches module file
  - supervises worker process
  - routes interaction dispatch

dcc_hot_reload_worker
  - dlopen module
  - run module init and healthcheck
  - execute handler
  - return response payload
```

## Last-Good Rollback

The parent keeps a last-good worker alive. If a candidate crashes, times out, or
fails healthcheck, the parent keeps routing to last-good and reports rollback
state in health JSON.

Worker IPC is bounded by the configured worker health timeout. Parent-to-worker
event and health writes, worker-to-parent READY/result writes, and response body
reads all return timeout instead of blocking the Gateway owner forever. A worker
that stalls mid-frame is treated as unhealthy and can be retried or rolled back
through last-good. Event JSON forwarded to workers is capped, and child workers
also time out partial event bodies after accepting an EVENT frame.

Worker shutdown is bounded as well. The parent sends STOP, waits for the drain
window, then escalates termination instead of waiting forever for a stuck module.

POSIX workers are started with `posix_spawn()` and an explicit file-action
mapping for the IPC sockets. The worker receives a sanitized environment: basic
runtime/locale variables and the documented `DCC_DISCORD_API_BASE` and
`DCC_ISOLATED_TEST_URL` overrides are retained, while token variables and other
unlisted secrets are not inherited. Treat this as process/fault isolation, not
as a substitute for OS sandboxing.

## Canary Promotion

Candidate workers can receive a small percentage of requests before promotion.
The canary policy tracks failures and promotes only after the configured window
stays healthy.

Use the installed initializer and option structs. This configuration function
is compiled from `examples/docs/guide_configuration.c`; it does not start a
worker. Pass the returned options to the
[hot-reload lifecycle API](../reference/api/hot_reload/lifecycle.md).

<!-- DCC_DOC_SNIPPET_BEGIN(hot-reload-canary) -->
```c
#include <dcc/hot_reload.h>

/* worker_path must remain valid while the caller uses these options. */
dcc_hot_reload_options_t dcc_example_canary_options(const char *worker_path) {
    dcc_hot_reload_canary_options_t canary;
    dcc_hot_reload_canary_options_init(&canary);
    canary.canary_percent = 5U;
    canary.promote_after_ms = 30000U;
    dcc_hot_reload_options_t options = {
        .size = sizeof(options),
        .backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER,
        .worker_path = worker_path,
        .worker_health_timeout_ms = 5000U,
        .worker_drain_timeout_ms = 1000U,
        .worker_canary_enabled = 1U,
        .worker_canary_options = canary,
    };
    return options;
}
```
<!-- DCC_DOC_SNIPPET_END(hot-reload-canary) -->

## Host CLI

```sh
DCC_TOKEN=... \
DCC_BOT_MODULE=./bot.so \
build/dcc_hot_reload_host --global
```

Guild-scoped development:

```sh
DCC_BOT_MODULE=./bot.so \
DCC_TOKEN=... \
build/dcc_hot_reload_host --guild "$GUILD_ID"
```

Preflight without network:

```sh
build/dcc_hot_reload_host --check
```

## Health

Enable the unsigned local health sidecar:

```sh
build/dcc_hot_reload_host \
  --health-address 127.0.0.1 \
  --health-port 18080
```

The response includes backend, active/last-good/candidate generations, canary
counters, promotion/rollback totals, and the latest rollback reason.
