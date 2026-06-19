#include "internal/rest/dcc_rest_channel_body_internal.h"
#include "internal/rest/dcc_rest_channel_builder_context_internal.h"
#include "internal/rest/dcc_rest_config_internal.h"

dcc_status_t dcc_rest_channel_build_context_init(
    dcc_rest_channel_build_context_t *ctx,
    const dcc_channel_params_t *params,
    uint8_t include_id
) {
    if (ctx == NULL || !dcc_rest_channel_base_params_valid(params, include_id)) {
        return DCC_ERR_INVALID_ARG;
    }

    ctx->params = params;
    ctx->include_id = include_id;
    ctx->available_tags =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, available_tags) ? params->available_tags : NULL;
    ctx->available_tag_count =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, available_tag_count) ? params->available_tag_count : 0U;
    ctx->default_reaction_emoji_id =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, default_reaction_emoji_id)
            ? params->default_reaction_emoji_id
            : 0;
    ctx->default_reaction_emoji_name =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, default_reaction_emoji_name)
            ? params->default_reaction_emoji_name
            : NULL;
    ctx->default_forum_layout =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, default_forum_layout)
            ? params->default_forum_layout
            : DCC_CHANNEL_FORUM_LAYOUT_NOT_SET;
    ctx->require_tag =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, require_tag) ? params->require_tag : 0;
    ctx->hide_media_download_options =
        DCC_REST_HAS_FIELD(params, dcc_channel_params_t, hide_media_download_options)
            ? params->hide_media_download_options
            : 0;

    ctx->archive_duration = params->default_auto_archive_duration;
    if (ctx->archive_duration == 0) {
        ctx->archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY;
    }

    return dcc_rest_channel_forum_params_valid(ctx) &&
           dcc_rest_channel_archive_duration_valid(ctx->archive_duration)
        ? DCC_OK
        : DCC_ERR_INVALID_ARG;
}
