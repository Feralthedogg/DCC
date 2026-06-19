#include "internal/rest/dcc_rest_channel_body_internal.h"
#include "internal/rest/dcc_rest_thread_body_internal.h"

#include <string.h>

static int dcc_rest_thread_type_valid(dcc_channel_type_t type) {
    return type == DCC_CHANNEL_ANNOUNCEMENT_THREAD ||
           type == DCC_CHANNEL_PUBLIC_THREAD ||
           type == DCC_CHANNEL_PRIVATE_THREAD;
}

static dcc_status_t dcc_rest_thread_archive_duration(
    dcc_channel_auto_archive_duration_t requested,
    dcc_channel_auto_archive_duration_t *out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_channel_auto_archive_duration_t duration = requested;
    if (duration == 0) {
        duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY;
    }
    if (!dcc_rest_channel_archive_duration_valid(duration)) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = duration;
    return DCC_OK;
}

dcc_status_t dcc_rest_thread_body_validate(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    dcc_channel_auto_archive_duration_t *out_archive_duration
) {
    if (params == NULL ||
        params->size < sizeof(*params) ||
        params->name == NULL ||
        params->name[0] == '\0' ||
        strlen(params->name) > 100U ||
        (params->applied_tag_count != 0 && params->applied_tags == NULL)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_thread_archive_duration(params->auto_archive_duration, out_archive_duration);
    if (status != DCC_OK) {
        return status;
    }

    if ((mode == DCC_REST_THREAD_BODY_CREATE ||
         mode == DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE ||
         mode == DCC_REST_THREAD_BODY_CREATE_FORUM) &&
        params->channel_id == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if ((mode == DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE && params->message_id == 0) ||
        (mode == DCC_REST_THREAD_BODY_MODIFY && params->thread_id == 0) ||
        (mode == DCC_REST_THREAD_BODY_CREATE && !dcc_rest_thread_type_valid(params->type)) ||
        (mode == DCC_REST_THREAD_BODY_CREATE_FORUM &&
         (params->message_json == NULL || params->message_json[0] == '\0'))) {
        return DCC_ERR_INVALID_ARG;
    }
    return DCC_OK;
}
