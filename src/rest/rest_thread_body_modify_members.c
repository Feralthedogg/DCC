#include "internal/rest/dcc_rest_thread_body_internal.h"

dcc_status_t dcc_rest_thread_body_append_modify(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_channel_auto_archive_duration_t archive_duration
) {
    dcc_status_t status = dcc_rest_json_append_bool_member(body, first, "archived", params->archived);
    if (status == DCC_OK) {
        status = dcc_rest_thread_body_append_archive(body, first, archive_duration);
    }
    if (status == DCC_OK) {
        status = dcc_rest_json_append_bool_member(body, first, "locked", params->locked);
    }
    if (status == DCC_OK && params->type == DCC_CHANNEL_PRIVATE_THREAD) {
        status = dcc_rest_json_append_bool_member(body, first, "invitable", params->invitable);
    }
    if (status == DCC_OK && params->rate_limit_per_user != 0) {
        status = dcc_rest_json_append_u64_member(body, first, "rate_limit_per_user", params->rate_limit_per_user);
    }
    if (status == DCC_OK && params->applied_tag_count != 0) {
        status = dcc_rest_thread_body_append_applied_tags_member(body, first, params->applied_tags, params->applied_tag_count);
    }
    return status;
}
