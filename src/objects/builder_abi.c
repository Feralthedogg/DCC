#include "internal/objects/dcc_builder_abi_internal.h"

#include <string.h>

typedef struct dcc_builder_abi_prefix {
    size_t size;
    uint32_t version;
    uint64_t present;
} dcc_builder_abi_prefix_t;

#define DCC_BUILDER_KNOWN_MESSAGE \
    (DCC_MESSAGE_BUILDER_PRESENT_CONTENT | DCC_MESSAGE_BUILDER_PRESENT_NONCE | \
     DCC_MESSAGE_BUILDER_PRESENT_TTS | DCC_MESSAGE_BUILDER_PRESENT_FLAGS | \
     DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS | DCC_MESSAGE_BUILDER_PRESENT_EMBEDS | \
     DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS | DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2 | \
     DCC_MESSAGE_BUILDER_PRESENT_POLL | DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS | \
     DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON | DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON | \
     DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE | \
     DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS)

#define DCC_MESSAGE_REFERENCE_KNOWN \
    (DCC_MESSAGE_REFERENCE_PRESENT_TYPE | \
     DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID | \
     DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID | \
     DCC_MESSAGE_REFERENCE_PRESENT_GUILD_ID | \
     DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS)

#define DCC_MESSAGE_ATTACHMENT_KNOWN \
    (DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME | \
     DCC_MESSAGE_ATTACHMENT_PRESENT_TITLE | \
     DCC_MESSAGE_ATTACHMENT_PRESENT_DESCRIPTION | \
     DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS | \
     DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM | \
     DCC_MESSAGE_ATTACHMENT_PRESENT_IS_SPOILER)

#define DCC_BUILDER_KNOWN_POLL \
    (DCC_POLL_BUILDER_PRESENT_QUESTION | DCC_POLL_BUILDER_PRESENT_ANSWERS | \
     DCC_POLL_BUILDER_PRESENT_DURATION_HOURS | DCC_POLL_BUILDER_PRESENT_LAYOUT_TYPE | \
     DCC_POLL_BUILDER_PRESENT_ALLOW_MULTISELECT)

#define DCC_BUILDER_KNOWN_MENTIONS \
    (DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS | \
     DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES | \
     DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS | \
     DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES | \
     DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE | \
     DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER)

#define DCC_BUILDER_KNOWN_EMBED \
    (DCC_EMBED_BUILDER_PRESENT_TITLE | DCC_EMBED_BUILDER_PRESENT_TYPE | \
     DCC_EMBED_BUILDER_PRESENT_DESCRIPTION | DCC_EMBED_BUILDER_PRESENT_URL | \
     DCC_EMBED_BUILDER_PRESENT_TIMESTAMP | DCC_EMBED_BUILDER_PRESENT_FOOTER | \
     DCC_EMBED_BUILDER_PRESENT_IMAGE | DCC_EMBED_BUILDER_PRESENT_THUMBNAIL | \
     DCC_EMBED_BUILDER_PRESENT_AUTHOR | DCC_EMBED_BUILDER_PRESENT_FIELDS | \
     DCC_EMBED_BUILDER_PRESENT_COLOR)

#define DCC_BUILDER_KNOWN_MODAL \
    (DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID | DCC_MODAL_BUILDER_PRESENT_TITLE | \
     DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON | DCC_MODAL_BUILDER_PRESENT_COMPONENTS | \
     DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2 | DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON)

#define DCC_BUILDER_KNOWN_CHOICE \
    (DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME | \
     DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON | \
     DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE)

#define DCC_BUILDER_KNOWN_OPTION \
    (DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS_JSON | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED | \
     DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE)

#define DCC_BUILDER_KNOWN_COMMAND \
    (DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION | \
     DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW)

int dcc_builder_abi_field_covered(size_t size, size_t offset, size_t width) {
    return size >= offset && width <= size - offset;
}

static int dcc_builder_abi_field_partially_covered(
    size_t size,
    size_t offset,
    size_t width
) {
    return size > offset && size - offset < width;
}

