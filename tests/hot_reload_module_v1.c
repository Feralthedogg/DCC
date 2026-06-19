#include <dcc/hot_reload.h>

#include "hot_reload_test_state.h"

static void hot_reload_v1_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    if (user_data != NULL) {
        hot_reload_shared_state_t *shared = (hot_reload_shared_state_t *)user_data;
        shared->ready_count++;
        if (shared->test_state != NULL) {
            atomic_fetch_add_explicit(&shared->test_state->ready_v1, 1U, memory_order_acq_rel);
            atomic_store_explicit(&shared->test_state->shared_ready_after_v1, shared->ready_count, memory_order_release);
        }
    }
}

DCC_MODULE_API dcc_status_t dcc_bot_module_init(dcc_bot_module_ctx_t *ctx) {
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)dcc_bot_module_user_data(ctx);
    if (state == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    hot_reload_shared_state_t *shared =
        (hot_reload_shared_state_t *)dcc_bot_module_state(ctx, sizeof(*shared));
    if (shared == NULL) {
        return DCC_ERR_NOMEM;
    }
    if (shared->marker == 0) {
        shared->marker = HOT_RELOAD_SHARED_MARKER;
    }
    if (shared->marker != HOT_RELOAD_SHARED_MARKER) {
        return DCC_ERR_STATE;
    }
    shared->test_state = state;
    atomic_fetch_add_explicit(&state->init_v1, 1U, memory_order_acq_rel);
    atomic_store_explicit(&state->shared_init_v1, shared->ready_count, memory_order_release);
    return dcc_bot_module_on(ctx, DCC_EVENT_READY, hot_reload_v1_ready, shared);
}

DCC_MODULE_API void dcc_bot_module_deinit(dcc_bot_module_ctx_t *ctx) {
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)dcc_bot_module_user_data(ctx);
    if (state != NULL) {
        atomic_fetch_add_explicit(&state->deinit_v1, 1U, memory_order_acq_rel);
    }
}
