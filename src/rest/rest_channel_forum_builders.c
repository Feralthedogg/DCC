#include "internal/rest/dcc_rest_channel_forum_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#include <string.h>

int dcc_rest_channel_forum_layout_valid(dcc_channel_forum_layout_t layout) {
    return layout == DCC_CHANNEL_FORUM_LAYOUT_NOT_SET ||
           layout == DCC_CHANNEL_FORUM_LAYOUT_LIST_VIEW ||
           layout == DCC_CHANNEL_FORUM_LAYOUT_GALLERY_VIEW;
}

int dcc_rest_channel_forum_tag_valid(const dcc_channel_forum_tag_params_t *tag) {
    if (tag == NULL ||
        tag->size < sizeof(*tag) ||
        tag->name == NULL ||
        strlen(tag->name) > 20U ||
        (tag->emoji_id != 0 && tag->emoji_name != NULL && tag->emoji_name[0] != '\0')) {
        return 0;
    }
    return 1;
}

static dcc_status_t dcc_rest_build_channel_forum_tag_body(
    const dcc_channel_forum_tag_params_t *tag,
    dcc_rest_buffer_t *body
) {
    if (!dcc_rest_channel_forum_tag_valid(tag) || body == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(body, &first, "name", tag->name);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(body, &first, "moderated", tag->moderated);
    }
    if (status == DCC_OK && tag->emoji_id != 0) {
        status = dcc_rest_json_append_u64_string_member(body, &first, "emoji_id", tag->emoji_id);
    } else if (status == DCC_OK && tag->emoji_name != NULL && tag->emoji_name[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, &first, "emoji_name", tag->emoji_name);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_build_channel_forum_tags_array(
    const dcc_channel_forum_tag_params_t *tags,
    size_t tag_count,
    dcc_rest_buffer_t *body
) {
    if (body == NULL || (tag_count != 0 && tags == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_buffer_append_cstr(body, "[");
    for (size_t i = 0; i < tag_count && status == DCC_OK; ++i) {
        if (i != 0) {
            status = dcc_rest_buffer_append_cstr(body, ",");
        }
        if (status == DCC_OK) {
            status = dcc_rest_build_channel_forum_tag_body(&tags[i], body);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "]");
    }
    return status;
}
