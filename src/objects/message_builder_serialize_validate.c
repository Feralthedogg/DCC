#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

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
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_EMBEDS_JSON) && builder->embeds_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_JSON) && builder->components_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ATTACHMENTS_JSON) && builder->attachments_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_POLL_JSON) && builder->poll_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON) &&
            builder->components_v2_json == NULL) ||
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON) &&
            HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS))) {
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
#undef HAS
    return DCC_OK;
}
