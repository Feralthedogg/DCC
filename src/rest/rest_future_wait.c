#include "internal/rest/dcc_rest_future_internal.h"

#include <stdlib.h>
#include <string.h>

dcc_status_t dcc_rest_future_wait(
    dcc_rest_future_t *future,
    uint32_t timeout_ms,
    dcc_rest_response_t *out
) {
    if (future == NULL || out == NULL || out->size < sizeof(*out)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_status_t status = dcc_rest_future_wait_channel(future, timeout_ms);
    if (status != DCC_OK) {
        return status;
    }

    size_t out_size = out->size;
    memcpy(out, &future->response, sizeof(future->response));
    out->size = out_size;
    return DCC_OK;
}

uint8_t dcc_rest_future_completed(const dcc_rest_future_t *future) {
    return future != NULL && atomic_load_explicit(&future->completed, memory_order_acquire) ? 1U : 0U;
}

dcc_status_t dcc_rest_future_destroy(dcc_rest_future_t *future) {
    if (future == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!atomic_load_explicit(&future->completed, memory_order_acquire)) {
        return DCC_ERR_STATE;
    }
    dcc_rest_future_deinit_completed(future);
    free(future);
    return DCC_OK;
}
