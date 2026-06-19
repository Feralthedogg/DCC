#ifndef DCC_CACHE_GUARD_INTERNAL_H
#define DCC_CACHE_GUARD_INTERNAL_H

#include "internal/client/dcc_client_state_internal.h"

#include <dcc/dcc.h>

static inline dcc_status_t dcc_cache_require_enabled(const dcc_client_t *client) {
    if (client == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    return client->enable_cache ? DCC_OK : DCC_ERR_STATE;
}

#endif
