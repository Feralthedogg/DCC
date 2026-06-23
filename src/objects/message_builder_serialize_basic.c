#include "internal/objects/dcc_message_builder_serialize_internal.h"
#include "internal/objects/dcc_message_json_members_internal.h"

dcc_status_t dcc_message_builder_append_basic_json(
    dcc_message_json_buffer_t *buffer,
    int *first,
    const dcc_message_builder_t *builder
) {
    dcc_status_t status = DCC_OK;
    if (builder->has_content) {
        status = dcc_message_json_append_string_member(buffer, first, "content", builder->content);
    }
    if (status == DCC_OK && builder->has_tts) {
        status = dcc_message_json_member_prefix(buffer, first, "tts");
        if (status == DCC_OK) {
            status = dcc_message_json_append_cstr(buffer, builder->tts ? "true" : "false");
        }
    }
    const int has_components_v2 = builder->components_v2_count != 0 ||
        builder->components_v2_json != NULL;
    if (status == DCC_OK && (builder->has_flags || has_components_v2)) {
        uint64_t flags = builder->flags;
        if (has_components_v2) {
            flags |= DCC_MESSAGE_FLAG_IS_COMPONENTS_V2;
        }
        status = dcc_message_json_member_prefix(buffer, first, "flags");
        if (status == DCC_OK) {
            status = dcc_message_json_append_u64(buffer, flags);
        }
    }
    if (status == DCC_OK && builder->has_nonce) {
        status = dcc_message_json_append_string_member(buffer, first, "nonce", builder->nonce);
    }
    if (status == DCC_OK && builder->sticker_ids_count != 0) {
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
    if (status == DCC_OK && builder->message_reference_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "message_reference", builder->message_reference_json);
    }
    if (status == DCC_OK && builder->allowed_mentions_json != NULL) {
        status = dcc_message_json_append_raw_member(buffer, first, "allowed_mentions", builder->allowed_mentions_json);
    }
    if (status == DCC_OK && builder->allowed_mentions != NULL) {
        status = dcc_message_json_member_prefix(buffer, first, "allowed_mentions");
        if (status == DCC_OK) {
            status = dcc_allowed_mentions_append_json(builder->allowed_mentions, buffer);
        }
    }
    return status;
}
