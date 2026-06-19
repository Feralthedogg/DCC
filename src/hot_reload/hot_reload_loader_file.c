#include "internal/hot_reload/dcc_hot_reload_loader_file_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

dcc_status_t dcc_hot_reload_copy_file(const char *source, const char *target) {
    FILE *in = fopen(source, "rb");
    if (in == NULL) {
        return DCC_ERR_STATE;
    }
    FILE *out = fopen(target, "wb");
    if (out == NULL) {
        fclose(in);
        return DCC_ERR_STATE;
    }

    unsigned char buffer[16384];
    dcc_status_t status = DCC_OK;
    for (;;) {
        size_t read_count = fread(buffer, 1, sizeof(buffer), in);
        if (read_count > 0 && fwrite(buffer, 1, read_count, out) != read_count) {
            status = DCC_ERR_STATE;
            break;
        }
        if (read_count < sizeof(buffer)) {
            if (ferror(in)) {
                status = DCC_ERR_STATE;
            }
            break;
        }
    }
    if (fclose(out) != 0 && status == DCC_OK) {
        status = DCC_ERR_STATE;
    }
    fclose(in);
    if (status != DCC_OK) {
        remove(target);
    }
    return status;
}

char *dcc_hot_reload_temp_path(const char *path, uint64_t generation) {
    if (path == NULL) {
        return NULL;
    }
#if defined(_WIN32)
    unsigned long pid = GetCurrentProcessId();
    const char *suffix = ".dll";
#else
    unsigned long pid = (unsigned long)getpid();
    const char *suffix = ".so";
#endif
    size_t path_len = strlen(path);
    size_t suffix_len = strlen(suffix);
    size_t cap = path_len + suffix_len + 64U;
    char *out = (char *)malloc(cap);
    if (out == NULL) {
        return NULL;
    }
    int n = snprintf(
        out,
        cap,
        "%s.dcc-hot-%lu-%llu%s",
        path,
        pid,
        (unsigned long long)generation,
        suffix
    );
    if (n <= 0 || (size_t)n >= cap) {
        free(out);
        return NULL;
    }
    return out;
}
