#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_builder_abi_internal.h"

dcc_status_t dcc_message_builder_append_basic_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
) {
    dcc_builder_abi_view_t view;
    if (dcc_message_builder_abi_validate(builder, &view) != DCC_OK) {
        return DCC_ERR_INVALID_ARG;
    }
#define HAS(bit_) dcc_builder_abi_view_has(&view, (bit_))
    dcc_status_t status = DCC_OK;
    if (HAS(DCC_MESSAGE_BUILDER_PRESENT_CONTENT)) {
        status = dcc_message_json_append_string_member(buffer, first, "content", builder->content);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_TTS)) {
        status = dcc_message_json_member_prefix(buffer, first, "tts");
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(buffer, builder->tts ? "true" : "false");
        }
    }
    const int has_components_v2 =
        (HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2) && builder->components_v2_count != 0U) ||
        HAS(DCC_MESSAGE_BUILDER_PRESENT_COMPONENTS_V2_JSON);
    if (status == DCC_OK && (HAS(DCC_MESSAGE_BUILDER_PRESENT_FLAGS) || has_components_v2)) {
        uint64_t flags = HAS(DCC_MESSAGE_BUILDER_PRESENT_FLAGS) ? builder->flags : 0U;
        if (has_components_v2) {
            flags |= DCC_MESSAGE_FLAG_IS_COMPONENTS_V2;
        }
        status = dcc_message_json_member_prefix(buffer, first, "flags");
        if (status == DCC_OK) {
            status = dcc_message_json_append_u64(buffer, flags);
        }
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_NONCE)) {
        status = dcc_message_json_append_string_member(buffer, first, "nonce", builder->nonce);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_STICKER_IDS) &&
        builder->sticker_ids_count != 0U) {
        status = dcc_message_json_member_prefix(buffer, first, "sticker_ids");
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(buffer, "[");
        }
        for (size_t i = 0; status == DCC_OK && i < builder->sticker_ids_count; ++i) {
            if (i != 0) {
                status = dcc_message_json_append_cstr(buffer, ",");
            }
            if (status == DCC_OK) {
                status = dcc_message_json_append_snowflake_string(buffer, builder->sticker_ids[i]);
            }
        }
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(buffer, "]");
        }
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_MESSAGE_REFERENCE_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "message_reference", builder->message_reference_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS_JSON)) {
        status = dcc_message_json_append_raw_member(buffer, first, "allowed_mentions", builder->allowed_mentions_json);
    }
    if (status == DCC_OK && HAS(DCC_MESSAGE_BUILDER_PRESENT_ALLOWED_MENTIONS)) {
        status = dcc_message_json_member_prefix(buffer, first, "allowed_mentions");
        if (status == DCC_OK) {
            status = dcc_allowed_mentions_append_json(builder->allowed_mentions, buffer);
        }
    }
    #undef HAS
    return status;
}
