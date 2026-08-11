# Versioned Public Builders

DCC's public message and application-command builders are caller-owned,
size-versioned values. This contract lets an application compiled with an older
header pass a shorter record to a newer DCC library, and lets a newer
application pass a longer record to an older compatible library.

The contract applies to these nine public types:

- `dcc_message_builder_t`
- `dcc_poll_builder_t`
- `dcc_allowed_mentions_builder_t`
- `dcc_embed_builder_t`
- `dcc_modal_builder_t`
- `dcc_autocomplete_choice_t`
- `dcc_autocomplete_builder_t`
- `dcc_application_command_option_builder_t`
- `dcc_application_command_builder_t`

## Initialize Every Record

Every record starts with the same prefix:

```c
size_t size;
uint32_t version;
uint64_t present;
```

Use the initializer function for mutable storage:

```c
dcc_message_builder_t message;
dcc_message_builder_init(&message);

dcc_status_t status =
    dcc_message_builder_set_content(&message, "hello");
```

Use the constant initializer when a declaration needs an initializer expression:

```c
dcc_embed_builder_t embed = DCC_EMBED_BUILDER_INIT;
```

Do not use `{0}` or a field-only literal for these types. A zero `size` and
`version` is not a valid builder record.

## Presence Is Authoritative

The `present` mask determines which fields are supplied. The stored value does
not determine presence. This distinction matters for valid zero and false
values:

```c
dcc_message_builder_t message;
dcc_message_builder_init(&message);

/* Serializes `"flags": 0` because the setter marks the field present. */
dcc_message_builder_set_flags(&message, 0);
```

Setters update the value and its presence bit together. Passing `NULL` to a
nullable string setter or a zero count to a collection setter clears that
field's presence. When constructing a record directly, set the matching
`DCC_*_PRESENT_*` bit explicitly:

```c
dcc_application_command_option_builder_t option = {
    .size = sizeof(dcc_application_command_option_builder_t),
    .version = DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION,
    .present = DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME |
               DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION |
               DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE |
               DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED,
    .name = "query",
    .description = "Search query",
    .type = DCC_APPLICATION_COMMAND_OPTION_STRING,
    .required = 0
};
```

The last line intentionally serializes `required: false`. Without the presence
bit, the field is omitted.

## Size And Version Rules

DCC validates the prefix before reading payload fields.

- `version` must equal the type's current `DCC_*_VERSION` value.
- Unknown presence bits are rejected.
- A present field must fit completely inside the declared `size`.
- A shorter historical record is accepted when every present field is covered.
- A larger record is accepted; DCC ignores bytes it does not know.
- A setter validates its entire write set before modifying any byte.

This means trailing bytes in a historical or future record are never a source
of implicit values. Only covered, present fields are read.

## Arrays Use The Declared Byte Stride

Arrays of versioned builders are byte-strided. The first record's `size` is the
stride, and every record must declare that same size and version. The base
address and stride must satisfy the type's alignment. DCC rejects mismatched
records and count or address-span overflow before iterating the array.

Do not index a historical builder array with normal C pointer arithmetic in
adapter code. Public DCC serializers and the command registry already use the
declared byte stride.

## Ownership

Normal builders borrow their strings and arrays for the duration documented by
the receiving operation. The command registry is the exception: successful
registration recursively deep-copies strings, choices, nested options, and
channel-type arrays into current-layout records. Caller storage may be changed
or released after `dcc_command_registry_add_builder()` succeeds.

## Autocomplete Filtering Output

`dcc_ctx_autocomplete_filter_choices()` requires every output slot up to
`out_capacity` to be initialized, empty, and use one common declared stride:

```c
dcc_autocomplete_choice_t filtered[DCC_AUTOCOMPLETE_MAX_CHOICES];
for (size_t i = 0; i < DCC_AUTOCOMPLETE_MAX_CHOICES; ++i) {
    dcc_autocomplete_choice_init(&filtered[i], NULL);
}

size_t filtered_count = 0;
dcc_status_t status = dcc_ctx_autocomplete_filter_choices(
    ctx,
    choices,
    choice_count,
    filtered,
    DCC_AUTOCOMPLETE_MAX_CHOICES,
    &filtered_count
);
```

DCC validates all input and output records before writing the first result, so
an invalid later record cannot leave a partially updated output array.
Source records are validated even when the output capacity is zero. `out_count`
is required, is cleared before validation, and remains zero on every error. A
valid empty source with zero output capacity succeeds without requiring an
output array.

## Command Serialization Versus Creation

`dcc_application_command_builder_build_json()` intentionally accepts partial
builders because the same serializer is used for command `PATCH` requests. For
example, a builder containing only `description` is a valid edit payload.

Creation boundaries are stricter. Registry insertion and REST `POST`/bulk
`PUT` require a non-empty name, a supported command type, and a description for
chat-input and primary-entry-point commands. User and message commands omit
description and options. Invalid create schemas are rejected locally before a
request or registry mutation occurs.

The raw JSON fields on message and command builders are transition helpers.
Prefer typed embeds, components, polls, mentions, choices, and options for new
code so validation and versioned-array handling remain available.
