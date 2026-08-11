# Task 12 brief — tagged, versioned Components v2

Implement only Task 12 from
`docs/superpowers/plans/2026-08-11-dcc-2.0-api-redesign.md`. The
`Components v2 builder` and `ABI-safe Value and Builder Types` sections of the
approved design, together with this brief, are authoritative. Work test-first,
do not push, and do not edit the ignored progress ledger. Keep every
intermediate commit buildable.

## Outcome and scope

Components v2 is already functionally broad: the current builder, validator,
and serializer recognize all 20 component type values currently documented by
Discord; enforce much of the message/modal placement grammar; integrate with
the message and modal builders; and share exact build/measure encoding after
Task 6. Task 12 is not a second Components v2 implementation. It replaces the
unsafe public construction ABI and closes semantic gaps without regressing that
coverage.

The current `dcc_component_v2_builder_t` is an unversioned flat bag containing
the fields of every component kind plus sixteen `has_*` bytes. Setters can
write fields that do not belong to the active type, validators and serializers
read the current full layout directly, and recursive arrays use
`sizeof(dcc_component_v2_builder_t)` as their stride. Many inactive fields are
silently ignored, so malformed cross-kind values can serialize to apparently
valid but ambiguous JSON. Those properties are incompatible with the DCC 2 ABI
contract.

This task must:

- replace the flat builder with one size/versioned tagged union;
- make a 64-bit presence mask authoritative and delete builder `has_*` bytes;
- provide allocation-free, by-value constructors;
- read historical DCC 2 prefixes and recursive arrays without over-read;
- return a stable, named field path for validation failures;
- make validation, JSON building, and exact measurement use one traversal;
- correct the Discord-schema mismatches listed below; and
- migrate all in-tree low-level, Sugar-transition, message, modal, endpoint,
  package-consumer, and component-session callers.

The parser-owned `dcc_component_v2_t` response view is not the builder. Do not
fold it into the tagged builder or change its ownership model in this task.
Parser views may be versioned separately under the general DCC 2 view policy.

The implementer must preserve this current functionality while replacing its
construction model:

| Existing working surface | ABI/design gap Task 12 closes |
| --- | --- |
| 20 type values: Action Row, Button, five Selects, Text Input, Section, Text Display, Thumbnail, Media Gallery, File, Separator, Container, Label, File Upload, Radio Group, Checkbox Group, Checkbox | one flat builder stores every kind's fields and permits cross-kind state |
| message, modal, and generic validation contexts | only a status is returned; modal roots and some context-only fields are stale |
| recursive layout and duplicate ID/custom-ID checks | recursion indexes current structs directly and cannot consume historical strides |
| JSON build/free and Task 6 exact count-only measurement | build and measure depend on the same flat full-layout reads rather than a normalized tagged view |
| message/modal builders, endpoint preflight, Sugar, package consumer, and component-session integrations | many callers use direct flat designated initializers and must migrate atomically |
| parser-owned response view plus child/accessory/label accessors | response parsing is a separate concern and must not be destabilized by the request-builder rewrite |

## Hard DCC 2.0 compatibility cut

DCC 1.x source and binary compatibility is not preserved. Do not attempt to
interpret an old flat `dcc_component_v2_builder_t` as the new structure. Its
first word contains the old component type, not a trustworthy size, and an old
array has no recoverable byte stride. Heuristics would make small types
ambiguous with plausible sizes and would still permit out-of-bounds reads.

The compatibility baseline is therefore:

- old flat 1.x builder bytes are always rejected or recompiled, never probed;
- the first released tagged layout is Components v2 builder version 1;
- version 1 and valid historical prefixes of that tagged layout remain
  supported throughout DCC 2.x;
- all future evolution is suffix-only; and
- the release/shared-library boundary remains the already-approved 2.0.0 ABI
  major 2 boundary.

Document this hard cut in the implementation report and later migration table.
It is not a compatibility defect.

## Public header ownership

Create `include/dcc/component_types.h` and move the component-independent
shared request leaf types out of legacy `include/dcc/component.h`:

- `dcc_button_style_t`;
- `dcc_text_input_style_t`;
- `dcc_component_emoji_t`; and
- `dcc_select_option_t`.

Both `include/dcc/component.h` and `include/dcc/component_v2.h` include the new
header, so current source still sees the same names through the old include.
`component_v2.h` must no longer include `component.h`. This removes the final
structural dependency from the canonical Components v2 surface to the legacy
builder that Task 14 deletes.

Keep fixed leaf records fixed-size for ABI 2. Add distinct V2 leaf records where
the Discord shapes differ instead of extending an unrelated leaf:

```c
typedef struct dcc_component_v2_unfurled_media {
    const char *url;
} dcc_component_v2_unfurled_media_t;

typedef struct dcc_component_v2_media_gallery_item {
    dcc_component_v2_unfurled_media_t media;
    const char *description;
    uint8_t spoiler;
    uint8_t has_spoiler;
} dcc_component_v2_media_gallery_item_t;

typedef struct dcc_component_v2_choice_option {
    const char *label;
    const char *value;
    const char *description;
    uint8_t is_default;
    uint8_t has_default;
} dcc_component_v2_choice_option_t;

typedef enum dcc_component_v2_select_default_type {
    DCC_COMPONENT_V2_SELECT_DEFAULT_USER = 1,
    DCC_COMPONENT_V2_SELECT_DEFAULT_ROLE = 2,
    DCC_COMPONENT_V2_SELECT_DEFAULT_CHANNEL = 3
} dcc_component_v2_select_default_type_t;

typedef struct dcc_component_v2_select_default_value {
    dcc_snowflake_t id;
    dcc_component_v2_select_default_type_t type;
} dcc_component_v2_select_default_value_t;
```

