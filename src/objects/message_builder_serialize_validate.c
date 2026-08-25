#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"
#include "internal/objects/dcc_component_validation_internal.h"
#include "internal/objects/dcc_component_v2_internal.h"
#include "internal/objects/dcc_embed_builder_internal.h"

#include <stdint.h>

#define DCC_MESSAGE_MAX_EMBEDS 10U

static int dcc_message_array_span_valid(
    const void *values,
    size_t count,
    size_t item_size
) {
    if (count == 0U) {
        return 1;
    }
    if (values == NULL || item_size == 0U || count > SIZE_MAX / item_size) {
        return 0;
    }
    size_t span = count * item_size;
    uintptr_t address = (uintptr_t)values;
    return address <= UINTPTR_MAX - (span - 1U);
}

dcc_status_t dcc_message_builder_validate_for_json(const dcc_message_builder_t *builder) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }

#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    if ((HAS(DCC_MESSAGE_BUILDER_PRESENT_CONTENT) && builder->content == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_NONCE) && builder->nonce == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) &&
            builder->embeds_count != 0U && builder->embeds == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) &&
            builder->components_count != 0U && builder->components == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) &&
            builder->components_v2_count != 0U && builder->components_v2 == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL) &&
            (builder->poll == NULL || dcc_message_poll_validate(builder->poll) != DCC_OK)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON) &&
            builder->allowed_mentions_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS) &&
            (builder->allowed_mentions == NULL ||
             dcc_allowed_mentions_validate_for_json(builder->allowed_mentions) != DCC_OK)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) &&
            builder->sticker_ids_count != 0U && builder->sticker_ids == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON) &&
            builder->message_reference_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE) &&
            (builder->message_reference == NULL ||
             dcc_message_reference_validate(builder->message_reference, NULL) != DCC_OK)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON) && builder->embeds_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON) && builder->components_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON) && builder->attachments_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS) &&
            (builder->attachment_count == 0U || builder->attachments == NULL ||
             dcc_message_attachment_array_validate(
                 builder->attachments, builder->attachment_count, NULL
             ) != DCC_OK)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON) && builder->poll_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON) &&
            builder->components_v2_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON) &&
            HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON) &&
            HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE)) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON) &&
            HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS))) {
        return DCC_ERR_INVALID_ARG;
    }

    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) &&
        (builder->embeds_count > DCC_MESSAGE_MAX_EMBEDS ||
         dcc_embed_builder_validate_array(
             builder->embeds,
             builder->embeds_count
         ) != DCC_OK)) {
        return DCC_ERR_INVALID_ARG;
    }
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) &&
        dcc_component_validate_message_array(
            builder->components,
            builder->components_count
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) &&
        dcc_component_v2_validate(
            builder->components_v2,
            builder->components_v2_count,
            DCC_COMPONENT_V2_CONTEXT_MESSAGE_V2,
            NULL
        ) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) &&
        !dcc_message_array_span_valid(
            builder->sticker_ids,
            builder->sticker_ids_count,
            sizeof(*builder->sticker_ids)
        )) {
        return DCC_ERR_INVALID_ARG;
    }

    const int has_components_v2 =
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) && builder->components_v2_count != 0U) ||
        HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON);
    if (has_components_v2 &&
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_CONTENT) ||
            (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) && builder->embeds_count != 0U) ||
            HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON) ||
            (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) && builder->components_count != 0U) ||
            HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON) ||
            HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL) ||
            HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON) ||
            (HAS(DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) && builder->sticker_ids_count != 0U))) {
        return DCC_ERR_INVALID_ARG;
    }
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE)) {
        dcc_message_reference_type_t reference_type;
        if (dcc_message_reference_validate(
                builder->message_reference, &reference_type
            ) != DCC_OK) {
            return DCC_ERR_INVALID_ARG;
        }
        if (reference_type == DCC_MESSAGE_REFERENCE_FORWARD &&
            (HAS(DCC_MESSAGE_BUILDER_PRESENT_CONTENT) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS) ||
             HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON))) {
            return DCC_ERR_INVALID_ARG;
        }
    }
#undef HAS
    return DCC_OK;
}
