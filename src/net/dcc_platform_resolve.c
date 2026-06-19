#include "internal/dcc_platform_resolve.h"

#if LLAM_PLATFORM_POSIX
#include <llam/runtime.h>

#include <stddef.h>

typedef struct dcc_platform_getaddrinfo_call {
    const char *node;
    const char *service;
    const struct addrinfo *hints;
    struct addrinfo *result;
    int gai_error;
} dcc_platform_getaddrinfo_call_t;

static void *dcc_platform_getaddrinfo_blocking(void *arg) {
    dcc_platform_getaddrinfo_call_t *call = (dcc_platform_getaddrinfo_call_t *)arg;
    call->gai_error = getaddrinfo(call->node, call->service, call->hints, &call->result);
    return NULL;
}

dcc_status_t dcc_platform_getaddrinfo(
    const char *node,
    const char *service,
    const struct addrinfo *hints,
    struct addrinfo **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_platform_getaddrinfo_call_t call = {
        .node = node,
        .service = service,
        .hints = hints,
        .result = NULL,
        .gai_error = 0,
    };

    if (llam_current_task() != NULL) {
        void *ignored = NULL;
        if (llam_call_blocking_result(dcc_platform_getaddrinfo_blocking, &call, &ignored) != 0) {
            return DCC_ERR_NETWORK;
        }
    } else {
        call.gai_error = getaddrinfo(node, service, hints, &call.result);
    }

    if (call.gai_error != 0 || call.result == NULL) {
        dcc_platform_freeaddrinfo(call.result);
        return DCC_ERR_NETWORK;
    }

    *out = call.result;
    return DCC_OK;
}

void dcc_platform_freeaddrinfo(struct addrinfo *result) {
    if (result != NULL) {
        freeaddrinfo(result);
    }
}
#endif