String Select continues to use `dcc_select_option_t`, which can contain an
emoji. Radio Group and Checkbox Group use
`dcc_component_v2_choice_option_t`, which cannot represent an emoji. Thumbnail
and File use one unfurled media value. Media Gallery alone uses gallery items
with description/spoiler state. This prevents the current File description and
duplicate-spoiler ambiguity by construction.

## Canonical builder ABI

The common prefix order is exact and deliberately matches the Task 5 private
ABI helpers:

```c
typedef struct dcc_component_v2_builder {
    size_t size;
    uint32_t version;
    uint64_t present;
    dcc_component_v2_type_t type;
    uint32_t id;
    union {
        dcc_component_v2_button_value_t button;
        dcc_component_v2_select_value_t select;
        dcc_component_v2_layout_value_t layout;
        dcc_component_v2_media_value_t media;
        dcc_component_v2_text_input_value_t text_input;
        dcc_component_v2_modal_value_t modal;
        dcc_component_v2_text_display_value_t text_display;
        dcc_component_v2_separator_value_t separator;
    } as;
} dcc_component_v2_builder_t;
```

Do not reorder this as `size/version/type/present`. Keeping
`size/version/present` first allows the generic Task 5 prefix reader and array
span rules to be reused without offset-dependent special cases. `id` is common
metadata and its presence bit determines whether it is serialized; a present
zero retains Discord's documented behavior of being treated as empty and
reassigned.

The nested ABI is exact, including member order and scalar width. Forward-
declare `dcc_component_v2_builder_t`, then publish these definitions before the
complete builder:

```c
typedef struct dcc_component_v2_builder dcc_component_v2_builder_t;

typedef struct dcc_component_v2_button_value {
    dcc_button_style_t style;
    const char *label;
    dcc_component_emoji_t emoji;
    uint8_t disabled;
    union {
        const char *custom_id;
        const char *url;
        dcc_snowflake_t sku_id;
    } target;
} dcc_component_v2_button_value_t;

typedef struct dcc_component_v2_select_value {
    const char *custom_id;
    const char *placeholder;
    uint32_t min_values;
    uint32_t max_values;
    uint8_t required;
    uint8_t disabled;
    union {
        struct {
            const dcc_select_option_t *options;
            size_t option_count;
        } string_select;
        struct {
            const dcc_component_v2_select_default_value_t *default_values;
            size_t default_value_count;
        } entity_select;
        struct {
            const dcc_component_v2_select_default_value_t *default_values;
            size_t default_value_count;
            const uint32_t *channel_types;
            size_t channel_type_count;
        } channel_select;
    } data;
} dcc_component_v2_select_value_t;

typedef union dcc_component_v2_layout_value {
    struct {
        const dcc_component_v2_builder_t *components;
        size_t component_count;
    } action_row;
    struct {
        const dcc_component_v2_builder_t *components;
        size_t component_count;
        const dcc_component_v2_builder_t *accessory;
    } section;
    struct {
        const dcc_component_v2_builder_t *components;
        size_t component_count;
        uint32_t accent_color;
        uint8_t spoiler;
    } container;
    struct {
        const char *label;
        const char *description;
        const dcc_component_v2_builder_t *component;
    } label;
} dcc_component_v2_layout_value_t;

typedef union dcc_component_v2_media_value {
    struct {
        dcc_component_v2_unfurled_media_t media;
        const char *description;
        uint8_t spoiler;
    } thumbnail;
    struct {
        const dcc_component_v2_media_gallery_item_t *items;
        size_t item_count;
    } gallery;
    struct {
        dcc_component_v2_unfurled_media_t file;
        uint8_t spoiler;
    } file;
} dcc_component_v2_media_value_t;

typedef struct dcc_component_v2_text_input_value {
    const char *custom_id;
    dcc_text_input_style_t style;
    const char *placeholder;
    uint32_t min_length;
    uint32_t max_length;
    const char *value;
    uint8_t required;
} dcc_component_v2_text_input_value_t;

typedef union dcc_component_v2_modal_value {
    struct {
        const char *custom_id;
        uint32_t min_values;
        uint32_t max_values;
        uint8_t required;
        const char *const *file_types;
        size_t file_type_count;
    } file_upload;
    struct {
        const char *custom_id;
        const dcc_component_v2_choice_option_t *options;
        size_t option_count;
        uint8_t required;
    } radio_group;
    struct {
        const char *custom_id;
        const dcc_component_v2_choice_option_t *options;
        size_t option_count;
        uint32_t min_values;
        uint32_t max_values;
        uint8_t required;
    } checkbox_group;
    struct {
        const char *custom_id;
        uint8_t default_value;
    } checkbox;
} dcc_component_v2_modal_value_t;

typedef struct dcc_component_v2_text_display_value {
    const char *content;
} dcc_component_v2_text_display_value_t;

typedef struct dcc_component_v2_separator_value {
    uint8_t divider;
    dcc_component_v2_separator_spacing_t spacing;
} dcc_component_v2_separator_value_t;
```

