#ifndef DCC_REST_CHANNEL_BUILDER_CONTEXT_INTERNAL_H
#define DCC_REST_CHANNEL_BUILDER_CONTEXT_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>
#include <stdint.h>

typedef struct dcc_rest_channel_build_context {
    const dcc_channel_params_t *params;
    const dcc_channel_forum_tag_params_t *available_tags;
    const char *default_reaction_emoji_name;
    size_t available_tag_count;
    dcc_snowflake_t default_reaction_emoji_id;
    dcc_channel_forum_layout_t default_forum_layout;
    dcc_channel_auto_archive_duration_t archive_duration;
    uint8_t include_id;
    uint8_t require_tag;
    uint8_t hide_media_download_options;
} dcc_rest_channel_build_context_t;

int dcc_rest_channel_base_params_valid(const dcc_channel_params_t *params, uint8_t include_id);
int dcc_rest_channel_forum_params_valid(const dcc_rest_channel_build_context_t *ctx);
dcc_status_t dcc_rest_channel_build_context_init(
    dcc_rest_channel_build_context_t *ctx,
    const dcc_channel_params_t *params,
    uint8_t include_id
);

#endif
