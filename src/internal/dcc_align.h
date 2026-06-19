#ifndef DCC_ALIGN_H
#define DCC_ALIGN_H

#include <stddef.h>

#if defined(_MSC_VER)
typedef __declspec(align(16)) union dcc_max_align {
    long long integer_value;
    long double float_value;
    double double_value;
    void *pointer_value;
    void (*function_value)(void);
} dcc_max_align_t;
#else
typedef max_align_t dcc_max_align_t;
#endif

#define DCC_MAX_ALIGNMENT ((size_t)_Alignof(dcc_max_align_t))

#endif
