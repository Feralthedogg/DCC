#include "internal/interactions/dcc_interaction_server_internal.h"

#include <stdio.h>
#include <string.h>

static int dcc_interaction_ascii_lower(int ch) {
    return ch >= 'A' && ch <= 'Z' ? ch + ('a' - 'A') : ch;
}

void dcc_interaction_copy_text(char *dst, size_t dst_size, const char *src) {
    if (dst == NULL || dst_size == 0U) {
        return;
    }
    if (src == NULL) {
        src = "";
    }
    snprintf(dst, dst_size, "%s", src);
}

int dcc_interaction_ascii_eq_n(const char *left, size_t left_len, const char *right) {
    size_t right_len = strlen(right);
    if (left == NULL || left_len != right_len) {
        return 0;
    }
    for (size_t i = 0; i < left_len; ++i) {
        if (dcc_interaction_ascii_lower((unsigned char)left[i]) !=
            dcc_interaction_ascii_lower((unsigned char)right[i])) {
            return 0;
        }
    }
    return 1;
}

int dcc_interaction_ascii_eq(const char *left, const char *right) {
    if (left == NULL || right == NULL) {
        return 0;
    }
    return dcc_interaction_ascii_eq_n(left, strlen(left), right);
}
