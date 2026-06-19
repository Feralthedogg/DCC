#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_channel_append_identity_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    const dcc_channel_params_t *params = ctx->params;
    dcc_status_t status = DCC_OK;
    if (ctx->include_id != 0) {
        status = dcc_rest_json_append_u64_string_member(body, first, "id", params->channel_id);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_string_member(body, first, "guild_id", params->guild_id);
    }
    if (status == DCC_OK && params->position != 0) {
        status = dcc_rest_json_append_u64_member(body, first, "position", params->position);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(body, first, "name", params->name);
    }
    if (status == DCC_OK && params->topic != NULL && params->topic[0] != '\0') {
        status = dcc_rest_json_append_string_member(body, first, "topic", params->topic);
    }
    return status;
}
