#include "internal/rest/dcc_rest_thread_body_internal.h"

dcc_status_t dcc_rest_thread_body_append_applied_tags_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_snowflake_t *tags,
    size_t tag_count
) {
    if (body == NULL || first == NULL || (tag_count != 0 && tags == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_status_t status = dcc_rest_json_member_sep(body, first);
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(body, "\"applied_tags\":");
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_snowflake_json_string_array(body, tags, tag_count);
    }
    return status;
}

dcc_status_t dcc_rest_thread_body_append_archive(
    dcc_rest_buffer_t *body,
    int *first,
    dcc_channel_auto_archive_duration_t archive_duration
) {
    return dcc_rest_json_append_u64_member(body, first, "auto_archive_duration", archive_duration);
}

dcc_status_t dcc_rest_thread_body_append_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_channel_auto_archive_duration_t archive_duration
) {
    if (mode == DCC_REST_THREAD_BODY_CREATE_FORUM) {
        return dcc_rest_thread_body_append_forum_create(body, first, params, archive_duration);
    }
    if (mode == DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE) {
        return dcc_rest_thread_body_append_message_create(body, first, params, archive_duration);
    }
    if (mode == DCC_REST_THREAD_BODY_CREATE) {
        return dcc_rest_thread_body_append_standalone_create(body, first, params, archive_duration);
    }
    return dcc_rest_thread_body_append_modify(body, first, params, archive_duration);
}
