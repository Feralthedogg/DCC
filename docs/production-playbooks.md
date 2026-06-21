# Production Playbooks

These playbooks describe the operational behavior DCC already exposes and the
checks to run before treating a bot deployment as release-ready.

## Gateway And Cluster

Use `dcc_cluster_recovery_plan()` as the single supervisor snapshot. It returns
aggregate health, a prioritized action, the selected shard, and a reason string.
Use `dcc_cluster_status_json()` when a `/healthz` route or sidecar needs one
fixed-buffer response containing recovery-plan, rolling-operation, and
supervisor decision status. Use `dcc_cluster_add_status_route()` to attach that response as an
unsigned `GET /clusterz` route on a `dcc_interaction_server_t` sidecar.
Use `dcc_cluster_recovery_plan_json()`,
`dcc_cluster_remediation_result_json()`, or
`dcc_cluster_remediation_loop_result_json()` when a `/healthz` route or sidecar
needs a fixed-buffer JSON body.
Use `dcc_cluster_operation_status_json()` when the same route needs to expose
the current rolling reconnect plan, active batch, progress counters, and last
operation status without heap allocation.
Use `dcc_cluster_wait_until_ready()` from the supervisor/control thread after
`dcc_cluster_start()` when deploy admission should wait for every shard to reach
READY. It returns the final health summary on success and fails early with
`DCC_ERR_STATE` when a shard closes, stops, logs an error, or enters error state
before startup completes.
Use `dcc_cluster_health_wait_options_init()` and
`dcc_cluster_wait_until_health()` when admission or a supervisor loop needs the
recovery-plan classifier itself to reach a target state, usually
`DCC_CLUSTER_HEALTH_HEALTHY`. The default options wait 30 seconds, poll with
LLAM-backed 10 ms sleeps, fail fast on CRITICAL health, and return the last
recovery plan for deployment logs or sidecar JSON.
Use `dcc_cluster_supervisor_status()` when alerts need to separate policy
errors, restart scheduling, and restart suppression without parsing the full
cluster-status JSON document. Use
`dcc_cluster_set_supervisor_decision_callback()` when metrics or logs need every
restart/suppress/default decision as it happens; keep the callback short because
it runs on the supervisor task. Use `dcc_cluster_supervisor_decision_event_json()`
from that callback when logs need the exact decision event, action, policy,
status snapshot, and shard snapshot in one fixed-buffer payload.
Use `dcc_cluster_remediate()` when a sidecar should apply DCC's conservative
default action directly. It reconnects shards for reconnect plans by default;
restart-budget rearm remains opt-in through `allow_rearm_restart`.
Use `dcc_cluster_remediation_loop_options_init()` followed by
`dcc_cluster_remediate_loop()` when a supervisor should apply the conservative
action repeatedly with a bounded attempt count and LLAM-backed interval.

Alert on:

- `health=critical`
- `action=rearm_restart`
- repeated shard `restarts`
- non-zero `socket_closes`, `log_errors`, or `error` shard counts
- `/gateway/bot` session-start-limit exhaustion

First response:

1. Read the recovery plan, call `dcc_cluster_remediate()` once, or run
   `dcc_cluster_remediate_loop()` with an explicit attempt bound.
2. If the action is `rearm_restart`, set `allow_rearm_restart` only after
   confirming the restart budget should reset, or call the explicit rearm API
   for the selected shard/all shards.
3. If identify sessions are low, stop rollout and wait for Discord reset timing
   before admitting more shards.
4. Run the local chaos suite before redeploying gateway changes:

```sh
ctest --test-dir build -R '^dcc_(gateway|cluster)_chaos_smoke$' --output-on-failure
```

For planned restarts, prefer the rolling reconnect controls and keep the shard
window at or below the discovered `max_concurrency`. The `dcc_cluster_rollout`
example accepts `DCC_CLUSTER_PRINT_JSON=1` to print the same fixed-buffer
cluster-status JSON a sidecar would expose.

## REST And Rate Limits

DCC serializes major-route buckets and keeps 429 retry queues from occupying
active worker slots. Supervisors should still watch retry counters and long
pending queues.

Alert on:

- sustained 429 retries on one route
- growing REST pending counts while active workers are saturated
- unexpected 401/403 responses after token or permission changes

First response:

1. Identify whether the route is global, major-route, or application scoped.
2. Check `Retry-After`, `X-RateLimit-Bucket`, and route path in logs.
3. Reduce producer concurrency before raising global REST concurrency.
4. Add a request-level smoke assertion for every new REST wrapper.

## Interaction Webhook

Bind the built-in interaction server to loopback or a private interface and
terminate TLS at a reverse proxy. Preserve the raw body and the Discord
signature headers exactly.
Use `deploy/interaction-webhook/` as the baseline systemd/nginx, Compose/Caddy,
or Kubernetes template when deploying the bundled `dcc_interaction_webhook`
example.
The example handles SIGINT/SIGTERM by draining active requests for
`DCC_INTERACTION_DRAIN_TIMEOUT_MS` before forcing runtime stop.
In Kubernetes, keep `terminationGracePeriodSeconds` above that drain timeout and
let the provided initContainer run `dcc_interaction_webhook --check` before the
pod becomes eligible for traffic.

