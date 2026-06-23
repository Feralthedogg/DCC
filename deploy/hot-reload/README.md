# DCC Hot Reload Deployment

These templates run `dcc_hot_reload_host` with isolated worker mode. The parent
process owns the Discord Gateway session and cache. Bot logic lives in a watched
module file, and the module is loaded only by `dcc_hot_reload_worker`.

## Files

- `dcc-hot-reload.env`: environment file consumed by systemd.
- `dcc-hot-reload.service`: hardened systemd unit for the parent host.
- `dcc-hot-reload.tmpfiles.conf`: directory creation rules for module and log
  storage.
- `Containerfile`: multi-stage build for the installed isolated host/worker.
- `docker-compose.yaml`: container deployment with the module directory mounted.
- `Caddyfile`: loopback health sidecar reverse-proxy route template.

## Install

```sh
sudo install -d -m 0750 -o dcc -g dcc /etc/dcc
sudo install -m 0640 -o root -g dcc dcc-hot-reload.env /etc/dcc/
sudo install -m 0644 dcc-hot-reload.service /etc/systemd/system/
sudo install -m 0644 dcc-hot-reload.tmpfiles.conf /etc/tmpfiles.d/
sudo systemd-tmpfiles --create /etc/tmpfiles.d/dcc-hot-reload.tmpfiles.conf
sudoedit /etc/dcc/dcc-hot-reload.env
sudo install -m 0640 -o dcc -g dcc my_bot_module.so /var/lib/dcc/hot_reload/bot.so
sudo systemctl daemon-reload
sudo systemctl enable --now dcc-hot-reload
```

Build your module as a shared object and install it atomically:

```sh
install -m 0640 -o dcc -g dcc my_bot_module.so /var/lib/dcc/hot_reload/bot.so.tmp
mv /var/lib/dcc/hot_reload/bot.so.tmp /var/lib/dcc/hot_reload/bot.so
```

The host watches `DCC_BOT_MODULE`. When the file changes, it starts a candidate
worker, loads the module in that worker, runs module init, and promotes the
worker only after healthcheck succeeds. If the candidate fails, the previous
worker stays active as the last-good route.

## Checks

Validate configuration without opening a Gateway session:

```sh
sudo -u dcc sh -c '
set -a
. /etc/dcc/dcc-hot-reload.env
set +a
/usr/local/bin/dcc_hot_reload_host --check
'
```

Inspect service state:

```sh
systemctl status dcc-hot-reload
journalctl -u dcc-hot-reload -n 100 --no-pager
curl http://127.0.0.1:18080/hot-reloadz
```

`DCC_HOT_RELOAD_HEALTH_PORT` enables the unsigned loopback health sidecar. The
JSON response includes the active worker generation, last-good worker
generation, backend, supervisor state, timeout settings, and `worker_events`
counters for active worker failures, last-good promotions, dispatch failures,
and temporary interaction error responses. Keep
`DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1` unless a local reverse proxy or
monitoring agent is explicitly meant to reach it.
`DCC_HOT_RELOAD_POLL_MS`, `DCC_HOT_RELOAD_SETTLE_MS`,
`DCC_HOT_RELOAD_WORKER_HEALTH_MS`, and `DCC_HOT_RELOAD_WORKER_DRAIN_MS` tune the
watcher and isolated-worker lifecycle. The worker health timeout covers
candidate readiness, idle supervisor IPC health pings, and event dispatch
replies. Use `DCC_HOT_RELOAD_SETTLE_MS=none` only when deploying modules
through atomic replacement.

For development guild-only command registration, set
`DCC_HOT_RELOAD_GUILD_ID` or pass `--guild <id>` when running manually. Leave it
unset for default global slash-command registration, or pass `--global` to
clear an inherited guild id for one run.

## Compose

The default Compose build expects `DCC/` and `LLAM/` to be sibling directories,
then uses this template from `DCC/deploy/hot-reload/`:

```sh
mkdir -p modules
install -m 0644 my_bot_module.so modules/bot.so
docker compose up --build
```

`docker-compose.yaml` overrides the container health sidecar address to
`0.0.0.0` inside the Compose network and publishes Caddy only on host loopback
as `127.0.0.1:18080`. The module remains a mounted file at
`/var/lib/dcc/hot_reload/bot.so`, so rollouts are still atomic replacements:

```sh
install -m 0644 my_bot_module.so modules/bot.so.tmp
mv modules/bot.so.tmp modules/bot.so
```

Keep `DCC_TOKEN` in `dcc-hot-reload.env` or an external secret store, and keep
`DCC_HOT_RELOAD_GUILD_ID` empty unless you intentionally want guild-scoped
development command registration. Manual runs can pass `--global` to force
global command registration even when the environment file contains a guild id.
