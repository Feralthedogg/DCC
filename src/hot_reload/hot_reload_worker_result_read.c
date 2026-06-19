#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <stdlib.h>
#include <string.h>

static void dcc_worker_rest_entry_deinit(dcc_hot_reload_worker_rest_entry_t *entry) {
    if (entry == NULL) {
        return;
    }
    free(entry->method);
    free(entry->path);
    free(entry->content_type);
    free(entry->body);
    memset(entry, 0, sizeof(*entry));
}

static char *dcc_worker_read_text(int fd, size_t len, uint32_t timeout_ms) {
    if (len > SIZE_MAX - 1U) {
        return NULL;
    }
    char *out = (char *)malloc(len + 1U);
    if (out == NULL) {
        return NULL;
    }
    if (len != 0U &&
        dcc_hot_reload_worker_read_all_timeout(fd, out, len, timeout_ms) != 0) {
        free(out);
        return NULL;
    }
    out[len] = '\0';
    return out;
}

static dcc_status_t dcc_worker_read_rest_entry(
    int fd,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_rest_entry_t *out
) {
    dcc_hot_reload_worker_rest_t rest;
    if (dcc_hot_reload_worker_read_all_timeout(fd, &rest, sizeof(rest), timeout_ms) != 0 ||
        rest.method_len == 0U ||
        rest.path_len == 0U ||
        rest.method_len > DCC_HOT_RELOAD_WORKER_MAX_METHOD_LEN ||
        rest.path_len > DCC_HOT_RELOAD_WORKER_MAX_PATH_LEN ||
        rest.content_type_len > DCC_HOT_RELOAD_WORKER_MAX_CONTENT_TYPE_LEN ||
        rest.body_len > DCC_HOT_RELOAD_WORKER_MAX_BODY_LEN ||
        rest.body_len > SIZE_MAX) {
        return DCC_ERR_RUNTIME;
    }
    memset(out, 0, sizeof(*out));
    out->method = dcc_worker_read_text(fd, rest.method_len, timeout_ms);
    out->path = dcc_worker_read_text(fd, rest.path_len, timeout_ms);
    out->content_type = dcc_worker_read_text(fd, rest.content_type_len, timeout_ms);
    out->body = dcc_worker_read_text(fd, (size_t)rest.body_len, timeout_ms);
    out->body_len = (size_t)rest.body_len;
    if (out->method == NULL || out->path == NULL || out->content_type == NULL || out->body == NULL) {
        dcc_worker_rest_entry_deinit(out);
        return DCC_ERR_NOMEM;
    }
    return DCC_OK;
}

dcc_status_t dcc_hot_reload_worker_result_set_read(
    dcc_hot_reload_worker_process_t *worker,
    uint32_t timeout_ms,
    dcc_hot_reload_worker_result_set_t *out
) {
    dcc_hot_reload_worker_header_t header;
    if (dcc_hot_reload_worker_read_header(worker->out_fd, &header, timeout_ms) != 0 ||
        header.kind != DCC_HOT_RELOAD_WORKER_MSG_EVENT_RESULT ||
        header.size != sizeof(dcc_hot_reload_worker_result_t)) {
        return DCC_ERR_RUNTIME;
    }
    dcc_hot_reload_worker_result_t result;
    if (dcc_hot_reload_worker_read_all_timeout(worker->out_fd, &result, sizeof(result), timeout_ms) != 0) {
        return DCC_ERR_RUNTIME;
    }
    memset(out, 0, sizeof(*out));
    out->status = (dcc_status_t)result.status;
    if (result.rest_count == 0U) {
        return DCC_OK;
    }
    if (result.rest_count > DCC_HOT_RELOAD_WORKER_MAX_REST_COUNT) {
        return DCC_ERR_RUNTIME;
    }
    out->items = (dcc_hot_reload_worker_rest_entry_t *)calloc(result.rest_count, sizeof(out->items[0]));
    if (out->items == NULL) {
        return DCC_ERR_NOMEM;
    }
    out->count = result.rest_count;
    for (size_t i = 0; i < out->count; ++i) {
        dcc_status_t status = dcc_worker_read_rest_entry(worker->out_fd, timeout_ms, &out->items[i]);
        if (status != DCC_OK) {
            dcc_hot_reload_worker_result_set_deinit(out);
            return status;
        }
    }
    return DCC_OK;
}
