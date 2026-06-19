#ifndef DCC_REST_THREADS_INTERNAL_H
#define DCC_REST_THREADS_INTERNAL_H

#include "internal/rest/dcc_rest_builders_internal.h"
#include "internal/rest/dcc_rest_json_internal.h"
#include "internal/rest/dcc_rest_paths_internal.h"
#include "internal/rest/dcc_rest_request_internal.h"

#include <dcc/dcc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum dcc_rest_thread_body_mode {
    DCC_REST_THREAD_BODY_CREATE = 0,
    DCC_REST_THREAD_BODY_CREATE_FROM_MESSAGE,
    DCC_REST_THREAD_BODY_CREATE_FORUM,
    DCC_REST_THREAD_BODY_MODIFY
} dcc_rest_thread_body_mode_t;

dcc_status_t dcc_rest_build_thread_body(
    const dcc_thread_params_t *params,
    dcc_rest_thread_body_mode_t mode,
    char **out
);

#ifdef __cplusplus
}
#endif

#endif