Thus the selected members are `button.target`, `select.data`,
`layout.{action_row,section,container,label}`,
`media.{thumbnail,gallery,file}`, `text_input`,
`modal.{file_upload,radio_group,checkbox_group,checkbox}`, `text_display`, and
`separator`; no implementation may substitute a layout-compatible anonymous
bag or reorder pointer/count pairs.

The top-level type is the only discriminant. Do not add a second public subtag
that can disagree with it. The nested unions only prevent storage of mutually
exclusive shapes. Presence bits define which semantic fields are active; raw
padding or inactive-union bytes are never inspected.

### Presence model

Publish `dcc_component_v2_field_t` as field indices and a static inline
`dcc_component_v2_field_mask(dcc_component_v2_field_t)` helper. Use these
exact enum constants for the version baseline:

```c
enum {
    DCC_COMPONENT_V2_BUILDER_VERSION_1 = 1,
    DCC_COMPONENT_V2_BUILDER_VERSION = DCC_COMPONENT_V2_BUILDER_VERSION_1,
    DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION_1 = 1,
    DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION =
        DCC_COMPONENT_V2_VALIDATION_ERROR_VERSION_1
};
```

Use enum constants for validation reasons/contexts and new limits as well. Do
not publish one preprocessor macro per bit.

The field-index set covers these logical fields:

```c
DCC_COMPONENT_V2_FIELD_ID
DCC_COMPONENT_V2_FIELD_COMPONENTS
DCC_COMPONENT_V2_FIELD_STYLE
DCC_COMPONENT_V2_FIELD_LABEL
DCC_COMPONENT_V2_FIELD_EMOJI
DCC_COMPONENT_V2_FIELD_CUSTOM_ID
DCC_COMPONENT_V2_FIELD_URL
DCC_COMPONENT_V2_FIELD_SKU_ID
DCC_COMPONENT_V2_FIELD_DISABLED
DCC_COMPONENT_V2_FIELD_OPTIONS
DCC_COMPONENT_V2_FIELD_PLACEHOLDER
DCC_COMPONENT_V2_FIELD_DEFAULT_VALUES
DCC_COMPONENT_V2_FIELD_CHANNEL_TYPES
DCC_COMPONENT_V2_FIELD_MIN_VALUES
DCC_COMPONENT_V2_FIELD_MAX_VALUES
DCC_COMPONENT_V2_FIELD_REQUIRED
DCC_COMPONENT_V2_FIELD_MIN_LENGTH
DCC_COMPONENT_V2_FIELD_MAX_LENGTH
DCC_COMPONENT_V2_FIELD_VALUE
DCC_COMPONENT_V2_FIELD_ACCESSORY
DCC_COMPONENT_V2_FIELD_CONTENT
DCC_COMPONENT_V2_FIELD_MEDIA
DCC_COMPONENT_V2_FIELD_DESCRIPTION
DCC_COMPONENT_V2_FIELD_SPOILER
DCC_COMPONENT_V2_FIELD_ITEMS
DCC_COMPONENT_V2_FIELD_DIVIDER
DCC_COMPONENT_V2_FIELD_SPACING
DCC_COMPONENT_V2_FIELD_ACCENT_COLOR
DCC_COMPONENT_V2_FIELD_COMPONENT
DCC_COMPONENT_V2_FIELD_FILE_TYPES
DCC_COMPONENT_V2_FIELD_DEFAULT
DCC_COMPONENT_V2_FIELD_COUNT
```

The indices are contiguous from zero. The mask helper returns
`UINT64_C(1) << field` for a valid field below `FIELD_COUNT` and zero for an
invalid field, without undefined shifts.

One pointer/count pair is one logical presence bit. A bit is valid only when
the caller-declared size covers the complete logical field group for the active
type. Presence has these exact semantics:

- an unknown bit is an error at `.present`;
- a known bit not allowed for the active type is an error at that JSON field;
- a set bit naming a field not fully covered by `size` is an error at that
  field;
- an uncovered optional suffix with no bit set behaves as absent;
- an absent or uncovered field required by the active type is an error at that
  field; and
- serializers never infer presence from a pointer, scalar value, or inactive
  union bytes.

Keep per-type allowed and required masks in one private descriptor table. The
same table must provide each field's covered byte range and JSON field name so
setter checks, validation, path reporting, build, and measure cannot drift.

Freeze the nonconditional mask inventory exactly as follows; `ID` is allowed
for every type and omitted from the table only for readability:

