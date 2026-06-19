#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

#include <errno.h>

dcc_status_t dcc_runtime_run(dcc_runtime_t *runtime) {
    if (runtime == NULL || !runtime->initialized) {
        return DCC_ERR_INVALID_ARG;
    }
    if (llam_run() != 0) {
        return errno == ECANCELED ? DCC_ERR_CANCELED : DCC_ERR_RUNTIME;
    }
    return DCC_OK;
}

dcc_status_t dcc_runtime_request_stop(dcc_runtime_t *runtime) {
    if (runtime == NULL || !runtime->initialized) {
        return DCC_OK;
    }
    if (llam_runtime_request_stop() != 0) {
        return DCC_ERR_RUNTIME;
    }
    return DCC_OK;
}
