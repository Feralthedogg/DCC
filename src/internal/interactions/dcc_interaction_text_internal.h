#ifndef DCC_INTERACTION_TEXT_INTERNAL_H
#define DCC_INTERACTION_TEXT_INTERNAL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void dcc_interaction_copy_text(char *dst, size_t dst_size, const char *src);
int dcc_interaction_ascii_eq_n(const char *left, size_t left_len, const char *right);
int dcc_interaction_ascii_eq(const char *left, const char *right);

#ifdef __cplusplus
}
#endif

#endif
