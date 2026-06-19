#ifndef DCC_CLIENT_RUNTIME_STATE_FIELDS_INTERNAL_H
#define DCC_CLIENT_RUNTIME_STATE_FIELDS_INTERNAL_H

#include "internal/cache/dcc_cache_state_internal.h"
#include "internal/events/dcc_event_state_internal.h"
#include "internal/events/dcc_interaction_listener_state_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <stdatomic.h>

#define DCC_CLIENT_RUNTIME_STATE_FIELDS \
    dcc_runtime_t runtime; \
    dcc_event_bus_t events; \
    atomic_flag interaction_listeners_lock; \
    dcc_interaction_listener_ctx_t *interaction_listeners; \
    dcc_cache_t cache

#endif
