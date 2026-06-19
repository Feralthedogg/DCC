#include "internal/replay/dcc_replay_internal.h"

#include <stdlib.h>

dcc_status_t dcc_replay_read_line(FILE *file, char **out_line, size_t *out_len) {
    if (file == NULL || out_line == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out_line = NULL;
    if (out_len != NULL) {
        *out_len = 0;
    }

    size_t cap = 256U;
    char *line = (char *)malloc(cap);
    if (line == NULL) {
        return DCC_ERR_NOMEM;
    }
    size_t len = 0;
    int ch = EOF;
    while ((ch = fgetc(file)) != EOF) {
        if (len + 1U >= cap) {
            if (cap > SIZE_MAX / 2U) {
                free(line);
                return DCC_ERR_NOMEM;
            }
            size_t next_cap = cap * 2U;
            char *next = (char *)realloc(line, next_cap);
            if (next == NULL) {
                free(line);
                return DCC_ERR_NOMEM;
            }
            line = next;
            cap = next_cap;
        }
        line[len++] = (char)ch;
        if (ch == '\n') {
            break;
        }
    }
    if (len == 0U && ch == EOF) {
        free(line);
        return DCC_ERR_CANCELED;
    }
    line[len] = '\0';
    *out_line = line;
    if (out_len != NULL) {
        *out_len = len;
    }
    return DCC_OK;
}
