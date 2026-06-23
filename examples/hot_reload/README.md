# DCC Hot Reload Slash Command Example

This example runs a Discord bot through the DCC hot reload host. The module
registers `/테스트` and replies with an embed whose description is configured in
`bot.c`. The host uses DCC isolated worker mode: the Gateway connection stays in
the parent process, and the module `.so` is loaded only by
`build/dcc_hot_reload_worker`.

## Run

Put `DCC_TOKEN=...`, `BOT_TOKEN=...`, or `DISCORD_TOKEN=...` in this
directory's `.env`, the repository root `.env`, or export it in your shell.

```sh
make
make run
```

The host prints plain logs to the terminal. Stop it with Ctrl-C when you are
done testing.

`make run` also starts a loopback health sidecar at
`http://127.0.0.1:18080/hot-reloadz`. It returns fixed-buffer hot-reload JSON
with the active generation, backend, active worker generation, and last-good
worker generation:

```sh
curl http://127.0.0.1:18080/hot-reloadz
```

Override the bind address or port when needed:

```sh
DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1 DCC_HOT_RELOAD_HEALTH_PORT=18081 make run
```

The same Makefile also works from an installed package under
`share/dcc/examples/hot_reload`. In that mode it builds the module through the
installed `dcc` CMake package and copies the installed
`dcc_hot_reload_host`/`dcc_hot_reload_worker` into the local `build/` directory.
If LLAM is not installed as a CMake package, pass the same paths used for DCC:

```sh
DCC_LLAM_ROOT=/path/to/LLAM \
DCC_LLAM_LIBRARY=/path/to/LLAM/libllam_runtime.a \
make
```

While `make run` is still running, edit `bot.c` in another terminal and run:

```sh
make
```

The Makefile writes a fresh local build stamp header, rebuilds the module, and
replaces `build/dcc_hot_reload_test_bot.so`. The host watches that exact path,
starts a candidate worker for the new module, and routes new events to it only
after worker healthcheck succeeds.
The sidecar updates with the promoted generation, so local scripts can confirm
that a rebuild is live before testing the command again.

If the rebuild fails, the Makefile leaves the previous module in place. If a new
module is built but fails to load or initialize, DCC keeps the previous
generation running. In isolated worker mode, the previous worker remains the
last-good route while the candidate is rejected. If a worker dies while handling
an interaction and no last-good retry can answer it, the parent sends an
ephemeral temporary error response.

After a reload, run `/테스트` again in Discord. Already-sent embed messages are
not edited retroactively.

By default the slash command is global, so Discord can take a while to show it.
For faster guild-only testing:

```sh
DCC_HOT_RELOAD_GUILD_ID=123456789012345678 make run
```

## Live Soak

To validate repeated live reloads without manually rebuilding in another
terminal, run:

```sh
DCC_TOKEN=... make live-soak
```

`make live-soak` starts the isolated host, waits for Gateway READY, rebuilds the
module several times, and waits for each active generation to appear in
`build/live-soak.log`. Tune it with:

```sh
DCC_HOT_RELOAD_SOAK_RELOADS=5
DCC_HOT_RELOAD_SOAK_INTERVAL_SECONDS=2
DCC_HOT_RELOAD_SOAK_READY_TIMEOUT_SECONDS=90
DCC_HOT_RELOAD_SOAK_RELOAD_TIMEOUT_SECONDS=30
```
