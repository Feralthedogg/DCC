#include "internal/dcc_command_sync.h"

#include <stdlib.h>

dcc_status_t dcc_command_sync_load_file(
    const char *path,
    char **out,
    size_t *out_len
) {
    if (path == NULL || out == NULL || out_len == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;
    *out_len = 0;

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return DCC_ERR_RUNTIME;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    long len = ftell(file);
    if (len < 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return DCC_ERR_RUNTIME;
    }

    char *data = (char *)malloc((size_t)len + 1U);
    if (data == NULL) {
        fclose(file);
        return DCC_ERR_NOMEM;
    }
    size_t read_len = fread(data, 1U, (size_t)len, file);
    if (read_len != (size_t)len || ferror(file)) {
        free(data);
        fclose(file);
        return DCC_ERR_RUNTIME;
    }
    fclose(file);
    data[read_len] = '\0';
    *out = data;
    *out_len = read_len;
    return DCC_OK;
}
