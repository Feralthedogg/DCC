# Components v2

Components v2 lets a message be built from layout components instead of legacy
`content` and `embeds`. DCC exposes this through `<dcc/sugar.h>` for compact
layouts and through `<dcc/component_v2.h>` for explicit builder control.

## Message Rules

When a message uses Components v2, DCC automatically sets
`DCC_MESSAGE_FLAG_IS_COMPONENTS_V2`. The builder rejects legacy fields that
Discord does not allow with this flag:

- `content`
- `embeds`
- `poll`
- `sticker_ids`
- legacy `dcc_component_builder_t` components

Attachments are still allowed. Reference them from v2 media components with
`attachment://filename`.

## Basic Layout

```c
dcc_message_builder_t message =
    DCC_MESSAGE_COMPONENTS_V2(
        DCC_V2_CONTAINER_ACCENT(
            0x5865F2,
            DCC_V2_SECTION(
                DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"),
                DCC_V2_TEXT("# Runtime status")
            ),
            DCC_V2_ACTION_ROW(
                DCC_V2_BUTTON_SECONDARY("Details", "status.details"),
                DCC_V2_BUTTON_DANGER("Close", "status.close")
            )
        )
    );
```

Inside an interaction handler, use the one-call reply/update helpers when the
message does not need to be stored:

```c
DCC_REPLY_V2(
    ctx,
    DCC_V2_CONTAINER_ACCENT(
        0x5865F2,
        DCC_V2_TEXT("## Runtime status"),
        DCC_V2_ACTION_ROW(DCC_V2_BUTTON_PRIMARY("Refresh", "status.refresh"))
    )
);
```

The v2 message macro sets `DCC_MESSAGE_FLAG_IS_COMPONENTS_V2` and hides the
short-lived component arrays.

Use the explicit builder API when you generate the tree incrementally or need
setter-level error handling:

```c
dcc_component_v2_builder_t title;
dcc_component_v2_builder_init_text_display(&title, "# Runtime status");

dcc_component_v2_builder_t refresh;
dcc_component_v2_builder_init_button(
    &refresh,
    DCC_BUTTON_PRIMARY,
    "Refresh",
    "status.refresh"
);

dcc_component_v2_builder_t section;
dcc_component_v2_builder_init_section(&section, &title, 1, &refresh);

dcc_component_v2_builder_t container;
dcc_component_v2_builder_init_container(&container, &section, 1);
dcc_component_v2_builder_set_accent_color(&container, 0x5865F2);

dcc_message_builder_t explicit_message;
dcc_message_builder_init(&explicit_message);
dcc_message_builder_set_components_v2(&explicit_message, &container, 1);
```

The variadic sugar macros create block-scoped compound literal arrays. Use the
matching `*_ARRAY` macro when you already have a persistent array and count.

## Media

Media Gallery, Thumbnail, and File components use `dcc_component_v2_media_t`.

```c
dcc_component_v2_builder_t gallery =
    DCC_V2_MEDIA_GALLERY(
        DCC_V2_MEDIA("attachment://graph.png", "Gateway latency graph")
    );
```

Use the `_ARRAY` form when media entries already live in an array:

```c
dcc_component_v2_media_t media[] = {
    DCC_V2_MEDIA("attachment://graph.png", "Gateway latency graph"),
};

dcc_component_v2_builder_t gallery =
    DCC_V2_MEDIA_GALLERY_ARRAY(media, DCC_ARRAY_LEN(media));
```

Add the matching attachment through the message builder:

```c
dcc_message_builder_set_attachments_json(
    &message,
    "[{\"id\":\"0\",\"filename\":\"graph.png\"}]"
);
```

## Select Defaults

Entity selects support `default_values`. Channel selects may also restrict
allowed channel types.

```c
dcc_component_v2_builder_t select =
    DCC_V2_CHANNEL_SELECT(
        "settings.channel",
        DCC_V2_DEFAULT_VALUES(DCC_V2_DEFAULT_CHANNEL(222)),
        DCC_V2_CHANNEL_TYPES(0U, 5U)
    );
```

For string selects, use `dcc_select_option_t::has_default` on options instead
of `default_values`:

```c
dcc_component_v2_builder_t string_select =
    DCC_V2_STRING_SELECT(
        "settings.mode",
        DCC_SELECT_OPTION("Fast", "fast"),
        DCC_SELECT_OPTION_DEFAULT("Safe", "safe")
    );
```

## Modal Inputs

The v2 builder also supports modal-only components such as Label, File Upload,
Radio Group, Checkbox Group, and Checkbox. A Label wraps exactly one input
component.

```c
dcc_component_v2_builder_t upload = DCC_V2_FILE_UPLOAD("config.upload");
upload.min_values = 1;
upload.max_values = 3;
upload.required = 1;
upload.has_min_values = 1;
upload.has_max_values = 1;
upload.has_required = 1;

dcc_modal_builder_t modal =
    DCC_MODAL_V2(
        "config-modal",
        "Upload config",
        DCC_V2_LABEL("Config files", upload),
        DCC_MODAL_V2_FIELD_CHECKBOX("config.accept", "Apply immediately", 1U)
    );
```

Checkbox uses Discord's `default` JSON field for its initial checked state:

```c
dcc_component_v2_builder_t accept =
    DCC_V2_CHECKBOX("config.accept", "Apply immediately", 1U);
```

## Reading Message Layouts

Gateway message objects keep both the original raw component JSON and a typed
Components v2 view. Use the typed accessors when you only need the layout tree:

```c
const dcc_message_t *message = dcc_event_message(event);

const dcc_component_v2_t *root = dcc_message_component_root(message, 0);
if (root != NULL && root->type == DCC_COMPONENT_V2_CONTAINER) {
    for (size_t i = 0; i < root->child_count; ++i) {
        const dcc_component_v2_t *child =
            dcc_component_v2_child(message, root, i);
        if (child != NULL && child->type == DCC_COMPONENT_V2_TEXT_DISPLAY) {
            /* child->content */
        }
    }
}

const dcc_component_v2_t *refresh =
    dcc_message_component_by_custom_id(message, "status.refresh");
if (refresh != NULL) {
    /* refresh->label, refresh->style, refresh->disabled */
}
```

The raw JSON remains available as `message->components_json` for fields DCC does
not project yet.

## Reading Submitted Values

Components v2 modal submits arrive in `interaction.form_fields`. Use the
accessors instead of walking nested component arrays yourself:

```c
const dcc_interaction_t *interaction = dcc_event_interaction(event);

const char *name = dcc_interaction_form_value(interaction, "profile.name");

uint8_t accepted = 0;
if (dcc_interaction_form_boolean(interaction, "config.accept", &accepted)) {
    /* checkbox value is available */
}

const dcc_interaction_form_field_t *tags =
    dcc_interaction_form_field_by_custom_id(interaction, "profile.tags");
if (tags != NULL && tags->value_type == DCC_INTERACTION_FORM_VALUE_VALUES) {
    for (size_t i = 0; i < tags->values_count; ++i) {
        /* tags->values[i] */
    }
}
```
