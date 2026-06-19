#ifndef DCC_JSON_CAPACITY_H
#define DCC_JSON_CAPACITY_H

#include <stddef.h>

static inline int dcc_json_next_cap(size_t current, size_t initial, size_t elem_size, size_t *out) {
    if (out == NULL || initial == 0 || elem_size == 0) {
        return -1;
    }
    size_t next = initial;
    if (current != 0) {
        if (current > (size_t)-1 / 2U) {
            return -1;
        }
        next = current * 2U;
    }
    if (next > (size_t)-1 / elem_size) {
        return -1;
    }
    *out = next;
    return 0;
}

#endif
