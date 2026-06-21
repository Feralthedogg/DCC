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

## Canary Promotion

Candidate workers can receive a small percentage of requests before promotion.
The canary policy tracks failures and promotes only after the configured window
stays healthy.

```c
dcc_hot_reload_canary_options_t canary =
    DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT();

dcc_hot_reload_options_t options =
    DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS("dcc_hot_reload_worker", canary);
```

## Host CLI

```sh
DISCORD_TOKEN=... \
DCC_BOT_MODULE=./bot.so \
build/dcc_hot_reload_host --global
```

Guild-scoped development:

```sh
DCC_BOT_MODULE=./bot.so \
DISCORD_TOKEN=... \
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
