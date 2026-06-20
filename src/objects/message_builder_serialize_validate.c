#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"

dcc_status_t dcc_message_builder_validate_for_json(const dcc_message_builder_t *builder) {
    if (builder == NULL ||
        (builder->has_content && builder->content == NULL) ||
        (builder->has_nonce && builder->nonce == NULL) ||
        (builder->embeds_count != 0 && builder->embeds == NULL) ||
        (builder->components_count != 0 && builder->components == NULL) ||
        (builder->components_v2_count != 0 && builder->components_v2 == NULL) ||
        (builder->poll != NULL && dcc_message_poll_validate(builder->poll) != DCC_OK) ||
        (builder->sticker_ids_count != 0 && builder->sticker_ids == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    const int has_components_v2 = builder->components_v2_count != 0 ||
        builder->components_v2_json != NULL;
    if (has_components_v2 &&
        (builder->has_content ||
            builder->embeds_count != 0 ||
            builder->embeds_json != NULL ||
            builder->components_count != 0 ||
            builder->components_json != NULL ||
            builder->poll != NULL ||
            builder->poll_json != NULL ||
            builder->sticker_ids_count != 0)) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}
