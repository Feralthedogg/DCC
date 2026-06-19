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

static char *dcc_worker_memdup_text(const void *data, size_t len) {
    char *out = (char *)malloc(len + 1U);
    if (out == NULL) {
        return NULL;
    }
    if (len != 0U) {
        memcpy(out, data, len);
    }
    out[len] = '\0';
    return out;
}

static dcc_status_t dcc_worker_capture_grow(dcc_hot_reload_worker_capture_t *capture) {
    size_t next_cap = 4U;
    if (capture->cap != 0U) {
        if (capture->cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap = capture->cap * 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(capture->items[0])) {
        return DCC_ERR_NOMEM;
    }
    void *next = realloc(capture->items, next_cap * sizeof(capture->items[0]));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    capture->items = (dcc_hot_reload_worker_rest_entry_t *)next;
    capture->cap = next_cap;
    return DCC_OK;
}

dcc_status_t dcc_hot_reload_worker_capture_push(
    dcc_hot_reload_worker_capture_t *capture,
    const char *method,
    const char *path,
    const void *body,
    size_t body_len,
    const char *content_type
) {
    if (capture == NULL || method == NULL || path == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    size_t method_len = strlen(method);
    size_t path_len = strlen(path);
    size_t content_type_len = content_type != NULL ? strlen(content_type) : 0U;
    if (capture->count >= DCC_HOT_RELOAD_WORKER_MAX_REST_COUNT ||
        method_len == 0U ||
        path_len == 0U ||
        method_len > DCC_HOT_RELOAD_WORKER_MAX_METHOD_LEN ||
        path_len > DCC_HOT_RELOAD_WORKER_MAX_PATH_LEN ||
        content_type_len > DCC_HOT_RELOAD_WORKER_MAX_CONTENT_TYPE_LEN ||
        body_len > DCC_HOT_RELOAD_WORKER_MAX_BODY_LEN) {
        return DCC_ERR_INVALID_ARG;
    }
    if (capture->count == capture->cap) {
        dcc_status_t status = dcc_worker_capture_grow(capture);
        if (status != DCC_OK) {
            return status;
        }
    }

    dcc_hot_reload_worker_rest_entry_t *entry = &capture->items[capture->count];
    memset(entry, 0, sizeof(*entry));
    entry->method = dcc_worker_memdup_text(method, method_len);
    entry->path = dcc_worker_memdup_text(path, path_len);
    entry->content_type = dcc_worker_memdup_text(
        content_type != NULL ? content_type : "",
        content_type_len
    );
    entry->body = dcc_worker_memdup_text(body != NULL ? body : "", body_len);
    entry->body_len = body_len;
    if (entry->method == NULL || entry->path == NULL ||
        entry->content_type == NULL || entry->body == NULL) {
        dcc_worker_rest_entry_deinit(entry);
        return DCC_ERR_NOMEM;
    }
    capture->count++;
    return DCC_OK;
}