| Type | Other allowed fields | Unconditionally required |
| --- | --- | --- |
| ACTION_ROW | COMPONENTS | COMPONENTS |
| BUTTON | STYLE, LABEL, EMOJI, CUSTOM_ID, URL, SKU_ID, DISABLED | STYLE |
| STRING_SELECT | CUSTOM_ID, OPTIONS, PLACEHOLDER, MIN_VALUES, MAX_VALUES, REQUIRED, DISABLED | CUSTOM_ID, OPTIONS |
| USER_SELECT / ROLE_SELECT / MENTIONABLE_SELECT | CUSTOM_ID, PLACEHOLDER, DEFAULT_VALUES, MIN_VALUES, MAX_VALUES, REQUIRED, DISABLED | CUSTOM_ID |
| CHANNEL_SELECT | CUSTOM_ID, PLACEHOLDER, DEFAULT_VALUES, CHANNEL_TYPES, MIN_VALUES, MAX_VALUES, REQUIRED, DISABLED | CUSTOM_ID |
| TEXT_INPUT | CUSTOM_ID, STYLE, PLACEHOLDER, MIN_LENGTH, MAX_LENGTH, VALUE, REQUIRED | CUSTOM_ID, STYLE |
| SECTION | COMPONENTS, ACCESSORY | COMPONENTS, ACCESSORY |
| TEXT_DISPLAY | CONTENT | CONTENT |
| THUMBNAIL | MEDIA, DESCRIPTION, SPOILER | MEDIA |
| MEDIA_GALLERY | ITEMS | ITEMS |
| FILE | MEDIA, SPOILER | MEDIA |
| SEPARATOR | DIVIDER, SPACING | none |
| CONTAINER | COMPONENTS, ACCENT_COLOR, SPOILER | COMPONENTS |
| LABEL | LABEL, DESCRIPTION, COMPONENT | LABEL, COMPONENT |
| FILE_UPLOAD | CUSTOM_ID, MIN_VALUES, MAX_VALUES, REQUIRED, FILE_TYPES | CUSTOM_ID |
| RADIO_GROUP | CUSTOM_ID, OPTIONS, REQUIRED | CUSTOM_ID, OPTIONS |
| CHECKBOX_GROUP | CUSTOM_ID, OPTIONS, MIN_VALUES, MAX_VALUES, REQUIRED | CUSTOM_ID, OPTIONS |
| CHECKBOX | CUSTOM_ID, DEFAULT | CUSTOM_ID |

The conditional Button target/content rules and context-only REQUIRED/DISABLED
rules remain predicates layered on this exact table; they never change the
field byte range. The complete Button style matrix is:

| Style | Required | Forbidden | Visible content |
| --- | --- | --- | --- |
| PRIMARY/SECONDARY/SUCCESS/DANGER (1–4) | CUSTOM_ID | URL, SKU_ID | at least one of LABEL or EMOJI |
| LINK (5) | URL | CUSTOM_ID, SKU_ID | at least one of LABEL or EMOJI |
| PREMIUM (6) | SKU_ID | CUSTOM_ID, URL, LABEL, EMOJI | generated by Discord; neither LABEL nor EMOJI may be present |

No unknown style serializes. Required target strings are non-empty and Premium
`sku_id` is nonzero. These predicates are applied both at mutation and final
validate/measure/serialize time.

Before the first RED production compile, add
`tests/support/component_v2_expected_layout.h`. It repeats the complete public
typedef skeleton above under test-only names and defines the exact per-type
allowed/required masks plus every logical field's `offsetof`/end expression.
Compile a probe that compares production versus expected member types,
`offsetof`, `_Alignof`, and `sizeof` on each supported ILP32, LP64, and Windows
ABI. The probe also drives every mask/range through the public validator. Use
this same skeleton for the macro sentinel; do not generate expected layout from
the production header being tested.

Conditional required rules remain predicates beside that table:

- Action Row requires components;
- Button follows the complete six-style required/forbidden/content matrix
  above; there is no style-neutral fallback;
- String Select requires custom ID and options;
- User/Role/Mentionable/Channel Select requires custom ID;
- Text Input requires custom ID and style;
- Section requires components and accessory;
- Text Display requires content;
- Thumbnail and File require media;
- Media Gallery requires items;
- Container requires components;
- Label requires label and component;
- File Upload requires custom ID;
- Radio Group and Checkbox Group require custom ID and options;
- Checkbox requires custom ID; and
- Separator has no additional required field.

## Constructors, initializers, and setters

Publish one constant initializer macro:

```c
#define DCC_COMPONENT_V2_BUILDER_INIT(type_) /* complete C/C++ initializer */
```

It initializes the current caller-visible size, builder version 1, zero
presence, requested type, zero ID, and a zeroed union. All canonical value
constructors are `static inline`, return `dcc_component_v2_builder_t` by value,
perform no allocation, set the active kind, and set every required presence
bit supplied by that constructor. Use these signatures:

```c
dcc_component_v2_builder_t dcc_component_v2_text_display(const char *content);
dcc_component_v2_builder_t dcc_component_v2_button(
    dcc_button_style_t style, const char *label, const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_link_button(
    const char *label, const char *url);
dcc_component_v2_builder_t dcc_component_v2_premium_button(
    dcc_snowflake_t sku_id);
dcc_component_v2_builder_t dcc_component_v2_action_row(
    const dcc_component_v2_builder_t *components, size_t component_count);
dcc_component_v2_builder_t dcc_component_v2_string_select(
    const char *custom_id, const dcc_select_option_t *options,
    size_t option_count);
dcc_component_v2_builder_t dcc_component_v2_user_select(const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_role_select(const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_mentionable_select(
    const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_channel_select(
    const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_text_input(
    const char *custom_id, dcc_text_input_style_t style);
dcc_component_v2_builder_t dcc_component_v2_section(
    const dcc_component_v2_builder_t *components, size_t component_count,
    const dcc_component_v2_builder_t *accessory);
dcc_component_v2_builder_t dcc_component_v2_thumbnail(
    dcc_component_v2_unfurled_media_t media);
dcc_component_v2_builder_t dcc_component_v2_media_gallery(
    const dcc_component_v2_media_gallery_item_t *items, size_t item_count);
dcc_component_v2_builder_t dcc_component_v2_file(
    dcc_component_v2_unfurled_media_t file);
dcc_component_v2_builder_t dcc_component_v2_separator(
    uint8_t divider, dcc_component_v2_separator_spacing_t spacing);
dcc_component_v2_builder_t dcc_component_v2_container(
    const dcc_component_v2_builder_t *components, size_t component_count);
dcc_component_v2_builder_t dcc_component_v2_label(
    const char *label, const dcc_component_v2_builder_t *component);
dcc_component_v2_builder_t dcc_component_v2_file_upload(
    const char *custom_id);
dcc_component_v2_builder_t dcc_component_v2_radio_group(
    const char *custom_id, const dcc_component_v2_choice_option_t *options,
    size_t option_count);
dcc_component_v2_builder_t dcc_component_v2_checkbox_group(
    const char *custom_id, const dcc_component_v2_choice_option_t *options,
    size_t option_count);
dcc_component_v2_builder_t dcc_component_v2_checkbox(
    const char *custom_id, uint8_t default_value);
```

