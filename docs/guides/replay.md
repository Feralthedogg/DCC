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

Use the installed record layout. This function is compiled from
`examples/docs/guide_configuration.c`; the caller supplies an open recorder and
valid payload JSON bytes. Keep those bytes alive through each write and close
the recorder with `dcc_replay_recorder_close` when finished.

<!-- DCC_DOC_SNIPPET_BEGIN(replay-records) -->
```c
#include <dcc/replay.h>

dcc_status_t dcc_example_record_pair(dcc_replay_recorder_t *recorder,
    uint64_t now_ms, const char *message_json, size_t message_len,
    const char *interaction_json, size_t interaction_len
) {
    dcc_replay_record_t gateway = {
        .size = sizeof(gateway), .kind = DCC_REPLAY_GATEWAY,
        .ts_ms = now_ms, .event = "MESSAGE_CREATE",
        .payload = message_json, .payload_len = message_len,
    };
    dcc_replay_record_t interaction = {
        .size = sizeof(interaction), .kind = DCC_REPLAY_INTERACTION,
        .ts_ms = now_ms, .event = NULL,
        .payload = interaction_json, .payload_len = interaction_len,
    };
    dcc_status_t status = dcc_replay_recorder_write(recorder, &gateway);
    if (status == DCC_OK)
        status = dcc_replay_recorder_write(recorder, &interaction);
    /* These records borrow caller payloads; do not record_deinit them.
     * Deinit records returned by dcc_replay_player_next instead. */
    return status;
}
```
<!-- DCC_DOC_SNIPPET_END(replay-records) -->

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
