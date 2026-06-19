#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <string.h>

int dcc_hot_reload_worker_capture_send_result(
    dcc_hot_reload_worker_capture_t *capture,
    dcc_status_t status
) {
    if (capture != NULL && capture->count > DCC_HOT_RELOAD_WORKER_MAX_REST_COUNT) {
        return -1;
    }
    dcc_hot_reload_worker_result_t result = {
        .status = (uint32_t)status,
        .rest_count = (uint32_t)(capture != NULL ? capture->count : 0U),
    };
    if (dcc_hot_reload_worker_send_header(
            DCC_HOT_RELOAD_WORKER_FD_OUT,
            DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT,
            sizeof(result)
        ) != 0 ||
        dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &result, sizeof(result)) != 0) {
        return -1;
    }
    for (size_t i = 0; capture != NULL && i < capture->count; ++i) {
        dcc_hot_reload_worker_rest_entry_t *entry = &capture->items[i];
        size_t method_len = strlen(entry->method);
        size_t path_len = strlen(entry->path);
        size_t content_type_len = strlen(entry->content_type);
        if (method_len == 0U ||
            path_len == 0U ||
            method_len > DCC_HOT_RELOAD_WORKER_MAX_METHOD_LEN ||
            path_len > DCC_HOT_RELOAD_WORKER_MAX_PATH_LEN ||
            content_type_len > DCC_HOT_RELOAD_WORKER_MAX_CONTENT_TYPE_LEN ||
            entry->body_len > DCC_HOT_RELOAD_WORKER_MAX_BODY_LEN) {
            return -1;
        }
        dcc_hot_reload_worker_rest_t rest = {
            .method_len = (uint32_t)method_len,
            .path_len = (uint32_t)path_len,
            .content_type_len = (uint32_t)content_type_len,
            .body_len = entry->body_len,
        };
        if (dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, &rest, sizeof(rest)) != 0 ||
            dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, entry->method, rest.method_len) != 0 ||
            dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, entry->path, rest.path_len) != 0 ||
            dcc_hot_reload_worker_write_all(
                DCC_HOT_RELOAD_WORKER_FD_OUT,
                entry->content_type,
                rest.content_type_len
            ) != 0 ||
            dcc_hot_reload_worker_write_all(DCC_HOT_RELOAD_WORKER_FD_OUT, entry->body, entry->body_len) != 0) {
            return -1;
        }
    }
    return 0;
}
