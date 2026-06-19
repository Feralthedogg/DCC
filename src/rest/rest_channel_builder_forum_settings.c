#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

#define DCC_CHANNEL_DISCORD_FLAG_REQUIRE_TAG (1U << 4)
#define DCC_CHANNEL_DISCORD_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS (1U << 15)

dcc_status_t dcc_rest_channel_append_forum_settings_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    const dcc_channel_params_t *params = ctx->params;
    dcc_status_t status = DCC_OK;
    if (params->type == DCC_CHANNEL_FORUM || params->type == DCC_CHANNEL_MEDIA) {
        uint32_t flags = ctx->require_tag != 0 ? DCC_CHANNEL_DISCORD_FLAG_REQUIRE_TAG : 0U;
        if (params->type == DCC_CHANNEL_MEDIA && ctx->hide_media_download_options != 0) {
            flags |= DCC_CHANNEL_DISCORD_FLAG_HIDE_MEDIA_DOWNLOAD_OPTIONS;
        }
        if (flags != 0) {
            status = dcc_rest_json_append_u64_member(body, first, "flags", flags);
        }
    }
    if (status == DCC_OK &&
        params->type == DCC_CHANNEL_FORUM &&
        ctx->default_forum_layout != DCC_CHANNEL_FORUM_LAYOUT_NOT_SET) {
        status = dcc_rest_json_append_u64_member(body, first, "default_forum_layout", ctx->default_forum_layout);
    }
    return status;
}
