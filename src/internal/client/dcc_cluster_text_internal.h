#ifndef DCC_CLUSTER_TEXT_INTERNAL_H
#define DCC_CLUSTER_TEXT_INTERNAL_H

#include <stddef.h>
#include <stdio.h>

static inline void dcc_cluster_copy_text(char *dst, size_t dst_size, const char *src) {
    if (dst == NULL || dst_size == 0) {
        return;
    }
    if (src == NULL) {
        src = "";
    }
    snprintf(dst, dst_size, "%s", src);
}

#endif
