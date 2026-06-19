#include "internal/events/dcc_interaction_listener_internal.h"

#include <stdlib.h>

void dcc_interaction_listeners_lock(dcc_client_t *client) {
    while (atomic_flag_test_and_set_explicit(&client->interaction_listeners_lock, memory_order_acquire)) {
    }
}

void dcc_interaction_listeners_unlock(dcc_client_t *client) {
    atomic_flag_clear_explicit(&client->interaction_listeners_lock, memory_order_release);
}

void dcc_interaction_listener_free(dcc_interaction_listener_ctx_t *ctx) {
    if (ctx == NULL) {
        return;
    }
    free(ctx->key);
    free(ctx);
}
