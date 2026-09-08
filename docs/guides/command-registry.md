# Slash Command Registry

The command registry reconciler compares local command declarations with the
commands currently registered in Discord.

## Scope Defaults

DCC defaults command registration to global scope. Guild scope must be selected
explicitly.

Start with `DCC_COMMAND_REGISTRY_OPTIONS_INIT`, then select scope with
`dcc_command_registry_options_set_global` or
`dcc_command_registry_options_set_guild`. Use one guild for development rollouts.

Global commands can take time to propagate through Discord. Treat a global
sync as a release action, not a rapid inner-loop development action.

## Plan Before Apply

This offline example is compiled from `examples/docs/guide_configuration.c`.
After inspecting a successful plan, release it with
`dcc_command_registry_plan_deinit`. The fixture is not a live remote snapshot.

<!-- DCC_DOC_SNIPPET_BEGIN(command-registry) -->
```c
#include <dcc/command_registry.h>

/* Pure planning: no Discord connection. Pass 0 for global scope, otherwise
 * an explicit guild id. The caller owns a successful out_plan. */
dcc_status_t dcc_example_command_plan(dcc_snowflake_t guild_id,
                                      dcc_command_registry_plan_t *out_plan) {
    dcc_command_registry_options_t options = DCC_COMMAND_REGISTRY_OPTIONS_INIT;
    dcc_status_t status = guild_id == 0U
        ? dcc_command_registry_options_set_global(&options)
        : dcc_command_registry_options_set_guild(&options, guild_id);
    if (status != DCC_OK) return status;
    status = dcc_command_registry_options_set_delete_stale(&options, 1U);
    if (status != DCC_OK) return status;

    dcc_command_registry_t registry;
    dcc_command_registry_init(&registry);
    dcc_application_command_builder_t ping = DCC_APPLICATION_COMMAND_BUILDER_INIT;
    status = dcc_application_command_builder_set_type(
        &ping, DCC_APPLICATION_COMMAND_CHAT_INPUT);
    if (status == DCC_OK)
        status = dcc_application_command_builder_set_name(&ping, "ping");
    if (status == DCC_OK)
        status = dcc_application_command_builder_set_description(&ping, "Latency check");
    if (status == DCC_OK) status = dcc_command_registry_add_builder(&registry, &ping);
    /* Fixture only: replace with the actual remote snapshot when reconciling. */
    dcc_command_registry_remote_entry_t remote[] = {{
        .size = sizeof(dcc_command_registry_remote_entry_t),
        .id = 123U,
        .name = "old-command",
        .type = DCC_APPLICATION_COMMAND_CHAT_INPUT,
        .canonical_json = "{\"name\":\"old-command\",\"type\":1}",
    }};
    if (status == DCC_OK)
        status = dcc_command_registry_build_plan(&registry, &options, remote,
            sizeof(remote) / sizeof(remote[0]), out_plan);
    dcc_command_registry_deinit(&registry);
    return status;
}
```
<!-- DCC_DOC_SNIPPET_END(command-registry) -->

Plan actions are:

- `create`
- `update`
- `delete_stale`
- `noop`

Rename is represented as create plus delete because Discord commands do not
have a rename primitive.

Use `dcc_command_registry_options_set_delete_stale(&options, 1U)` only when
stale remote commands should be removed; the default is disabled. To apply a
reviewed plan, use `dcc_command_registry_apply` and retain/release its operation
handle as described in the [registry API](../reference/api/command_registry.md).

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
