#include "internal/runtime/dcc_task_group_internal.h"

#include <errno.h>

dcc_status_t dcc_task_group_status_from_errno(int err) {
    switch (err) {
        case 0:
            return DCC_OK;
        case EINVAL:
            return DCC_ERR_INVALID_ARG;
        case ENOMEM:
            return DCC_ERR_NOMEM;
        case ETIMEDOUT:
        case EAGAIN:
            return DCC_ERR_TIMEOUT;
        case ECANCELED:
        case EPIPE:
            return DCC_ERR_CANCELED;
        case EBUSY:
        case ENOTSUP:
        case EXDEV:
            return DCC_ERR_STATE;
        default:
            return DCC_ERR_RUNTIME;
    }
}

dcc_status_t dcc_task_group_wait_status_from_errno(int err) {
    if (err == 0) {
        return DCC_OK;
    }
    return dcc_task_group_status_from_errno(err);
}
