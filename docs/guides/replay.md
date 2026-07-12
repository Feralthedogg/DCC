# Replay Testing

Replay files let you capture Discord payloads and run them through local bot
logic without reconnecting to Discord.

## JSONL Format

Each line is one event:

```json
{"kind":"interaction","ts":12345,"payload":{"id":"1","type":3}}
{"kind":"gateway","event":"MESSAGE_CREATE","ts":12346,"payload":{"id":"2","content":"hi"}}
```

Gateway records may be full Gateway frames or event payloads with an `event`
field. Interaction records are wrapped as `INTERACTION_CREATE`.

## Summarize

```sh
build/dcc_replay --file events.jsonl --summary
```

## Validate

```sh
build/dcc_replay --file events.jsonl --validate
```

Validation parses the entire outer record and payload JSON, rejects missing or
wrongly typed fields, caps each JSONL line at 8 MiB, and requires monotonic
timestamps. Use explicit expectations in CI so an accidentally empty or
partially captured fixture cannot pass:

```sh
build/dcc_replay --file events.jsonl --validate \
  --expect-records 120 --expect-gateway 100 --expect-interactions 20
```

`--max-records` bounds fixture work. `--allow-nonmonotonic` is available for
legacy captures, but deterministic fixtures should normally be sorted.

## API Records

When recording from a custom harness, use the sugar record literals:

```c
dcc_replay_record_t gateway =
    DCC_REPLAY_GATEWAY_RECORD("MESSAGE_CREATE", now_ms, payload, payload_len);

dcc_replay_record_t interaction =
    DCC_REPLAY_INTERACTION_RECORD(now_ms, payload, payload_len);
```

## Replay Into A Module

```sh
build/dcc_replay \
  --file events.jsonl \
  --module ./bot.so \
  --worker build/dcc_hot_reload_worker \
  --isolated
```

`--realtime` respects timestamps, with each sleep capped by `--max-gap-ms`
(default 5000) so a corrupt capture cannot stall CI indefinitely. Without it,
DCC replays as fast as the local runtime can dispatch.

Replay payloads can contain tokens, message content, and user data. Keep capture
files out of source control unless sanitized and store local captures with
owner-only permissions.

Replay is especially useful before promoting a hot reload candidate: run the
same captured payload file against the new module and fail the build if handler
behavior changes unexpectedly.
