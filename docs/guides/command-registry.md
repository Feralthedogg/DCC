# Slash Command Registry

The command registry reconciler compares local command declarations with the
commands currently registered in Discord.

## Scope Defaults

DCC defaults command registration to global scope. Guild scope must be selected
explicitly.

```c
dcc_command_registry_options_t options =
    DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL();
```

Use one guild for fast development rollouts:

```c
dcc_command_registry_options_t guild_options =
    DCC_COMMAND_REGISTRY_OPTIONS_GUILD(123456789012345678ULL);
```

Global commands can take time to propagate through Discord. Treat a global
sync as a release action, not a rapid inner-loop development action.

## Plan Before Apply

```c
dcc_command_registry_t registry;
dcc_command_registry_init(&registry);

dcc_application_command_builder_t ping =
    DCC_SLASH_COMMAND("ping", "Latency check");
dcc_command_registry_add_builder(&registry, &ping);

dcc_command_registry_remote_entry_t remote[] = {
    {
        .size = sizeof(dcc_command_registry_remote_entry_t),
        .name = "old-command",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .canonical_json = "{\"name\":\"old-command\",\"type\":1}",
    },
};

dcc_command_registry_plan_t plan;
dcc_command_registry_build_plan(
    &registry,
    &options,
    remote,
    DCC_ARRAY_LEN(remote),
    &plan
);
```

Plan actions are:

- `create`
- `update`
- `delete_stale`
- `noop`

Rename is represented as create plus delete because Discord commands do not
have a rename primitive.

Use `DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DELETE_STALE()` or
`DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DELETE_STALE(guild_id)` when stale remote
commands should be removed during apply.

## CLI

```sh
build/dcc_command_sync \
  --commands commands.json \
  --application-id "$APPLICATION_ID" \
  --token "$DCC_TOKEN" \
  --plan
```

Apply only when the plan is expected:

```sh
build/dcc_command_sync \
  --commands commands.json \
  --application-id "$APPLICATION_ID" \
  --token "$DCC_TOKEN" \
  --apply
```

Guild scope:

```sh
build/dcc_command_sync \
  --commands commands.json \
  --application-id "$APPLICATION_ID" \
  --guild "$GUILD_ID" \
  --apply
```
