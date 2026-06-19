# CLI Tools

DCC installs runtime helper tools when `DCC_BUILD_TOOLS=ON`.

## dcc_hot_reload_host

Runs a Discord client and forwards bot logic to a hot reload module.

```sh
dcc_hot_reload_host --check
dcc_hot_reload_host --global
dcc_hot_reload_host --guild 123456789012345678
```

Important environment variables:

- `DISCORD_TOKEN`
- `DCC_BOT_MODULE`
- `DCC_HOT_RELOAD_WORKER`
- `DCC_HOT_RELOAD_HEALTH_ADDRESS`
- `DCC_HOT_RELOAD_HEALTH_PORT`

## dcc_hot_reload_worker

Loads a bot module in an isolated worker process. It is normally launched by
`dcc_hot_reload_host`.

## dcc_command_sync

Plans or applies application command reconciliation.

```sh
dcc_command_sync --commands commands.json --application-id "$APP_ID" --plan
dcc_command_sync --commands commands.json --application-id "$APP_ID" --apply
dcc_command_sync --commands commands.json --application-id "$APP_ID" --guild "$GUILD_ID" --apply
```

Use `--remote remote.json` to compare against a captured remote fixture without
network access.

## dcc_replay

Summarizes, validates, or replays captured Discord JSONL payloads.

```sh
dcc_replay --file events.jsonl --summary
dcc_replay --file events.jsonl --validate
dcc_replay --file events.jsonl --module ./bot.so --isolated
```

## dcc_interaction_webhook

Runs the shardless interaction webhook example with Discord Ed25519
verification.

```sh
dcc_interaction_webhook --check
```

Important environment variables:

- `DISCORD_PUBLIC_KEY`
- `DCC_INTERACTION_ADDRESS`
- `DCC_INTERACTION_PORT`
- `DCC_INTERACTION_PATH`
