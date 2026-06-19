#include "internal/rest/dcc_rest_channel_builder_json_internal.h"

dcc_status_t dcc_rest_channel_append_body_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    const dcc_channel_params_t *params = ctx->params;
    dcc_status_t status = dcc_rest_channel_append_base_members(body, first, ctx);
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_forum_settings_members(body, first, ctx);
    }
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_parent_archive_members(body, first, ctx);
    }
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_forum_detail_members(body, first, ctx);
    }
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_tail_members(body, first, params);
    }
    return status;
}