int dcc_builder_abi_view_has(const dcc_builder_abi_view_t *view, uint64_t bit) {
    return view != NULL && (view->present & bit) != 0U;
}

static dcc_status_t dcc_builder_abi_read(
    const void *value,
    uint32_t expected_version,
    uint64_t known_bits,
    dcc_builder_abi_view_t *out
) {
    if (value == NULL || out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    memset(out, 0, sizeof(*out));
    memcpy(&out->size, value, sizeof(out->size));
    if (!dcc_builder_abi_field_covered(
            out->size,
            offsetof(dcc_builder_abi_prefix_t, version),
            sizeof(out->version)
        ) ||
        !dcc_builder_abi_field_covered(
            out->size,
            offsetof(dcc_builder_abi_prefix_t, present),
            sizeof(out->present)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    memcpy(
        &out->version,
        (const unsigned char *)value + offsetof(dcc_builder_abi_prefix_t, version),
        sizeof(out->version)
    );
    memcpy(
        &out->present,
        (const unsigned char *)value + offsetof(dcc_builder_abi_prefix_t, present),
        sizeof(out->present)
    );
    if (out->version != expected_version || (out->present & ~known_bits) != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}

static int dcc_builder_bit_field(
    const dcc_builder_abi_view_t *view,
    uint64_t bit,
    size_t offset,
    size_t width
) {
    return !dcc_builder_abi_view_has(view, bit) ||
        dcc_builder_abi_field_covered(view->size, offset, width);
}

#define DCC_BIT_FIELD(type_, view_, bit_, field_) \
    dcc_builder_bit_field( \
        (view_), (bit_), offsetof(type_, field_), sizeof(((type_ *)0)->field_) \
    )

#define DCC_BIT_PAIR(type_, view_, bit_, first_, second_) \
    (DCC_BIT_FIELD(type_, view_, bit_, first_) && DCC_BIT_FIELD(type_, view_, bit_, second_))

#define DCC_PARTIAL_FIELD(type_, view_, field_) \
    dcc_builder_abi_field_partially_covered( \
        (view_)->size, offsetof(type_, field_), sizeof(((type_ *)0)->field_) \
    )

dcc_status_t dcc_builder_abi_array_begin(
    const void *values,
    size_t count,
    size_t alignment,
    size_t minimum_size,
    size_t expected_version,
    uint64_t known_bits,
    size_t *out_stride
) {
    if (out_stride == NULL || alignment == 0U || minimum_size == 0U ||
        (count != 0U && values == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_stride = 0U;
    if (count == 0U) {
        return DCC_OK;
    }
    uintptr_t address = (uintptr_t)values;
    if (address % alignment != 0U) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_builder_abi_view_t view;
    dcc_status_t status = dcc_builder_abi_read(values, expected_version, known_bits, &view);
    if (status != DCC_OK || view.size < minimum_size ||
        (count > 1U && view.size % alignment != 0U) ||
        count > SIZE_MAX / view.size) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t span = count * view.size;
    if (address > UINTPTR_MAX - (span - 1U)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_stride = view.size;
    return DCC_OK;
}

const void *dcc_builder_abi_array_at(const void *values, size_t stride, size_t index) {
    return (const unsigned char *)values + stride * index;
}

dcc_status_t dcc_message_builder_abi_validate(
    const dcc_message_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_MESSAGE_BUILDER_VERSION, DCC_BUILDER_KNOWN_MESSAGE, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_CONTENT, content) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_NONCE, nonce) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_TTS, tts) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_FLAGS, flags) &&
            DCC_BIT_PAIR(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS, sticker_ids, sticker_ids_count) &&
            DCC_BIT_PAIR(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_EMBEDS, embeds, embeds_count) &&
            DCC_BIT_PAIR(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS, components, components_count) &&
            DCC_BIT_PAIR(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2, components_v2, components_v2_count) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_POLL, poll) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS, allowed_mentions) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON, message_reference_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON, allowed_mentions_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON, embeds_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON, components_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON, attachments_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON, poll_json) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON, components_v2_json) &&
            !DCC_PARTIAL_FIELD(dcc_message_builder_t, out, message_reference) &&
            !DCC_PARTIAL_FIELD(dcc_message_builder_t, out, attachments) &&
            !DCC_PARTIAL_FIELD(dcc_message_builder_t, out, attachment_count) &&
            DCC_BIT_FIELD(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE, message_reference) &&
            DCC_BIT_PAIR(dcc_message_builder_t, out, DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS, attachments, attachment_count)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_message_reference_abi_validate(
    const dcc_message_reference_t *reference,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        reference, DCC_MESSAGE_REFERENCE_VERSION,
        DCC_MESSAGE_REFERENCE_KNOWN, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return !DCC_PARTIAL_FIELD(dcc_message_reference_t, out, type) &&
            !DCC_PARTIAL_FIELD(dcc_message_reference_t, out, message_id) &&
            !DCC_PARTIAL_FIELD(dcc_message_reference_t, out, channel_id) &&
            !DCC_PARTIAL_FIELD(dcc_message_reference_t, out, guild_id) &&
            !DCC_PARTIAL_FIELD(dcc_message_reference_t, out, fail_if_not_exists) &&
            DCC_BIT_FIELD(dcc_message_reference_t, out,
                DCC_MESSAGE_REFERENCE_PRESENT_TYPE, type) &&
            DCC_BIT_FIELD(dcc_message_reference_t, out,
                DCC_MESSAGE_REFERENCE_PRESENT_MESSAGE_ID, message_id) &&
            DCC_BIT_FIELD(dcc_message_reference_t, out,
                DCC_MESSAGE_REFERENCE_PRESENT_CHANNEL_ID, channel_id) &&
            DCC_BIT_FIELD(dcc_message_reference_t, out,
                DCC_MESSAGE_REFERENCE_PRESENT_GUILD_ID, guild_id) &&
            DCC_BIT_FIELD(dcc_message_reference_t, out,
                DCC_MESSAGE_REFERENCE_PRESENT_FAIL_IF_NOT_EXISTS,
                fail_if_not_exists)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_message_attachment_abi_validate(
    const dcc_message_attachment_t *attachment,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        attachment, DCC_MESSAGE_ATTACHMENT_VERSION,
        DCC_MESSAGE_ATTACHMENT_KNOWN, out
    );
    if (status != DCC_OK ||
        !dcc_builder_abi_field_covered(
            out->size, offsetof(dcc_message_attachment_t, id),
            sizeof(attachment->id)
        )) {
        return DCC_ERR_INVALID_ARG;
    }
    return !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, filename) &&
            !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, title) &&
            !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, description) &&
            !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, duration_secs) &&
            !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, waveform) &&
            !DCC_PARTIAL_FIELD(dcc_message_attachment_t, out, is_spoiler) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_FILENAME, filename) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_TITLE, title) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_DESCRIPTION, description) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_DURATION_SECS, duration_secs) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_WAVEFORM, waveform) &&
            DCC_BIT_FIELD(dcc_message_attachment_t, out,
                DCC_MESSAGE_ATTACHMENT_PRESENT_IS_SPOILER, is_spoiler)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_message_attachment_array_begin(
    const dcc_message_attachment_t *values,
    size_t count,
    size_t *out_stride
) {
    return dcc_builder_abi_array_begin(
        values, count, _Alignof(dcc_message_attachment_t),
        offsetof(dcc_message_attachment_t, id) +
            sizeof(((dcc_message_attachment_t *)0)->id),
        DCC_MESSAGE_ATTACHMENT_VERSION, DCC_MESSAGE_ATTACHMENT_KNOWN,
        out_stride
    );
}

