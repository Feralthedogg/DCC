#ifndef DCC_INTERACTION_BUFFER_INTERNAL_H
#define DCC_INTERACTION_BUFFER_INTERNAL_H

#include <stddef.h>

typedef struct dcc_interaction_buffer {
    char *data;
    size_t len;
    size_t cap;
} dcc_interaction_buffer_t;

#endif
