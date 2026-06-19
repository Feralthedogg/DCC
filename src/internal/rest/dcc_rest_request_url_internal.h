#ifndef DCC_REST_REQUEST_URL_INTERNAL_H
#define DCC_REST_REQUEST_URL_INTERNAL_H

#include <dcc/error.h>

#ifdef __cplusplus
extern "C" {
#endif

dcc_status_t dcc_rest_build_url(const char *path, char **out_url, int *out_absolute);

#ifdef __cplusplus
}
#endif

#endif
