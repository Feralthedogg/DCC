# Live Validation

DCC's default tests use local mock gateways and HTTP servers. Before a release,
run an opt-in live soak against Discord with a real bot token:

```sh
DCC_TOKEN=... \
DCC_SOAK_SECONDS=900 \
DCC_SOAK_INTERVAL_SECONDS=30 \
./build/dcc_live_gateway_soak
```

Useful shard-window controls:

```sh
DCC_SOAK_SHARDS=4
DCC_SOAK_TOTAL_SHARDS=16
DCC_SOAK_FIRST_SHARD=0
DCC_SOAK_MAX_CONCURRENCY=1
```

Set `DCC_SOAK_INTENTS` to a numeric mask, `0x...`, or the same presets accepted
by `DCC_ENV_INTENTS_OR`, for example `all` or `guilds,message_content`.

For a live planned rollout example, run `dcc_cluster_rollout` with the same
token and shard-window environment. Set `DCC_CLUSTER_PRINT_JSON=1` to print
fixed-buffer cluster-status JSON, including recovery-plan, rolling-operation,
and supervisor decision callbacks/snapshots, alongside the compact text logs.
When a production wrapper runs `dcc_cluster_wait()` on a runtime thread, use
`dcc_cluster_wait_until_ready()` from its control thread to block deploy
admission until all shards are READY or to fail early on close/error state.
Use `dcc_cluster_wait_until_health()` after the READY gate when the validation
must prove the recovery-plan classifier reached `healthy`; keep the returned
plan in the deployment log so any timeout or critical failure includes the
selected shard and reason.

For a live voice join probe, use a development guild and an empty voice channel:

```sh
DCC_TOKEN=... \
DCC_VOICE_GUILD_ID=... \
DCC_VOICE_CHANNEL_ID=... \
DCC_VOICE_USER_ID=... \
./build/dcc_live_voice_probe
```

`DCC_VOICE_GUILD_ID` is a raw guild ID. `DCC_VOICE_CHANNEL_ID` and
`DCC_VOICE_USER_ID` also accept copied Discord mentions such as `<#channel>`
and `<@user>`, matching the public `DCC_ENV_CHANNEL_OR` and `DCC_ENV_USER_OR`
sugar helpers.

The probe starts one Gateway session with `GUILDS` and `GUILD_VOICE_STATES`,
uses `dcc_voice_client_connect_and_wait_until_ready()` to arm the descriptor
waiter before sending Gateway Opcode 4, waits for the `VOICE_STATE_UPDATE` plus
`VOICE_SERVER_UPDATE` pair, waits for voice READY, then starts the managed voice
websocket loop and waits for ACTIVE by default. Set `DCC_VOICE_RUN_WEBSOCKET=0`
to stop after descriptor/READY validation, or `DCC_VOICE_WAIT_ACTIVE=0` to
start the loop without blocking on ACTIVE. `DCC_VOICE_PRINT_JSON=1` prints
fixed-buffer health JSON snapshots.

The soak tool fetches `/gateway/bot`, prints the session-start-limit state,
starts the configured cluster, waits until every shard reaches READY, and then
uses a DCC task group running on LLAM to print periodic recovery-plan snapshots.
Each snapshot includes `health`, top-level `action`, `action_shard`, aggregate
counters, and a short `reason`. At the end of `DCC_SOAK_SECONDS`, it calls
`dcc_cluster_stop()` and waits for a clean shutdown.

Failure signals to investigate before tagging a release:

- `gateway discovery failed` or `DCC_ERR_RATE_LIMITED`: session-start-limit or
  token problem before IDENTIFY.
- `ready` count below `total`: shard admission, identify concurrency, or gateway
  handshake issue.
- `health=critical`, `action=rearm_restart`, or non-zero `error`,
  `socket_closes`, `log_errors`, or repeated `restarts`: inspect the
  `action_shard`, `reason`, and shard lines with `DCC_SOAK_PRINT_SHARDS=1`.
- `RATE_LIMIT`: REST or gateway-side retry behavior should match the route and
  retry timing being exercised.

Use `dcc_cluster_recovery_plan()` for sidecar health endpoints or supervisors
that need one consistent cluster snapshot. It returns the aggregate summary,
health classifier, highest-priority recovery action, selected shard snapshot,
and reason string without requiring separate summary and shard-info reads. Use
`dcc_cluster_status_json()` when the sidecar should expose recovery-plan,
rolling-operation, and supervisor decision snapshots in one fixed-buffer response, or
`dcc_cluster_add_status_route()` to register that response as an unsigned
`GET /clusterz` interaction-server sidecar route. Use
`dcc_cluster_supervisor_decision_event_json()` from a supervisor decision
callback when the live run should emit one structured JSON line per
restart/suppress/default decision. Use
`dcc_cluster_recovery_plan_json()`, `dcc_cluster_remediation_result_json()`, or
`dcc_cluster_remediation_loop_result_json()` when the sidecar needs a
fixed-buffer JSON body.
Use `dcc_cluster_operation_status_json()` to include the current rolling
operation type, batch progress, completion counters, and last operation status
in the same fixed-buffer response.
Use `dcc_cluster_remediate_loop()` only with an explicit maximum attempt count
and interval when validating supervisor automation against a live bot.