The declarations above omit `static inline` only to keep the signatures
readable; their definitions live in the public header. They borrow
strings/arrays until the enclosing message, modal, or direct build call
serializes them. The ordinary Button constructor is for styles 1–4; Link and
Premium use their dedicated constructors. Optional fields are added through
validated setters.

Using inline constructors is an ABI requirement, not only convenience: an old
DCC 2 caller embeds the `sizeof(dcc_component_v2_builder_t)` from its own
header, so a newer library can recognize its historical stride. A library-side
initializer must never `memset(sizeof(current_library_layout))` into an older
caller's allocation. Convert the current generic/output-parameter init family
to header inline transition wrappers or remove it at the 2.0 cut; do not retain
an exported initializer that can overrun historical caller storage. Existing
build/free symbols may remain.

The existing generic setters may remain as the explicit low-level mutation
surface during Tasks 12–13, but each setter must:

1. validate the existing prefix, supported version, known bits, active type,
   target-field coverage, and argument;
2. reject a field that is inactive for the current type;
3. perform no write before every check succeeds;
4. write only the active named-union member; and
5. set or clear the corresponding presence bit consistently.

Add the missing File Upload file-types setter. On every rejection, the complete
declared object and surrounding canaries remain byte-for-byte unchanged.

### Macro budget

The old 256-name snapshot predates the Task 7 message leaf types and the Task 9
command-registry/request include graph, so it is not a valid Task 12 baseline.
Measure the completed Task 11 tree with the exact C11 Bot-dependency sentinel
used by Task 13 and record the bytewise-sorted full macro dump. The hard project
budget is 300. Within that measured baseline, Task 12 converts the seven
retained `DCC_COMPONENT_V2_MAX_*` semantic limits to enum constants and removes
the obsolete `DCC_COMPONENT_V2_MAX_CHANNEL_TYPES` ceiling, so all eight current
limit macros leave the compiler macro dump.

The recommended design adds exactly three:

1. the `DCC_COMPONENT_TYPES_H` include guard;
2. `DCC_COMPONENT_V2_BUILDER_INIT`; and
3. `DCC_COMPONENT_V2_VALIDATION_ERROR_INIT`.

All version values, field indices, limits, contexts, and error reasons are enum
constants; the field-mask operation is static inline. Task 12's exact compiler-
macro delta is therefore minus five (`-8 + 3`). Task 13 derives its exact final
count from the checked post-Task-12 dump plus its reviewed Bot-owned inventory;
neither task hard-codes the stale 251/283 arithmetic. Do not introduce any new
`DCC_V2_*`, `DCC_UI_*`, or `DCC_ROUTE_*` spelling in this task.

## Size-gated reads and recursive array stride

Reuse or extend the private Task 5 ABI helpers. Validation must first read only
`size`, then only covered prefix fields. Never form a current-layout struct
lvalue over caller bytes and never copy `sizeof(current)` from the caller.

The preferred per-node flow is:

1. validate the common prefix and declared size;
2. validate builder version 1 and known presence bits;
3. validate type before selecting a union member;
4. zero a current-layout local normalized value;
5. copy only `min(declared_size, sizeof(current))` bytes with `memcpy`, or read
   each field with covered `memcpy` helpers; and
6. use only fields allowed, covered, and present for the selected type.

Subtraction-based field coverage checks are required after confirming the
offset is within size, so `offset + width` cannot overflow.

Every array of versioned components uses this algorithm:

1. zero count accepts a null pointer and has no stride;
2. nonzero count requires a nonnull, suitably aligned first element;
3. validate the first prefix and use its declared `size` as byte stride;
4. reject a stride below the common prefix, alignment mismatch, multiplication
   overflow, pointer-span overflow, or unsupported version;
5. locate later values by checked byte arithmetic, not `array[i]`;
6. require every element to declare the same stride and version; and
7. validate/read every element through covered-field helpers.

This applies independently to:

- a top-level component array;
- each Action Row child array;
- each Section text-child array;
- each Container child array;
- a Section accessory singleton; and
- a Label component singleton.

A nested array does not inherit its parent's stride. Fixed leaf arrays such as
select options, defaults, media-gallery items, file-type strings, channel
types, and group choices continue to use their fixed element size but still
need checked span arithmetic.

## Structured validation error

