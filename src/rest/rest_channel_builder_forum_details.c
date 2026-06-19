#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_channel_forum_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

static dcc_status_t dcc_rest_channel_append_forum_tags_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    if (ctx->available_tag_count == 0) {
        return DCC_OK;
    }

    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"available_tags\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_build_channel_forum_tags_array(ctx->available_tags, ctx->available_tag_count, body);
    }
    return status;
}

static dcc_status_t dcc_rest_channel_append_default_reaction_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    if (ctx->default_reaction_emoji_id == 0 &&
        (ctx->default_reaction_emoji_name == NULL || ctx->default_reaction_emoji_name[0] == '\0')) {
        return DCC_OK;
    }

    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"default_reaction_emoji\":{");
    }
    int reaction_first = 1;
    if (status == DCC_OK && ctx->default_reaction_emoji_id != 0) {
        status = dcc_rest_json_append_u64_string_member(
            body,
            &reaction_first,
            "emoji_id",
            ctx->default_reaction_emoji_id
        );
    } else if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(
            body,
            &reaction_first,
            "emoji_name",
            ctx->default_reaction_emoji_name
        );
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "}");
    }
    return status;
}

dcc_status_t dcc_rest_channel_append_forum_detail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    dcc_status_t status = dcc_rest_channel_append_forum_tags_member(body, first, ctx);
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_default_reaction_member(body, first, ctx);
    }
    return status;
}