Expose:

- `/healthz` using `dcc_interaction_server_add_health_route()`
- drain hooks using `dcc_interaction_server_drain()`
- deploy/shutdown probes using `dcc_interaction_server_wait_until_ready()` and
  `dcc_interaction_server_wait_until_drained()`
- counters from `dcc_interaction_server_stats()`

Alert on:

- `health=degraded`
- rising `read_errors`, `write_errors`, or `spawn_errors`
- signature-related 4xx growth
- response 5xx growth
- active requests not draining during shutdown

First response:

1. Verify reverse-proxy body buffering and header forwarding.
2. Check configured max header/body sizes against Discord payload size.
3. Drain before process shutdown or module replacement; keep systemd
   `TimeoutStopSec` above `DCC_INTERACTION_DRAIN_TIMEOUT_MS`.
4. Re-run `dcc_interaction_server_smoke`.

## Hot Reload

Use hot reload when bot business logic should change without dropping Gateway or
Voice sessions. The host owns `dcc_client_t`; the module owns only handler logic
and optional DCC-preserved state.
Use `deploy/hot-reload/` as the baseline systemd or Compose/Caddy template when
deploying the installed isolated host/worker pair. The templates keep the
module under `/var/lib/dcc/hot_reload/bot.so`, run
`dcc_hot_reload_host --check` before opening Gateway where systemd is used, and
expect module rollouts to replace that file atomically.
Use the installed `dcc_hot_reload_host` plus `DCC_BOT_MODULE` and `BOT_TOKEN`
for the smallest no-code isolated development host when DCC's installed `bin`
directory is on `PATH`. Installed builds include both `dcc_hot_reload_host` and
`dcc_hot_reload_worker` by default through `DCC_BUILD_TOOLS=ON`, even when
examples and tests are disabled. Set `DCC_HOT_RELOAD_WORKER` only when you need
to override that worker executable. The installed host defaults to the `guilds`
intent for slash-command bots; pass `--guild <id>` for guild-only development
command registration, pass `--global` to clear inherited guild scoping, and set
`DCC_HOT_RELOAD_INTENTS` or pass `--intents` when
a module needs message, presence, voice, or other Gateway events; use
`DCC_HOT_RELOAD_INTENTS=all` only when every supported intent is needed and
privileged intents are enabled in the Developer Portal. It loads
`.env` from the current directory without overriding exported variables, accepts
`DISCORD_TOKEN` as a `BOT_TOKEN` fallback, and supports
`dcc_hot_reload_host --check` for no-network config validation. `--check`
verifies token presence, module readability, and worker executability or
`PATH` discovery, and reports optional health sidecar settings. The deployment
template enables loopback `GET /hot-reloadz` with
`DCC_HOT_RELOAD_HEALTH_ADDRESS=127.0.0.1` and
`DCC_HOT_RELOAD_HEALTH_PORT=18080`; remove the port or leave it empty to disable
that sidecar. Tune watcher and worker timing with
`DCC_HOT_RELOAD_POLL_MS`, `DCC_HOT_RELOAD_SETTLE_MS`,
`DCC_HOT_RELOAD_WORKER_HEALTH_MS`, and `DCC_HOT_RELOAD_WORKER_DRAIN_MS`.
The worker health timeout covers candidate readiness, idle supervisor IPC
health pings, event dispatch sends, event dispatch replies, worker READY
frames, and worker result payload reads. Stalled or half-written worker frames
surface as timeout failures instead of blocking the parent process, which keeps
Gateway ownership available for last-good retry or temporary interaction errors.
Workers also enforce a maximum forwarded event JSON size and bounded child-side
EVENT body reads. During shutdown, the host sends STOP, waits for the configured
drain timeout, and then escalates termination with a bounded grace window.
Set `DCC_HOT_RELOAD_SETTLE_MS=none` only when module replacement is explicitly
atomic. Use
`dcc_hot_reload_attach()` when a production host owns client startup and
sidecar/admin route setup. For explicit custom isolation, set
`dcc_hot_reload_options_t::backend` to
`DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER` and provide `worker_path`; the parent
keeps Gateway/cache ownership while the `.so` is loaded and executed only in a
worker process. `worker_path` defaults to `dcc_hot_reload_worker` in the
convenience runners and can be a direct path or an executable name found through
`PATH`. Candidate workers must pass `dlopen + init` before routing is
swapped, and the previous active worker remains available as last-good retry
state. The LLAM supervisor probes idle workers with IPC health pings and can
promote last-good before the next event if the active worker stops responding.
If an interaction cannot be completed by the active worker or the last-good
retry, the parent sends an ephemeral temporary error response so the Discord
command does not hang silently.

Deploy sequence:

1. Build the new module to a temporary file.
2. Atomically replace the watched module path.
3. Wait for `dcc_hot_reload_wait_for_generation()` when a specific generation is
   required, or `dcc_hot_reload_wait_until_loaded()` when any loaded
   module/worker is acceptable.
