#include "internal/dcc_new_app.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if defined(_WIN32)
#include <direct.h>
#include <sys/stat.h>
#define DCC_NEW_APP_MKDIR(path_) _mkdir(path_)
#define DCC_NEW_APP_STAT _stat
#define DCC_NEW_APP_STAT_T struct _stat
#else
#include <sys/stat.h>
#define DCC_NEW_APP_MKDIR(path_) mkdir((path_), 0775)
#define DCC_NEW_APP_STAT stat
#define DCC_NEW_APP_STAT_T struct stat
#endif

int dcc_new_app_path_exists(const char *path) {
    DCC_NEW_APP_STAT_T st;
    return DCC_NEW_APP_STAT(path, &st) == 0;
}

static int dcc_new_app_mkdir_one(const char *path) {
    if (path == NULL || path[0] == '\0') {
        return 0;
    }
    if (DCC_NEW_APP_MKDIR(path) == 0 || errno == EEXIST) {
        return 0;
    }
    fprintf(stderr, "could not create directory %s: %s\n", path, strerror(errno));
    return -1;
}

int dcc_new_app_mkdirs(const char *path) {
    size_t len = strlen(path);
    char *copy = (char *)malloc(len + 1U);
    if (copy == NULL) {
        return -1;
    }
    memcpy(copy, path, len + 1U);

    for (size_t i = 0U; i < len; ++i) {
        if (copy[i] != '/' && copy[i] != '\\') {
            continue;
        }
        if (i == 0U || (i == 2U && copy[1U] == ':')) {
            continue;
        }
        copy[i] = '\0';
        if (dcc_new_app_mkdir_one(copy) != 0) {
            free(copy);
            return -1;
        }
        copy[i] = path[i];
    }

    int status = dcc_new_app_mkdir_one(copy);
    free(copy);
    return status;
}

char *dcc_new_app_join(const char *lhs, const char *rhs) {
    size_t lhs_len = strlen(lhs);
    size_t rhs_len = strlen(rhs);
    int need_sep = lhs_len > 0U && lhs[lhs_len - 1U] != '/' && lhs[lhs_len - 1U] != '\\';
    size_t total = lhs_len + (need_sep ? 1U : 0U) + rhs_len + 1U;
    char *path = (char *)malloc(total);
    if (path == NULL) {
        return NULL;
    }
    memcpy(path, lhs, lhs_len);
    size_t offset = lhs_len;
    if (need_sep) {
        path[offset++] = '/';
    }
    memcpy(path + offset, rhs, rhs_len);
    path[offset + rhs_len] = '\0';
    return path;
}

void dcc_new_app_make_guard(const char *name, char *out, size_t out_len) {
    size_t written = 0U;
    const char prefix[] = "DCC_GENERATED_";
    const char suffix[] = "_H";
    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';
    for (size_t i = 0U; prefix[i] != '\0' && written + 1U < out_len; ++i) {
        out[written++] = prefix[i];
    }
    for (const char *p = name; p != NULL && *p != '\0' && written + 1U < out_len; ++p) {
        unsigned char ch = (unsigned char)*p;
        out[written++] = (char)(isalnum(ch) ? toupper(ch) : '_');
    }
    for (size_t i = 0U; suffix[i] != '\0' && written + 1U < out_len; ++i) {
        out[written++] = suffix[i];
    }
    out[written] = '\0';
}

void dcc_new_app_make_macro_name(const char *name, const char *suffix, char *out, size_t out_len) {
    size_t written = 0U;
    if (out_len == 0U) {
        return;
    }
    out[0] = '\0';
    for (const char *p = name; p != NULL && *p != '\0' && written + 1U < out_len; ++p) {
        unsigned char ch = (unsigned char)*p;
        out[written++] = (char)(isalnum(ch) ? toupper(ch) : '_');
    }
    if (written == 0U && written + 1U < out_len) {
        out[written++] = 'X';
    }
    for (size_t i = 0U; suffix != NULL && suffix[i] != '\0' && written + 1U < out_len; ++i) {
        unsigned char ch = (unsigned char)suffix[i];
        out[written++] = (char)(isalnum(ch) ? toupper(ch) : '_');
    }
    out[written] = '\0';
}

void dcc_new_app_make_subcommand_symbol_base(
    const dcc_new_app_options_t *options,
    char *out,
    size_t out_len
) {
    if (out_len == 0U) {
        return;
    }
    if (options == NULL) {
        out[0] = '\0';
        return;
    }
    snprintf(
        out,
        out_len,
        "on_%s_%s_%s",
        options->cog_name,
        options->command_name,
        options->subcommand_name
    );
}

FILE *dcc_new_app_open_output(const char *path, uint8_t force) {
    if (!force && dcc_new_app_path_exists(path)) {
        fprintf(stderr, "%s already exists; pass --force to overwrite generated files\n", path);
        return NULL;
    }
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "could not write %s: %s\n", path, strerror(errno));
    }
    return file;
}

