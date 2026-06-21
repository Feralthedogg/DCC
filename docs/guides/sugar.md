# Sugar-First API

DCC documents common construction with `<dcc/sugar.h>` first. The sugar layer is
just C compound literals and defaults over the public structs. It keeps short,
static values readable while leaving the explicit builder and setter APIs
available for generated, conditional, or long-lived data.

```c
#include <dcc/sugar.h>
```

Use focused headers when you are writing a narrow library module, but application
code can normally start with `sugar.h`.

## Client Options

```c
dcc_client_options_t options =
    DCC_CLIENT_OPTIONS(getenv("DISCORD_TOKEN"), DCC_INTENTS_DEFAULT);
```

For sharded clients:

```c
dcc_client_options_t options =
    DCC_CLIENT_SHARDED_OPTIONS(token, DCC_INTENTS_MESSAGES, shard_id, shard_count);
```

## Commands

```c
dcc_application_command_builder_t ping =
    DCC_SLASH_COMMAND("ping", "Latency check");

dcc_application_command_builder_t inspect_user =
    DCC_USER_COMMAND_BUILDER("Inspect user");

dcc_application_command_builder_t quote_message =
    DCC_MESSAGE_COMMAND_BUILDER("Quote message");
```

Registry options default to global command scope. Pick a guild explicitly for
development rollouts:

```c
dcc_command_registry_options_t global_plan =
    DCC_COMMAND_REGISTRY_OPTIONS_GLOBAL_DRY_RUN();

dcc_command_registry_options_t guild_apply =
    DCC_COMMAND_REGISTRY_OPTIONS_GUILD_DELETE_STALE(guild_id);
```

## Messages And Embeds

```c
dcc_message_builder_t text =
    DCC_MESSAGE_TEXT("pong");

dcc_message_builder_t private_text =
    DCC_MESSAGE_EPHEMERAL("Only you can see this.");

dcc_message_builder_t status =
    DCC_MESSAGE_TEXT_EMBEDS(
        "ready",
        DCC_EMBED_WITH_FIELDS(
            "Runtime",
            "Gateway online",
            DCC_EMBED_FIELD_INLINE("Shard", "0"),
            DCC_EMBED_FIELD("Mode", "production")
        )
    );
```

Use `DCC_MESSAGE_EMBEDS_ARRAY` and `DCC_EMBED_WITH_FIELDS_ARRAY` when the embed
or field arrays already exist.

## Legacy Components

```c
dcc_message_builder_t controls =
    DCC_MESSAGE_COMPONENTS(
        DCC_ACTION_ROW(
            DCC_BUTTON_PRIMARY_BUILDER("Refresh", "status.refresh"),
            DCC_BUTTON_DANGER_BUILDER("Close", "status.close")
        )
    );
```

String select options can be written inline:

```c
dcc_component_builder_t select =
    DCC_STRING_SELECT_BUILDER(
        "settings.mode",
        DCC_SELECT_OPTION("Fast", "fast"),
        DCC_SELECT_OPTION_DEFAULT("Safe", "safe")
    );
```

## Components v2

Components v2 message sugar sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` for you.

```c
dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            ),
            DCC_V2_SEPARATOR_SMALL(),
            DCC_V2_MEDIA_GALLERY(
                DCC_V2_MEDIA("attachment://latency.png", "Latency graph")
            )
        )
    );
```

Entity select defaults can also stay inline:

```c
dcc_component_v2_builder_t channel_select =
    DCC_V2_CHANNEL_SELECT(
        "settings.channel",
        DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(default_channel_id)),
        DCC_V2_CHANNEL_TYPES(0U, 5U)
    );
```

## Modals

```c
dcc_modal_builder_t modal =
    DCC_MODAL_V2_BUILDER(
        "settings-modal",
        "Settings",
        DCC_V2_LABEL(
            "Display name",
            DCC_V2_TEXT_INPUT("profile.name", "Name", DCC_TEXT_INPUT_SHORT)
        ),
        DCC_V2_LABEL(
            "Apply immediately",
            DCC_V2_CHECKBOX("settings.apply", "Apply after submit", 1U)
        )
    );
```

When a component needs fields not covered by a compact macro, create the sugar
value and then set the extra fields:

```c
dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("config.upload");
upload.min_values = 1U;
upload.max_values = 3U;
upload.required = 1U;
upload.has_min_values = 1U;
upload.has_max_values = 1U;
upload.has_required = 1U;
```

## Component Sessions

Session buttons still go through `dcc_component_session_button*()` because DCC
must sign the `custom_id`. Sugar covers the option objects around that flow.

```c
dcc_component_session_options_t session_options =
    DCC_COMPONENT_SESSION_OPTIONS_USER(secret, secret_len, interaction_user_id);
session_options.now_ms = now_ms;
session_options.ttl_ms = 300000U;
session_options.channel_id = channel_id;
session_options.lock_channel = 1U;

dcc_component_session_t session;
dcc_component_session_create(&session_options, &session);

dcc_component_v2_builder_t next;
dcc_component_session_button_v2(
    &session,
    "next",
    "Next",
    DCC_BUTTON_PRIMARY,
    &next
);
```

## Interaction Flows

```c
dcc_interaction_flow_t flow =
    DCC_FLOW_AUTO_DEFER_EPHEMERAL(client, interaction, 1500U);

dcc_message_builder_t message =
    DCC_MESSAGE_EPHEMERAL("Working...");

dcc_flow_reply(&flow, &message, NULL, NULL);
```

## Operations

```c
dcc_hot_reload_canary_options_t canary =
    DCC_HOT_RELOAD_CANARY_OPTIONS_DEFAULT();

dcc_hot_reload_options_t hot_reload =
    DCC_HOT_RELOAD_ISOLATED_CANARY_OPTIONS("dcc_hot_reload_worker", canary);

dcc_rest_firewall_options_t firewall =
    DCC_REST_FIREWALL_OPTIONS();

dcc_replay_record_t record =
    DCC_REPLAY_GATEWAY_RECORD("READY", now_ms, payload, payload_len);
```

## Lifetime Rules

Variadic macros create block-scoped compound literal arrays. This is ideal when
the value is immediately serialized, registered, or passed into DCC during the
same block.

Use an explicit array or the matching `*_ARRAY` macro when data must outlive the
current block or when it is generated dynamically:

```c
dcc_component_v2_builder_t rows[2];
size_t row_count = build_rows(rows, DCC_ARRAY_LEN(rows));

dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2_ARRAY(rows, row_count);
```

Use the explicit builder API when you need validation after each setter, complex
branching, dynamic allocation, or ABI-stable objects returned from another
module.