For event-volume checks, set `DCC_SOAK_RAW_SAMPLE=1000` to print every thousandth
raw dispatch without flooding stdout.

Local pre-soak checks:

```sh
ctest --test-dir build -R '^dcc_(gateway|cluster)_chaos_smoke$|^dcc_cluster_supervisor_policy' --output-on-failure
```

`dcc_gateway_chaos_smoke` uses a mock gateway to force a resumable close,
Gateway Opcode 7 reconnect, stale-session close, fresh identify, and final
clean stop. `dcc_cluster_chaos_smoke` applies the same transition pattern across
two shards and verifies aggregate health counters. The supervisor policy smokes
cover explicit suppress and restart-over-budget decisions. Run them before the
live soak when touching gateway reconnect, heartbeat, websocket send, cluster
health, supervisor policy, or shutdown code.
For live runs, also snapshot `dcc_client_gateway_info()` when reconnects repeat;
it exposes the cached session ID, sequence, resume decision, pending reconnect
request, identify delay, and last Gateway task status.

Hot-reload validation notes:

- Build `dcc_hot_reload_host` and `dcc_hot_reload_bot`, run the host with
  `DCC_TOKEN=...`, then rebuild the module at the same path. The Gateway session
  should stay connected while the module generation changes.
- Use `examples/hot_reload/` for end-to-end slash-command hot reload testing:
  `make run` starts a host and watched module, registers `/테스트` globally by
  default, and accepts `DCC_HOT_RELOAD_GUILD_ID=...` for fast guild-only
  validation. Use `--global` with the installed host to override an inherited
  guild id for a run. The example uses isolated worker mode, so verify that the printed
  worker executable path exists and that Gateway stays connected across module
  rebuilds. The installed copy under `share/dcc/examples/hot_reload` supports
  the same `make` flow by using the installed `dcc` CMake package; pass
  `DCC_LLAM_ROOT` and `DCC_LLAM_LIBRARY` if LLAM is not installed through CMake.
- For repeatable live reload validation, run
  `DCC_TOKEN=... make -C examples/hot_reload live-soak`. The helper starts the
  isolated host, waits for Gateway READY, rebuilds the module repeatedly, and
  waits for every expected `active generation` line in `build/live-soak.log`.
  Use `DCC_HOT_RELOAD_SOAK_RELOADS`, `DCC_HOT_RELOAD_SOAK_INTERVAL_SECONDS`,
  `DCC_HOT_RELOAD_SOAK_READY_TIMEOUT_SECONDS`, and
  `DCC_HOT_RELOAD_SOAK_RELOAD_TIMEOUT_SECONDS` to tune the run.
- `dcc_hot_reload_isolated_last_good_smoke` covers the isolated worker failure
  paths: a bad candidate that fails module init does not replace the active
  worker and preserves the init error in parent health; a candidate that exits
  before READY is classified as runtime failure while a hung candidate that
  never sends READY times out without replacing active; an invalid worker READY
  frame is rejected without replacing active; a crash candidate becomes active,
  is killed while idle, and the LLAM supervisor promotes the last-good worker
  through IPC health checks; malformed or hung health replies are also promoted
  through the same supervisor path. Malformed event-result frames, hung
  event-result waits, and crash candidates during slash-command dispatch are
  retried through last-good, then the failed active worker is drained instead of
  being kept as last-good. It also covers the no-last-good interaction failure
  path where the parent sends an ephemeral temporary error response and retires
  malformed or hung active workers, plus the double-failure path where the
  promoted last-good worker also fails the retry and is retired.
- Use installed `dcc_hot_reload_host` for the minimal `DCC_TOKEN` +
  `DCC_BOT_MODULE` development host when DCC's installed `bin` directory is on
  `PATH`. Installed package builds provide that host/worker pair by default
  through `DCC_BUILD_TOOLS=ON`, even when examples/tests are disabled. Set
  `DCC_HOT_RELOAD_WORKER` only when overriding the default worker. The installed
  host defaults to `guilds`; use `--guild <id>` for guild-only slash-command
  development registration, `--global` to clear inherited guild scoping, and
  `DCC_HOT_RELOAD_INTENTS` or `--intents` for
  non-slash-command Gateway events. `DCC_HOT_RELOAD_INTENTS=all` enables every
  supported intent; use it only when privileged intents are allowed for the bot.
  It loads `.env`, accepts `BOT_TOKEN` and `DISCORD_TOKEN` as fallbacks, and
  supports `dcc_hot_reload_host --check` before
  opening a Gateway session. `--check` confirms token presence, module
  readability, worker executable/PATH resolution, and health sidecar settings.
  Set `DCC_HOT_RELOAD_HEALTH_PORT=18080` or pass `--health-port 18080` and
  inspect `curl http://127.0.0.1:18080/hot-reloadz` during a reload. Use
  `--poll-ms`, `--settle-ms`, `--worker-health-ms`, and `--worker-drain-ms` to
  validate production timing values before opening Gateway. Use
  `dcc_hot_reload_run()` for explicit client/watcher options and
  `dcc_hot_reload_attach()` after
  `dcc_client_start()` when the host owns more setup. `attach` fails fast if
  generation 1 cannot load, so a broken handler module does not enter the
  watcher loop.