char *dcc_new_app_read_file(const char *path, size_t *out_len) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        return NULL;
    }
    if (fseek(file, 0L, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }
    long size = ftell(file);
    if (size < 0L || fseek(file, 0L, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }
    char *data = (char *)malloc((size_t)size + 1U);
    if (data == NULL) {
        fclose(file);
        return NULL;
    }
    size_t read_len = fread(data, 1U, (size_t)size, file);
    fclose(file);
    if (read_len != (size_t)size) {
        free(data);
        return NULL;
    }
    data[read_len] = '\0';
    if (out_len != NULL) {
        *out_len = read_len;
    }
    return data;
}

int dcc_new_app_write_file_all(const char *path, const char *data, size_t len) {
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        fprintf(stderr, "could not write %s: %s\n", path, strerror(errno));
        return -1;
    }
    int write_ok = fwrite(data, 1U, len, file) == len;
    int close_ok = fclose(file) == 0;
    return write_ok && close_ok ? 0 : -1;
}

int dcc_new_app_replace_range(
    const char *path,
    const char *data,
    size_t len,
    const char *start,
    size_t remove_len,
    const char *replacement
) {
    size_t prefix_len = (size_t)(start - data);
    size_t replacement_len = strlen(replacement);
    size_t suffix_offset = prefix_len + remove_len;
    if (prefix_len > len || suffix_offset > len) {
        return -1;
    }
    size_t suffix_len = len - suffix_offset;
    if (replacement_len > SIZE_MAX - prefix_len ||
        suffix_len > SIZE_MAX - prefix_len - replacement_len) {
        return -1;
    }
    size_t total = prefix_len + replacement_len + suffix_len;
    char *next = (char *)malloc(total + 1U);
    if (next == NULL) {
        return -1;
    }
    memcpy(next, data, prefix_len);
    memcpy(next + prefix_len, replacement, replacement_len);
    memcpy(next + prefix_len + replacement_len, data + suffix_offset, suffix_len);
    next[total] = '\0';
    int status = dcc_new_app_write_file_all(path, next, total);
    free(next);
    return status;
}

int dcc_new_app_insert_before_marker(
    const char *path,
    const char *marker,
    const char *insertion,
    const char *duplicate_needle
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }
    if (duplicate_needle != NULL && strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }
    char *where = strstr(data, marker);
    if (where == NULL) {
        fprintf(stderr, "warning: marker %s not found in %s; generated file was not wired automatically\n", marker, path);
        free(data);
        return 0;
    }

    size_t prefix_len = (size_t)(where - data);
    size_t insertion_len = strlen(insertion);
    size_t total = len + insertion_len;
    char *next = (char *)malloc(total + 1U);
    if (next == NULL) {
        free(data);
        return -1;
    }
    memcpy(next, data, prefix_len);
    memcpy(next + prefix_len, insertion, insertion_len);
    memcpy(next + prefix_len + insertion_len, where, len - prefix_len);
    next[total] = '\0';
    int status = dcc_new_app_write_file_all(path, next, total);
    free(next);
    free(data);
    return status;
}

int dcc_new_app_insert_before_marker_compat(
    const char *path,
    const char *marker,
    const char *legacy_marker,
    const char *insertion,
    const char *duplicate_needle
) {
    size_t len = 0U;
    char *data = dcc_new_app_read_file(path, &len);
    if (data == NULL) {
        fprintf(stderr, "could not read %s for marker update\n", path);
        return -1;
    }
    if (duplicate_needle != NULL && strstr(data, duplicate_needle) != NULL) {
        free(data);
        return 0;
    }

    char *where = marker != NULL ? strstr(data, marker) : NULL;
    const char *used_marker = marker;
    if (where == NULL && legacy_marker != NULL) {
        where = strstr(data, legacy_marker);
        used_marker = legacy_marker;
    }
    if (where == NULL) {
        fprintf(
            stderr,
            "warning: marker %s%s%s not found in %s; generated file was not wired automatically\n",
            marker != NULL ? marker : "(null)",
            legacy_marker != NULL ? " or " : "",
            legacy_marker != NULL ? legacy_marker : "",
            path
        );
        free(data);
        return 0;
    }
    (void)used_marker;

    size_t prefix_len = (size_t)(where - data);
    size_t insertion_len = strlen(insertion);
    size_t total = len + insertion_len;
    char *next = (char *)malloc(total + 1U);
    if (next == NULL) {
        free(data);
        return -1;
    }
    memcpy(next, data, prefix_len);
    memcpy(next + prefix_len, insertion, insertion_len);
    memcpy(next + prefix_len + insertion_len, where, len - prefix_len);
    next[total] = '\0';
    int status = dcc_new_app_write_file_all(path, next, total);
    free(next);
    free(data);
    return status;
}
