#include <dcc/store.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dcc_store_file_backend {
    char *path;
} dcc_store_file_backend_t;

static char *dcc_store_file_strdup(const char *text) {
    size_t len = strlen(text);
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, text, len + 1U);
    return copy;
}

static dcc_status_t dcc_store_file_read_all(
    const char *path,
    char **out_data,
    size_t *out_len
) {
    *out_data = NULL;
    *out_len = 0;

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return errno == ENOENT ? DCC_ERR_NOT_FOUND : DCC_ERR_RUNTIME;
    }
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    long file_len = ftell(file);
    if (file_len < 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    if (fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }

    char *data = (char *)malloc((size_t)file_len + 1U);
    if (data == NULL) {
        fclose(file);
        return DCC_ERR_NOMEM;
    }
    size_t read_len = fread(data, 1U, (size_t)file_len, file);
    if (read_len != (size_t)file_len || ferror(file)) {
        free(data);
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    fclose(file);
    data[read_len] = '\0';
    *out_data = data;
    *out_len = read_len;
    return DCC_OK;
}

static dcc_status_t dcc_store_file_write_all(
    const char *path,
    const char *data,
    size_t len
) {
    size_t path_len = strlen(path);
    if (path_len > ((size_t)-1) - 5U) {
        return DCC_ERR_NOMEM;
    }

    char *tmp_path = (char *)malloc(path_len + 5U);
    if (tmp_path == NULL) {
        return DCC_ERR_NOMEM;
    }
    memcpy(tmp_path, path, path_len);
    memcpy(tmp_path + path_len, ".tmp", 5U);

    FILE *file = fopen(tmp_path, "wb");
    if (file == NULL) {
        free(tmp_path);
        return DCC_ERR_RUNTIME;
    }
    if (len != 0U && fwrite(data, 1U, len, file) != len) {
        fclose(file);
        remove(tmp_path);
        free(tmp_path);
        return DCC_ERR_RUNTIME;
    }
    if (fclose(file) != 0) {
        remove(tmp_path);
        free(tmp_path);
        return DCC_ERR_RUNTIME;
    }

#ifdef _WIN32
    remove(path);
#endif
    if (rename(tmp_path, path) != 0) {
        remove(tmp_path);
        free(tmp_path);
        return DCC_ERR_RUNTIME;
    }
    free(tmp_path);
    return DCC_OK;
}

static const char *dcc_store_file_line_end(const char *cursor, const char *end) {
    const char *line_end = cursor;
    while (line_end < end && *line_end != '\n') {
        ++line_end;
    }
    return line_end;
}

static int dcc_store_file_line_matches(
    const char *line,
    const char *line_end,
    const char *key,
    size_t key_len,
    const char **out_value,
    size_t *out_value_len
) {
    const char *content_end = line_end;
    if (content_end > line && content_end[-1] == '\r') {
        --content_end;
    }

    const char *eq = line;
    while (eq < content_end && *eq != '=') {
        ++eq;
    }
    if (eq == content_end || (size_t)(eq - line) != key_len || memcmp(line, key, key_len) != 0) {
        return 0;
    }

    if (out_value != NULL) {
        *out_value = eq + 1;
    }
    if (out_value_len != NULL) {
        *out_value_len = (size_t)(content_end - (eq + 1));
    }
    return 1;
}

static dcc_status_t dcc_store_file_get(
    void *user_data,
    const char *key,
    char *out_value,
    size_t out_value_size,
    size_t *out_value_len
) {
    dcc_store_file_backend_t *backend = (dcc_store_file_backend_t *)user_data;
    char *data = NULL;
    size_t data_len = 0;
    dcc_status_t status = dcc_store_file_read_all(backend->path, &data, &data_len);
    if (status != DCC_OK) {
        return status;
    }

    const char *cursor = data;
    const char *end = data + data_len;
    size_t key_len = strlen(key);
    while (cursor < end) {
        const char *line_end = dcc_store_file_line_end(cursor, end);
        const char *value = NULL;
        size_t value_len = 0;
        if (dcc_store_file_line_matches(cursor, line_end, key, key_len, &value, &value_len)) {
            if (out_value_len != NULL) {
                *out_value_len = value_len;
            }
            if (out_value != NULL) {
                if (out_value_size <= value_len) {
                    free(data);
                    return DCC_ERR_STATE;
                }
                memcpy(out_value, value, value_len);
                out_value[value_len] = '\0';
            }
            free(data);
            return DCC_OK;
        }
        cursor = line_end < end ? line_end + 1 : line_end;
    }

    free(data);
    return DCC_ERR_NOT_FOUND;
}

static dcc_status_t dcc_store_file_set(
    void *user_data,
    const char *key,
    const char *value
) {
    dcc_store_file_backend_t *backend = (dcc_store_file_backend_t *)user_data;
    char *data = NULL;
    size_t data_len = 0;
    dcc_status_t status = dcc_store_file_read_all(backend->path, &data, &data_len);
    if (status == DCC_ERR_NOT_FOUND) {
        status = DCC_OK;
    }
    if (status != DCC_OK) {
        return status;
    }

    size_t key_len = strlen(key);
    size_t value_len = strlen(value);
    if (data_len > ((size_t)-1) - key_len - value_len - 3U) {
        free(data);
        return DCC_ERR_NOMEM;
    }
    char *out = (char *)malloc(data_len + key_len + value_len + 3U);
    if (out == NULL) {
        free(data);
        return DCC_ERR_NOMEM;
    }

    size_t out_len = 0;
    const char *cursor = data;
    const char *end = data + data_len;
    while (cursor < end) {
        const char *line_end = dcc_store_file_line_end(cursor, end);
        const char *next = line_end < end ? line_end + 1 : line_end;
        if (!dcc_store_file_line_matches(cursor, line_end, key, key_len, NULL, NULL)) {
            size_t line_size = (size_t)(next - cursor);
            memcpy(out + out_len, cursor, line_size);
            out_len += line_size;
        }
        cursor = next;
    }
    if (out_len != 0U && out[out_len - 1U] != '\n') {
        out[out_len++] = '\n';
    }
    memcpy(out + out_len, key, key_len);
    out_len += key_len;
    out[out_len++] = '=';
    memcpy(out + out_len, value, value_len);
    out_len += value_len;
    out[out_len++] = '\n';

    status = dcc_store_file_write_all(backend->path, out, out_len);
    free(out);
    free(data);
    return status;
}

static dcc_status_t dcc_store_file_delete(
    void *user_data,
    const char *key
) {
    dcc_store_file_backend_t *backend = (dcc_store_file_backend_t *)user_data;
    char *data = NULL;
    size_t data_len = 0;
    dcc_status_t status = dcc_store_file_read_all(backend->path, &data, &data_len);
    if (status != DCC_OK) {
        return status;
    }

    char *out = (char *)malloc(data_len + 1U);
    if (out == NULL) {
        free(data);
        return DCC_ERR_NOMEM;
    }

    int found = 0;
    size_t out_len = 0;
    size_t key_len = strlen(key);
    const char *cursor = data;
    const char *end = data + data_len;
    while (cursor < end) {
        const char *line_end = dcc_store_file_line_end(cursor, end);
        const char *next = line_end < end ? line_end + 1 : line_end;
        if (dcc_store_file_line_matches(cursor, line_end, key, key_len, NULL, NULL)) {
            found = 1;
        } else {
            size_t line_size = (size_t)(next - cursor);
            memcpy(out + out_len, cursor, line_size);
            out_len += line_size;
        }
        cursor = next;
    }

    if (!found) {
        free(out);
        free(data);
        return DCC_ERR_NOT_FOUND;
    }

    status = dcc_store_file_write_all(backend->path, out, out_len);
    free(out);
    free(data);
    return status;
}

static void dcc_store_file_cleanup(void *user_data) {
    dcc_store_file_backend_t *backend = (dcc_store_file_backend_t *)user_data;
    if (backend == NULL) {
        return;
    }
    free(backend->path);
    free(backend);
}

dcc_status_t dcc_store_file_open(const char *path, dcc_store_t *out_store) {
    dcc_store_file_options_t options = {
        .size = sizeof(options),
        .path = path,
    };
    return dcc_store_file_open_options(&options, out_store);
}

dcc_status_t dcc_store_file_open_options(
    const dcc_store_file_options_t *options,
    dcc_store_t *out_store
) {
    if (options == NULL || options->path == NULL || options->path[0] == '\0' || out_store == NULL) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_store_file_backend_t *backend =
        (dcc_store_file_backend_t *)calloc(1U, sizeof(*backend));
    if (backend == NULL) {
        return DCC_ERR_NOMEM;
    }
    backend->path = dcc_store_file_strdup(options->path);
    if (backend->path == NULL) {
        free(backend);
        return DCC_ERR_NOMEM;
    }

    dcc_store_options_t store_options = {
        .size = sizeof(store_options),
        .get = dcc_store_file_get,
        .set = dcc_store_file_set,
        .delete_key = dcc_store_file_delete,
        .cleanup = dcc_store_file_cleanup,
        .user_data = backend,
    };
    dcc_status_t status = dcc_store_init(out_store, &store_options);
    if (status != DCC_OK) {
        dcc_store_file_cleanup(backend);
    }
    return status;
}