4. Expose `dcc_hot_reload_health_snapshot_json()` to confirm `watching` and the
   expected generation. In isolated worker mode, also confirm `backend`,
   `worker_supervisor_running`, `active_worker.generation`,
   `active_worker.pid`, `last_good_worker.generation`, and
   `last_good_worker.pid`; track `worker_events` counters for crash recovery
   and fallback interaction responses before shifting traffic. Use
   `dcc_hot_reload_add_health_route()` when the same body should be served by an
   unsigned `GET /hot-reloadz` sidecar route. With the installed host, set
   `DCC_HOT_RELOAD_HEALTH_PORT` or pass `--health-port` instead of writing a
   custom route.

Rollback is the same operation with the previous module file. If the module is
broken, DCC keeps the last good generation and reports degraded health with the
last load error and last-good worker metadata.

Before release or a risky handler rollout, run
`BOT_TOKEN=... make -C examples/hot_reload live-soak` against a development bot.
It keeps one Gateway session open, repeatedly rebuilds the watched module, and
waits for each expected isolated-worker generation before exiting.

## Voice

Use `dcc_voice_client_stats()`, `dcc_voice_client_session_info()`, and
`dcc_voice_client_health_snapshot()` together. Stats explain media flow;
session info explains Discord voice session state; health explains what a
supervisor should do.
Use `dcc_voice_client_wait_until_ready()` after the voice state/server updates
are expected to have arrived, and `dcc_voice_client_wait_until_active()` after
the voice websocket is expected to finish session description. Both helpers use
health snapshots and return the last observed snapshot to the caller.

Alert on:

- `health=degraded` or `terminated`
- websocket loop stopped while the client should be connected
- UDP/RTP readiness false after voice ready
- increasing dropped/lost packet counters after media flow stabilizes
- reconnect action repeatedly flipping between resume and full reconnect

First response:

1. Log the last voice close code and
   `dcc_voice_reconnect_action_for_close_code()`.
2. For resumeable close codes, allow the managed loop to resume.
3. For full reconnect codes, reissue Opcode 4 connect through the managed voice
   APIs.
4. For terminal codes such as permission or deleted-channel failures, stop media
   producers and surface the failure to bot logic.

DAVE/MLS frame bridge helpers allow external MLS engines to integrate with DCC.
Built-in full MLS group crypto is still roadmap work, so production DAVE
deployments should keep the MLS engine boundary explicit.

For supervised voice joins, use
`dcc_voice_client_connect_and_wait_until_ready()` when the supervisor owns the
Opcode 4 join. It arms the descriptor waiter first, sends the join, waits for
the Gateway `VOICE_STATE_UPDATE` and `VOICE_SERVER_UPDATE` pair, then waits for
the managed voice client to reach READY. Use
`dcc_client_wait_for_voice_session_descriptor()` or the matching wait policy
initializer when another component has already sent the join and the supervisor
only needs to observe descriptor delivery.
Run `dcc_live_voice_probe` against a development guild before a voice-related
release. It exercises the same connect-and-wait helper, managed voice READY
wait, and optional websocket ACTIVE wait that a production supervisor should
depend on.

## Wait Policies

Use the lowest-level wait primitive that matches the workflow:

- `wait_for_event_any`: race a small set of acceptable next events.
- `wait_for_event_sequence`: enforce ordered protocol progress.
- `wait_for_event_set`: require unordered completion of a known set.
- `wait_for_event_count`: collect repeated filtered events.
- `wait_for_event_quorum`: proceed after distinct event types reach quorum.
- `dcc_event_wait_policy_init_*`: prepare reusable policy structs for composed
  waits without hand-filling the mode and target-count fields.
- `dcc_client_wait_for_gateway_ready_or_resumed`: wait for either Gateway READY
  or RESUMED during startup and reconnect workflows.
- `dcc_client_wait_for_gateway_ready_resumed_or_close`: wait for Gateway
  READY/RESUMED success or early SOCKET_CLOSE failure in supervisor startup
  workflows.
- `dcc_client_wait_until_gateway_ready`: same startup gate, but returns
  `DCC_ERR_STATE` directly when the observed event is SOCKET_CLOSE.
- `dcc_client_wait_until_interaction`: same interaction supervisor gate as
  `dcc_client_wait_for_interaction_or_close`, but reports SOCKET_CLOSE as
  `DCC_ERR_STATE`.
- `dcc_client_wait_until_interaction_owned`: admission gate that returns an
  owned interaction clone only for interaction events and keeps close snapshots
  available for recovery paths.
- typed object waits: use when the caller needs owned object clones.
- task groups: join higher-level work and cancel losers explicitly.

For long workflows, prefer composing small waits with task groups instead of
keeping one broad listener alive indefinitely.

## Release Gate

Before tagging:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
tools/release_check.sh
```

For Discord-facing validation:

```sh
BOT_TOKEN=... DCC_SOAK_SECONDS=900 ./build/dcc_live_gateway_soak
```

Do not tag when local chaos tests, package consumer tests, sanitizer tests, or
the live soak contradict the expected health/recovery-plan state.
