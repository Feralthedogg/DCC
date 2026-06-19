#include "internal/rest/dcc_rest_channel_builder_json_internal.h"

dcc_status_t dcc_rest_channel_append_base_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    const dcc_channel_params_t *params = ctx->params;
    dcc_status_t status = dcc_rest_channel_append_identity_members(body, first, ctx);
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_permission_members(body, first, params);
    }
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_rate_limit_members(body, first, params);
    }
    if (status == DCC_OK) {
        status = dcc_rest_channel_append_voice_members(body, first, params);
    }
    return status;
}