Do not use the App command-validation error type and do not store an error path
in thread-local state. Add a Components v2-specific, size/versioned public
error record with caller-owned path storage:

```c
typedef struct dcc_component_v2_validation_error {
    size_t size;
    uint32_t version;
    dcc_status_t status;
    dcc_component_v2_validation_reason_t reason;
    char field_path[512];
} dcc_component_v2_validation_error_t;
```

Publish these exact public context and reason constants:

```c
typedef enum dcc_component_v2_context {
    DCC_COMPONENT_V2_CONTEXT_ANY = 0,
    DCC_COMPONENT_V2_CONTEXT_MESSAGE_LEGACY,
    DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2,
    DCC_COMPONENT_V2_CONTEXT_MODAL
} dcc_component_v2_context_t;

typedef enum dcc_component_v2_validation_reason {
    DCC_COMPONENT_V2_VALIDATION_NONE = 0,
    DCC_COMPONENT_V2_VALIDATION_ABI_SIZE,
    DCC_COMPONENT_V2_VALIDATION_ABI_VERSION,
    DCC_COMPONENT_V2_VALIDATION_PRESENCE,
    DCC_COMPONENT_V2_VALIDATION_TYPE,
    DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_ALLOWED,
    DCC_COMPONENT_V2_VALIDATION_FIELD_NOT_COVERED,
    DCC_COMPONENT_V2_VALIDATION_REQUIRED_FIELD,
    DCC_COMPONENT_V2_VALIDATION_INVALID_VALUE,
    DCC_COMPONENT_V2_VALIDATION_LIMIT,
    DCC_COMPONENT_V2_VALIDATION_DUPLICATE,
    DCC_COMPONENT_V2_VALIDATION_PLACEMENT,
    DCC_COMPONENT_V2_VALIDATION_OVERFLOW
} dcc_component_v2_validation_reason_t;
```

Publish `DCC_COMPONENT_V2_VALIDATION_ERROR_INIT` and one public validator:

```c
dcc_status_t dcc_component_v2_validate(
    const dcc_component_v2_builder_t *components,
    size_t component_count,
    dcc_component_v2_context_t context,
    dcc_component_v2_validation_error_t *out_error
);
```

The context enum includes structural ANY, normal-message MESSAGE_LEGACY,
layout-message MESSAGE_V2, and MODAL. `ANY` validates only type-local shape and
nesting safety; it is not sufficient before message serialization. The normal
message context accepts Action Row roots containing buttons or exactly one
select, permits at most five top-level Action Rows, and rejects component types
9 and above. The V2 context accepts the documented Components v2 root grammar.
Keeping these modes in the shared
traversal prevents message setters from implementing a second, divergent
validator. `out_error == NULL` is valid and preserves status-only callers. On
failure, an initialized and sufficiently large error record receives the same
status plus a stable reason and a NUL-terminated JSON-oriented path.
Representative exact paths are:

```text
components[0].size
components[0].version
components[0].present
components[0].components[2].custom_id
components[1].component.file_types[3]
```

If the error record is itself invalid, return `DCC_ERR_INVALID_ARG` without
writing past its declared size. Traversal is depth-first and left-to-right:
validate a node's ABI, tag, and own fields before its children, then report the
first failing child leaf without replacing it with a generic parent failure.
Existing status-only build functions call the same validator with no error
output. If a public error-aware build entry point is added, add one canonical
function and keep the old build function as a thin wrapper; do not duplicate
validation.

## Context grammar and Discord corrections

