#include "internal/rest/dcc_rest_thread_body_internal.h"

dcc_status_t dcc_rest_build_thread_body(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    char **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_channel_auto_archive_duration_t archive_duration = DCC_CHANNEL_AUTO_ARCHIVE_1_DAY;
    dcc_status_t status = dcc_rest_thread_body_validate(params, mode, &archive_duration);
    if (status != DCC_OK) {
        return status;
    }

    dcc_rest_buffer_t body = {0};
    status = dcc_rest_buffer_append_cstr(&body, "{");
    int first = 1;
    if (status == DCC_OK) {
        status = dcc_rest_json_append_string_member(&body, &first, "name", params->name);
    }
    if (status == DCC_OK) {
        status = dcc_rest_thread_body_append_members(&body, &first, params, mode, archive_duration);
    }
    if (status == DCC_OK) {
        status = dcc_rest_buffer_append_cstr(&body, "}");
    }
    if (status == DCC_OK) {
        *out = body.data;
        return DCC_OK;
    }
    dcc_rest_buffer_deinit(&body);
    return status;
}
