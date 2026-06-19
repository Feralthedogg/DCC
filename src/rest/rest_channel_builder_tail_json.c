#include "internal/rest/dcc_rest_channel_builder_json_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"

dcc_status_t dcc_rest_channel_append_parent_archive_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_rest_channel_build_context_t *ctx
) {
    const dcc_channel_params_t *params = ctx->params;
    dcc_status_t status = dcc_rest_json_append_u64_member(body, first, "type", params->type);
    if (params->type != DCC_CHANNEL_DM) {
        if (status == DCC_OK && params->parent_id != 0) {
            status = dcc_rest_json_append_u64_string_member(body, first, "parent_id", params->parent_id);
        }
        if (status == DCC_OK) {
            status = dcc_rest_json_append_bool_member(body, first, "nsfw", params->nsfw);
        }
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_u64_member(
            body,
            first,
            "default_auto_archive_duration",
            ctx->archive_duration
        );
    }
    return status;
}

dcc_status_t dcc_rest_channel_append_tail_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_channel_params_t *params
) {
    dcc_status_t status = DCC_OK;
    if (params->default_sort_order != 0) {
        status = dcc_rest_json_append_u64_member(body, first, "default_sort_order", params->default_sort_order);
    }
    if (status == DCC_OK && params->lock_permissions != 0) {
        status = dcc_rest_json_append_bool_member(body, first, "lock_permissions", 1);
    }
    return status;
}