The implementation contract is the current official
[Discord Component Reference](https://docs.discord.com/developers/components/reference)
and
[Interaction callback/modal structure](https://docs.discord.com/developers/interactions/receiving-and-responding).
Retain the existing 20 supported component kinds and total 40-component message
limit, while making these corrections:

- Action Row contains up to five Buttons or exactly one Select. Deprecated
  modal Action Row/Text Input input is deliberately rejected by the DCC 2
  canonical surface.
- A normal `MESSAGE_LEGACY` message has at most five top-level Action Rows;
  Components-v2 message roots follow their separate 40-total grammar.
- Section contains one to three Text Displays and exactly one Button or
  Thumbnail accessory.
- Container children are limited to Action Row, Text Display, Section, Media
  Gallery, Separator, and File.
- Message roots are Action Row, Section, Text Display, Media Gallery, File,
  Separator, or Container.
- Modal roots number one through five and may be Label or Text Display. The
  current validator incorrectly permits only Label.
- Label contains exactly one Text Input, String/User/Role/Mentionable/Channel
  Select, File Upload, Radio Group, Checkbox Group, or Checkbox.
- Text Input has no request-level label in the tagged variant; that deprecated
  field belongs to the surrounding Label.
- Checkbox has only custom ID and optional default. It has no label or required
  member; presentation text belongs to the surrounding Label.
- Thumbnail owns `description` and `spoiler` outside its media object.
- Media Gallery items own their optional description and spoiler.
- File owns only `file` plus optional spoiler, accepts exactly one
  `attachment://<filename>` URL, and never serializes description.
- File Upload adds `file_types`, accepts at most ten entries, and accepts only
  `image`, `video`, `audio`, or a valid dot-prefixed extension.
- Radio Group and Checkbox Group options have label, value, optional
  description/default, but no emoji. Radio Group has 2–10 choices and at most
  one default. Checkbox Group has 1–10 choices.
- String Select has at most 25 options and has no `default_values` field.
  Entity-select default counts must fall within effective min/max values. The
  exact default object matrix is USER_SELECT=user only, ROLE_SELECT=role only,
  CHANNEL_SELECT=channel only, and MENTIONABLE_SELECT=user or role; every other
  kind/value combination is rejected.
- Select/Checkbox Group/File Upload min/max values honor default values,
  `min <= max`, option-count bounds where applicable, and the rule that an
  omitted/true `required` cannot combine with explicit zero minimum.
- `required` is modal-only and `disabled` is message-only for selects. A field
  valid only in the other context is rejected, not ignored.
- Boolean request values must be exactly zero or one.
- Accent color is in `0x000000..0xFFFFFF` when present.
- Component IDs and custom IDs remain unique across the full tree. Explicit ID
  zero is treated as absent by Discord; duplicate nonzero IDs fail.
- Remove `DCC_COMPONENT_V2_MAX_CHANNEL_TYPES == 16` as a semantic Discord
  limit. The official schema specifies a list of channel types but no count-16
  ceiling. Validate pointer/span and each supported channel type instead.

No invalid request may produce partial or ambiguous JSON. Public build failure
leaves `*out_json == NULL`; measure failure leaves `*out_json_len == 0`.

## One validate/build/measure traversal

Task 6 established allocation-free count mode and exact multipart preflight.
Preserve it. Refactor Components v2 around one private, ABI-aware traversal
that owns:

- prefix, presence, and stride reads;
- active-type and context validation;
- normal-message versus Components-v2 root grammar, including buttons/selects
  under Action Rows, rejection of types 9+, and rejection of mixed modes;
- recursive path construction;
- total/duplicate-ID state;
- fixed-leaf validation; and
- dispatch to a JSON sink.

The JSON sink has build and count-only modes. Both modes call the same node and
array encoders; count mode performs the same trailing-NUL overflow checks but
does not reserve, copy, or allocate. Do not implement a separate estimator.

The message builder, modal builder, and REST endpoint preflight call this same
context validator/encoder. Invalid nested input must fail before JSON buffer,
multipart buffer, endpoint request, admission, callback, observer, or network
activity. The measured length must equal the subsequent built length for every
valid tree.

Replace every direct `builders[i]`, `children[i]`, and
`sizeof(*builder)`-stride traversal in component validation, context-layout
validation, serialization, message/modal integration, and tests with the
checked array-view helper. Task 6's committed count-only functionality is an
input to this work and must not be reverted.

## Files and migration sequence

The original plan's `src/component_v2/` path is stale. Production Components
v2 code lives under `src/objects/`. Use this actual file map:

- Create: `include/dcc/component_types.h`
- Modify: `include/dcc/component.h`
- Rewrite: `include/dcc/component_v2.h`
- Create: `src/objects/component_v2_value.c` if non-inline transition wrappers
  remain; otherwise replace/remove `component_v2_convenience.c` in CMake
- Modify: `src/objects/component_v2_builder.c`
- Modify: `src/objects/component_v2_validate.c`
- Modify: `src/objects/component_v2_serialize.c`
- Modify: `src/objects/component_v2_build.c`
- Modify: `src/internal/objects/dcc_component_v2_internal.h`
- Extend as needed: `src/internal/objects/dcc_builder_abi_internal.h` and
  `src/objects/builder_abi.c`
- Modify: `src/objects/message_builder_serialize_validate.c`
- Modify: `src/objects/message_builder_serialize_rich.c`
- Modify: `src/objects/modal_builder_serialize.c`
- Modify only where contract tests require it: Task 6 REST multipart/preflight
  support that consumes exact component measurement
- Modify: `include/dcc/sugar/message_component_ui/v2_core.h`
- Modify: `include/dcc/sugar/message_component_ui/v2_selects.h`
- Modify: `include/dcc/sugar/message_component_ui/v2_inputs.h`
- Modify: `include/dcc/sugar/message_component_ui/v2_media.h`
- Modify: `include/dcc/sugar/message_component_ui/ui_aliases.h`
- Modify component-session, focused Sugar, package-consumer, examples, and
  direct designated-initializer callers discovered by `rg`
- Create: `tests/support/component_v2_expected_layout.h`
- Create: `tests/component_v2_abi_smoke.c`
- Modify: `tests/component_v2_smoke.c`
- Modify: `CMakeLists.txt`

Stage the migration in this order so each commit can remain buildable:

1. add the RED ABI fixture and exact expected failure evidence;
2. add the shared type header and tagged public layout/value constructors;
3. add private prefix/stride normalization and migrate setters;
4. migrate validator plus structured paths and make the RED fixture green;
5. migrate the shared build/measure encoder;
6. migrate message/modal/Task 6 consumers;
7. re-expand existing `DCC_V2_*` and `DCC_UI_*` transition macros against the
   new layout without adding spellings;
8. migrate remaining direct initializers and package symbol tests; and
9. run the complete verification matrix.

Tasks 12–13 keep existing transition macros buildable. Task 13 makes the
canonical `DCC_UI_*` vocabulary consume these value constructors. Task 14 then
deletes `DCC_V2_*`, legacy component Sugar, and old transition wrappers. Do not
preempt Task 14 by leaving intermediate repository targets uncompilable.

## Required RED matrix

Create and register `dcc_component_v2_abi_smoke`. Capture RED before changing
the production layout. The RED must demonstrate at least that the flat builder
lacks size/version/presence validation, accepts an inactive cross-kind field,
and walks an array at the current `sizeof` stride.

The final fixture and focused existing tests must cover all of the following.

### Prefix and presence

- initializer macro and every value constructor publish exact current size,
  version 1, active type, and required presence bits;
- zero size, short common prefix, unsupported version, unknown bits, known
  inactive bits, and present-but-uncovered fields;
- an exact historical Text Display allocation with poisoned suffix and
  external canaries;
- uncovered absent optional suffix succeeds without reading poison;
- uncovered/absent required suffix fails with its named path;
- an oversized future suffix with no unknown bits is ignored;
- every setter has a current-layout success and representative historical
  covered/uncovered failure with whole-object no-mutation evidence.

### Arrays and arithmetic

- two distinct historical-size top-level values serialize with their declared
  old stride;
- inconsistent stride/version, undersized stride, misalignment,
  `count * stride` overflow, and pointer-span overflow fail deterministically;
- Action Row, Section, and Container nested arrays each use their own first
  element's stride;
- Section accessory and Label component validate their own independent
  prefixes; and
- fixed leaf pointer/count arrays reject null/nonzero, overflow, and invalid
  elements without undefined behavior in the fixture.

### Tag/field matrix

- exact positive JSON for all 20 kinds;
- a table-driven check of every known field bit against every active type;
- explicit Button/Select/Layout/Media/Text Input/Modal cross-kind mixes;
- every allowed and forbidden cell of the ordinary/link/premium Button matrix,
  including Link label-or-emoji and Premium rejection of label and emoji;
- String Select defaults/channel types, auto-select options, and non-Channel
  Select channel types;
- File description, duplicate spoiler source, Text Input label, Checkbox
  label/required, and group-option emoji are impossible or rejected; and
- every failure leaves JSON null/length zero and reports the exact field path.

### Discord semantics and limits

- required and conditional Button target/content fields plus every
  entity-select/default-value kind pair (including both Mentionable positives
  and String Select rejection);
- min/max defaults, `min <= max`, selection count versus option count, radio
  single-default, boolean canonicalization, and duplicate ID/custom ID;
- total components 40/41;
- legacy-message top-level Action Rows 5/6;
- Action Row children 5/6 and one-select/two-select;
- Section children 1, 3/4 and accessory types;
- Media Gallery items 1, 10/11;
- String Select options 1, 25/26;
- select defaults 0, 25/26 and effective range;
- File Upload values 0/1 and 10/11 plus file types 10/11;
- Radio Group choices 2, 10/11;
- Checkbox Group choices 1, 10/11;
- modal roots 1, 5/6 and Label/Text Display mixture;
- every documented string bound at limit and limit+1;
- accent `0xFFFFFF` and `0x1000000`; and
- valid `attachment://name` plus invalid external/File-empty schemes.

### Context and integration

- the complete message-root, Container-child, Action Row-child, Section-child,
  Section-accessory, modal-root, and Label-child placement tables;
- select `required`/`disabled` context differences;
- existing Components v2 message flag plus content/embed/poll/sticker
  incompatibility;
- modal build and ordered-interaction serialization;
- build length equals allocation-free measured length for representative deep
  message and modal trees;
- representative Task 6 endpoint preflight performs no allocation/admission/
  callback/observer/network action for invalid nested components;
- current `DCC_V2_*` and `DCC_UI_*` transition expansion;
- component-session usage and installed package consumer symbol/type coverage;
  and
- existing parser/accessor behavior remains unchanged.

Use aligned byte fixtures and `memcpy`-based setup. The tests themselves must
not create undefined behavior through misaligned typed dereferences or invalid
pointer arithmetic. Poison and canary assertions must be deterministic under
normal, ASan/UBSan, and MinGW builds.

## Verification and handoff

Run at minimum:

- new `dcc_component_v2_abi_smoke`;
- existing `dcc_component_v2_smoke` and `dcc_builder_v2_abi_smoke`;
- message, modal, interaction-flow, and ordered-interaction focused targets;
- `dcc_rest_v2_endpoint_smoke`, including exact Task 6 measure/preflight
  contracts;
- `dcc_component_session_smoke`;
- `dcc_sugar_smoke` and focused Sugar transition targets;
- package-consumer configure/build/run against a fresh installation;
- standalone C11 and C++17 compile probes for `component_types.h`,
  `component.h`, and `component_v2.h`;
- native plus i686/x86_64 MinGW header/layout/source probes;
- the V2 transition/public-surface/source-layout/public-API audits;
- the exact preprocessed Bot-dependency macro audit, proving the reviewed three
  additions, eight removals, net `-5`, and a bytewise-sorted post-Task-12 dump;
- focused ASan/UBSan and TSan where the endpoint/message ownership path is
  concurrent;
- `mkdocs --strict`; and
- the full enabled CTest suite after focused verification is green.

Run `git diff --check` before every implementation handoff. Commit the
implementation/tests with `feat: tag and version Components v2`. Write a
separate `task-12-implementer-report.md` beside this brief containing the RED
evidence, exact public layout and minimum prefix, presence/allowed-mask table,
historical stride cases, public macro count before/after, official-schema
corrections, changed files, verification commands/results, sanitizer/platform
evidence, known pre-existing failures, and the implementation commit hash.
Commit that report separately and stop for review. Do not start Task 13.
