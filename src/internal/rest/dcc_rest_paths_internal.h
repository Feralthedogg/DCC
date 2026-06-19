#ifndef DCC_REST_PATHS_INTERNAL_H
#define DCC_REST_PATHS_INTERNAL_H

#include <dcc/dcc.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_format_path(char *out, size_t out_size, const char *fmt, ...);
dcc_status_t dcc_rest_alloc_formatted_path(char **out, const char *fmt, ...);
dcc_status_t dcc_rest_escape_path_segment(const char *value, char **out);

#ifdef __cplusplus
}
#endif

#endif
