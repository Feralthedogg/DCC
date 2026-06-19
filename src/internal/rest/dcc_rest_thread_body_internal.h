#ifndef DCC_REST_THREAD_BODY_INTERNAL_H
#define DCC_REST_THREAD_BODY_INTERNAL_H

#include "internal/rest/dcc_rest_buffer_internal.h"
#include "internal/rest/dcc_rest_threads_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_thread_body_validate(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_channel_auto_archive_duration_t *out_archive_duration
);
dcc_status_t dcc_rest_thread_body_append_members(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_channel_auto_archive_duration_t archive_duration
);
dcc_status_t dcc_rest_thread_body_append_applied_tags_member(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_snowflake_t *tags,
    size_t tag_count
);
dcc_status_t dcc_rest_thread_body_append_archive(
    dcc_rest_buffer_t *body,
    int *first,
    dcc_channel_auto_archive_duration_t archive_duration
);
dcc_status_t dcc_rest_thread_body_append_forum_create(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_channel_auto_archive_duration_t archive_duration
);
dcc_status_t dcc_rest_thread_body_append_message_create(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_channel_auto_archive_duration_t archive_duration
);
dcc_status_t dcc_rest_thread_body_append_standalone_create(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_channel_auto_archive_duration_t archive_duration
);
dcc_status_t dcc_rest_thread_body_append_modify(
    dcc_rest_buffer_t *body,
    int *first,
    const dcc_thread_params_t *params,
    dcc_channel_auto_archive_duration_t archive_duration
);

#ifdef __cplusplus
}
#endif

#endif