dcc_status_t dcc_poll_builder_abi_validate(
    const dcc_poll_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_POLL_BUILDER_VERSION, DCC_BUILDER_KNOWN_POLL, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_poll_builder_t, out, DCC_POLL_BUILDER_PRESENT_QUESTION, question) &&
            DCC_BIT_PAIR(dcc_poll_builder_t, out, DCC_POLL_BUILDER_PRESENT_ANSWERS, answers, answer_count) &&
            DCC_BIT_FIELD(dcc_poll_builder_t, out, DCC_POLL_BUILDER_PRESENT_DURATION_HOURS, duration_hours) &&
            DCC_BIT_FIELD(dcc_poll_builder_t, out, DCC_POLL_BUILDER_PRESENT_LAYOUT_TYPE, layout_type) &&
            DCC_BIT_FIELD(dcc_poll_builder_t, out, DCC_POLL_BUILDER_PRESENT_ALLOW_MULTISELECT, allow_multiselect)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_allowed_mentions_builder_abi_validate(
    const dcc_allowed_mentions_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_ALLOWED_MENTIONS_BUILDER_VERSION, DCC_BUILDER_KNOWN_MENTIONS, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_PAIR(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_USERS, users, user_count) &&
            DCC_BIT_PAIR(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_ROLES, roles, role_count) &&
            DCC_BIT_FIELD(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_USERS, parse_users) &&
            DCC_BIT_FIELD(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_ROLES, parse_roles) &&
            DCC_BIT_FIELD(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_PARSE_EVERYONE, parse_everyone) &&
            DCC_BIT_FIELD(dcc_allowed_mentions_builder_t, out, DCC_ALLOWED_MENTIONS_BUILDER_PRESENT_REPLIED_USER, replied_user)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_embed_builder_abi_validate(
    const dcc_embed_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_EMBED_BUILDER_VERSION, DCC_BUILDER_KNOWN_EMBED, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_TITLE, title) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_TYPE, type) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_DESCRIPTION, description) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_URL, url) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_TIMESTAMP, timestamp) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_FOOTER, footer) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_IMAGE, image) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_THUMBNAIL, thumbnail) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_AUTHOR, author) &&
            DCC_BIT_PAIR(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_FIELDS, fields, field_count) &&
            DCC_BIT_FIELD(dcc_embed_builder_t, out, DCC_EMBED_BUILDER_PRESENT_COLOR, color)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_modal_builder_abi_validate(
    const dcc_modal_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_MODAL_BUILDER_VERSION, DCC_BUILDER_KNOWN_MODAL, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_CUSTOM_ID, custom_id) &&
            DCC_BIT_FIELD(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_TITLE, title) &&
            DCC_BIT_FIELD(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_JSON, components_json) &&
            DCC_BIT_PAIR(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_COMPONENTS, components, components_count) &&
            DCC_BIT_PAIR(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2, components_v2, components_v2_count) &&
            DCC_BIT_FIELD(dcc_modal_builder_t, out, DCC_MODAL_BUILDER_PRESENT_COMPONENTS_V2_JSON, components_v2_json)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_autocomplete_choice_abi_validate(
    const dcc_autocomplete_choice_t *choice,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        choice, DCC_AUTOCOMPLETE_CHOICE_VERSION, DCC_BUILDER_KNOWN_CHOICE, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME, name) &&
            DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_NAME_LOCALIZATIONS_JSON, name_localizations_json) &&
            DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE, value_string) &&
            DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE, value_integer) &&
            DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE, value_number) &&
            DCC_BIT_FIELD(dcc_autocomplete_choice_t, out, DCC_AUTOCOMPLETE_CHOICE_PRESENT_VALUE, value_type)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_autocomplete_builder_abi_validate(
    const dcc_autocomplete_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_AUTOCOMPLETE_BUILDER_VERSION,
        DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_PAIR(dcc_autocomplete_builder_t, out, DCC_AUTOCOMPLETE_BUILDER_PRESENT_CHOICES, choices, choices_count)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_application_command_option_builder_abi_validate(
    const dcc_application_command_option_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION, DCC_BUILDER_KNOWN_OPTION, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME, name) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION, description) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON, name_localizations_json) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON, description_localizations_json) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES_JSON, choices_json) &&
            DCC_BIT_PAIR(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHOICES, choices, choices_count) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS_JSON, options_json) &&
            DCC_BIT_PAIR(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_OPTIONS, options, options_count) &&
            DCC_BIT_PAIR(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_CHANNEL_TYPES, channel_types, channel_types_count) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_INTEGER_VALUE, min_integer_value) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_INTEGER_VALUE, max_integer_value) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MIN_NUMBER_VALUE, min_number_value) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_MAX_NUMBER_VALUE, max_number_value) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_TYPE, type) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_REQUIRED, required) &&
            DCC_BIT_FIELD(dcc_application_command_option_builder_t, out, DCC_APPLICATION_COMMAND_OPTION_BUILDER_PRESENT_AUTOCOMPLETE, autocomplete)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_application_command_builder_abi_validate(
    const dcc_application_command_builder_t *builder,
    dcc_builder_abi_view_t *out
) {
    dcc_status_t status = dcc_builder_abi_read(
        builder, DCC_APPLICATION_COMMAND_BUILDER_VERSION, DCC_BUILDER_KNOWN_COMMAND, out
    );
    if (status != DCC_OK) {
        return status;
    }
    return DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME, name) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NAME_LOCALIZATIONS_JSON, name_localizations_json) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION, description) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DESCRIPTION_LOCALIZATIONS_JSON, description_localizations_json) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS_JSON, options_json) &&
            DCC_BIT_PAIR(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_OPTIONS, options, options_count) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_INTEGRATION_TYPES_JSON, integration_types_json) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_CONTEXTS_JSON, contexts_json) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_TYPE, type) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS, default_member_permissions) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DEFAULT_MEMBER_PERMISSIONS, default_member_permissions_null) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_DM_PERMISSION, dm_permission) &&
            DCC_BIT_FIELD(dcc_application_command_builder_t, out, DCC_APPLICATION_COMMAND_BUILDER_PRESENT_NSFW, nsfw)
        ? DCC_OK : DCC_ERR_INVALID_ARG;
}