- Use `dcc_hot_reload_wait_for_generation()` in a sidecar/control thread when a
  deployment script needs confirmation that a rebuilt `.so`/`.dll` is live.
- Use `dcc_hot_reload_wait_until_loaded()` when validation only needs to confirm
  that the current module or isolated worker is loaded before Gateway events are
  sent.
- Use `dcc_hot_reload_health_snapshot()` and
  `dcc_hot_reload_health_snapshot_json()` from an admin route or supervisor
  probe to expose `watching`, `stopped`, `reloading`, or `degraded` state,
  current generation, backend, isolated worker supervisor state,
  active/last-good worker generations and PIDs, pending-reload state, isolated
  worker recovery counters, and last reload error. Use
  `dcc_hot_reload_add_health_route()` for the standard unsigned
  `GET /hot-reloadz` sidecar route.

Voice validation notes:

- Use `dcc_voice_client_stats()` alongside `dcc_voice_client_session_info()` when
  exercising voice locally or in an opt-in bot. It snapshots queue depth, track
  markers, RTP loss/drop counters, reorder/jitter state, UDP/RTP state, websocket
  loop status, close-code reconnect state, and DAVE transition state.
- Use `dcc_voice_client_reconnect_action()` for the current effective decision
  after a voice close: `resume`, `full_reconnect`, or `terminate`. Use
  `dcc_voice_reconnect_action_for_close_code()` in logs or sidecars when only a
  raw close code is available.
- Use `dcc_voice_client_health_snapshot()` and
  `dcc_voice_client_health_snapshot_json()` for voice sidecar probes. The
  snapshot classifies `disconnected`, `connecting`, `ready`, `active`,
  `reconnecting`, `degraded`, and `terminated` states with reconnect action and
  media/websocket/UDP readiness booleans.
- Use `dcc_voice_client_connect_and_wait_until_ready()` when the probe owns the
  Opcode 4 join, so the descriptor waiter is registered before the join is sent
  and the managed voice client must reach READY before the probe proceeds.
- Use `dcc_voice_client_wait_until_ready()` and
  `dcc_voice_client_wait_until_active()` when the join or websocket loop is
  driven elsewhere and the probe only needs a health-state wait.
- Use `dcc_client_wait_for_voice_session_descriptor()` when validation needs to
  prove that Gateway delivered both the `VOICE_STATE_UPDATE` and
  `VOICE_SERVER_UPDATE` descriptors but another component already sent the join.
- Use `dcc_live_voice_probe` before release candidates that touch voice
  Gateway routing, Opcode 4 connect/leave, voice websocket session startup,
  UDP/select-protocol, close-code handling, or health snapshots.
- A healthy idle voice client should not grow `send_queue_size`,
  `receive_reorder_buffered`, `receive_packets_lost`, or
  `receive_packets_dropped` after media flow has stopped. A stuck websocket loop
  should show up through `websocket_loop_running` plus `websocket_loop_status`.

Interaction webhook validation notes:

- Use `dcc_interaction_server_health_snapshot()` for readiness and drain checks.
  It classifies the server as `ready`, `degraded`, `draining`, or `stopped` and
  includes the state, stats, booleans for `ready`/`accepting`/`drain_complete`,
  and a short reason.
- Use `dcc_interaction_server_add_health_route()` for the standard `/healthz`
  endpoint, or `dcc_interaction_server_health_snapshot_json()` when a sidecar or
  reverse-proxy probe needs the same fixed-buffer JSON body without registering
  a route.
- Use `dcc_interaction_server_drain()` from an external signal handler thread,
  sidecar, or supervisor path to stop accepting new connections and wait for
  active interaction callbacks before process shutdown.
- Use `dcc_interaction_server_wait_until_ready()` before routing traffic to a
  freshly started webhook process, and
  `dcc_interaction_server_wait_until_drained()` when a supervisor needs to
  observe drain completion without initiating another drain.
  The bundled `dcc_interaction_webhook` example wires SIGINT/SIGTERM to this
  drain path with `DCC_INTERACTION_DRAIN_TIMEOUT_MS`.
- Use `dcc_interaction_server_stats()` for counters and alerting. Unexpected
  growth in `read_errors`, `write_errors`, `spawn_errors`, `response_5xx`, or
  signature-related `response_4xx` counts should be investigated before exposing
  the endpoint through a public reverse proxy.
