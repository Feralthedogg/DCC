#ifndef DCC_REST_CHANNEL_BUILDER_JSON_INTERNAL_H
#define DCC_REST_CHANNEL_BUILDER_JSON_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_channel_builder_context_internal.h"

dcc_status_t dcc_rest_channel_append_body_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_base_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_identity_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_permission_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
);
dcc_status_t dcc_rest_channel_append_rate_limit_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
);
dcc_status_t dcc_rest_channel_append_voice_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
);
dcc_status_t dcc_rest_channel_append_forum_settings_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_forum_detail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_parent_archive_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
);
dcc_status_t dcc_rest_channel_append_tail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
);

#endif