dcc_status_t dcc_embed_builder_array_begin(
    const dcc_embed_builder_t *values,
    size_t count,
    size_t *out_stride
) {
    return dcc_builder_abi_array_begin(
        values, count, _Alignof(dcc_embed_builder_t),
        DCC_BUILDER_ABI_MINIMUM(dcc_embed_builder_t),
        DCC_EMBED_BUILDER_VERSION, DCC_BUILDER_KNOWN_EMBED, out_stride
    );
}

dcc_status_t dcc_autocomplete_choice_array_begin(
    const dcc_autocomplete_choice_t *values,
    size_t count,
    size_t *out_stride
) {
    return dcc_builder_abi_array_begin(
        values, count, _Alignof(dcc_autocomplete_choice_t),
        DCC_BUILDER_ABI_MINIMUM(dcc_autocomplete_choice_t),
        DCC_AUTOCOMPLETE_CHOICE_VERSION, DCC_BUILDER_KNOWN_CHOICE, out_stride
    );
}

dcc_status_t dcc_application_command_option_builder_array_begin(
    const dcc_application_command_option_builder_t *values,
    size_t count,
    size_t *out_stride
) {
    return dcc_builder_abi_array_begin(
        values, count, _Alignof(dcc_application_command_option_builder_t),
        DCC_BUILDER_ABI_MINIMUM(dcc_application_command_option_builder_t),
        DCC_APPLICATION_COMMAND_OPTION_BUILDER_VERSION,
        DCC_BUILDER_KNOWN_OPTION, out_stride
    );
}

dcc_status_t dcc_application_command_builder_array_begin(
    const dcc_application_command_builder_t *values,
    size_t count,
    size_t *out_stride
) {
    return dcc_builder_abi_array_begin(
        values, count, _Alignof(dcc_application_command_builder_t),
        DCC_BUILDER_ABI_MINIMUM(dcc_application_command_builder_t),
        DCC_APPLICATION_COMMAND_BUILDER_VERSION,
        DCC_BUILDER_KNOWN_COMMAND, out_stride
    );
}
