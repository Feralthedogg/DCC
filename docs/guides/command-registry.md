# Slash Command Registry

The command registry reconciler compares local command declarations with the
commands currently registered in Discord.

## Scope Defaults

DCC defaults command registration to global scope. Guild scope must be selected
explicitly.

```c
dcc_command_registry_options_t options;
dcc_command_registry_options_init(&options);
dcc_command_registry_options_set_global(&options);
```

Use one guild for fast development rollouts:

```c
dcc_command_registry_options_set_guild(&options, 123456789012345678ULL);
```

Global commands can take time to propagate through Discord. Treat a global
sync as a release action, not a rapid inner-loop development action.

## Plan Before Apply

```c
dcc_command_registry_t registry;
dcc_command_registry_init(&registry);

dcc_application_command_builder_t ping;
dcc_application_command_builder_init_chat_input(&ping, "ping", "Latency check");
dcc_command_registry_add_builder(&registry, &ping);

dcc_command_registry_plan_t plan;
dcc_command_registry_plan_init(&plan);
dcc_command_registry_plan(&registry, client, application_id, &options, &plan);
```

Plan actions are:

- `create`
- `update`
- `delete_stale`
- `noop`

Rename is represented as create plus delete because Discord commands do not
have a rename primitive.

## CLI

```sh
build/dcc_command_sync \
  --commands commands.json \
  --application-id "$APPLICATION_ID" \
  --token "$DISCORD_TOKEN" \
  --plan
```

Apply only when the plan is expected:

```sh
build/dcc_command_sync \
  --commands commands.json \
  --application-id "$APPLICATION_ID" \
  --token "$DISCORD_TOKEN" \
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
