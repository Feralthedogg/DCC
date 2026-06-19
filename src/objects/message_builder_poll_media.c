#include "internal/objects/dcc_message_json_members_internal.h"
#include "internal/objects/dcc_message_poll_internal.h"

static dcc_status_t dcc_message_poll_append_emoji(
    const dcc_component_emoji_t *emoji,
    dcc_message_json_buffer_t *buffer
) {
    dcc_status_t status = dcc_message_poll_validate_emoji(emoji);
    if (status != DCC_OK) {
        return status;
    }

    status = dcc_message_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK && emoji->has_id) {
        status = dcc_message_json_member_prefix(buffer, &first, "id");
        if (status == DCC_OK) {
            status = dcc_message_json_append_snowflake_string(buffer, emoji->id);
        }
    }
    if (status == DCC_OK && emoji->name != NULL) {
        status = dcc_message_json_append_string_member(buffer, &first, "name", emoji->name);
    }
    if (status == DCC_OK && emoji->has_animated) {
        status = dcc_message_json_append_bool_member(buffer, &first, "animated", emoji->animated);
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(buffer, "}");
    }
    return status;
}

dcc_status_t dcc_message_poll_append_media(
    const dcc_poll_media_t *media,
    dcc_message_json_buffer_t *buffer
) {
    if (media == NULL || media->text == NULL || (media->has_emoji &&
        dcc_message_poll_validate_emoji(&media->emoji) != DCC_OK)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_message_json_append_cstr(buffer, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_message_json_append_string_member(buffer, &first, "text", media->text);
    }
    if (status == DCC_OK && media->has_emoji) {
        status = dcc_message_json_member_prefix(buffer, &first, "emoji");
        if (status == DCC_OK) {
            status = dcc_message_poll_append_emoji(&media->emoji, buffer);
        }
    }
    if (status == DCC_OK) {
        status = dcc_message_json_append_cstr(buffer, "}");
    }
    return status;
}
