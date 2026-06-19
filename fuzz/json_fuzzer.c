#include "internal/json/dcc_json.h"

#include <stddef.h>
#include <stdint.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (data == NULL) {
        return 0;
    }

    const char *input = (const char *)(const void *)data;

    (void)dcc_json_stage1_validate(input, size);

    dcc_json_t *root = NULL;
    if (dcc_json_parse(input, size, &root) == DCC_OK) {
        dcc_json_free(root);
    } else if (root != NULL) {
        dcc_json_free(root);
    }

    static _Thread_local dcc_json_gateway_payload_t payload;
    (void)dcc_json_parse_gateway_payload(input, size, &payload);
    return 0;
}

#if defined(DCC_FUZZ_STANDALONE)
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int dcc_fuzz_one_file(const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        perror(path);
        return 1;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        perror(path);
        fclose(file);
        return 1;
    }
    long size = ftell(file);
    if (size < 0) {
        perror(path);
        fclose(file);
        return 1;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror(path);
        fclose(file);
        return 1;
    }

    uint8_t *data = NULL;
    if (size > 0) {
        data = (uint8_t *)malloc((size_t)size);
        if (data == NULL) {
            fclose(file);
            return 1;
        }
        if (fread(data, 1, (size_t)size, file) != (size_t)size) {
            perror(path);
            free(data);
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    LLVMFuzzerTestOneInput(data != NULL ? data : (const uint8_t *)"", (size_t)size);
    free(data);
    return 0;
}

static int dcc_fuzz_path(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        perror(path);
        return 1;
    }

    if (!S_ISDIR(st.st_mode)) {
        return dcc_fuzz_one_file(path);
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        return 1;
    }

    int failed = 0;
    struct dirent *entry = NULL;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        size_t path_len = strlen(path);
        size_t name_len = strlen(entry->d_name);
        if (path_len > (size_t)-1 - name_len - 2U) {
            failed = 1;
            break;
        }
        char *child = (char *)malloc(path_len + name_len + 2U);
        if (child == NULL) {
            failed = 1;
            break;
        }
        memcpy(child, path, path_len);
        child[path_len] = '/';
        memcpy(child + path_len + 1U, entry->d_name, name_len + 1U);
        if (dcc_fuzz_path(child) != 0) {
            failed = 1;
        }
        free(child);
    }
    closedir(dir);
    return failed;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <corpus-file-or-directory>...\n", argv[0]);
        return 2;
    }

    int failed = 0;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            continue;
        }
        if (dcc_fuzz_path(argv[i]) != 0) {
            failed = 1;
        }
    }
    return failed;
}
#endif
