#include <dcc/hot_reload.h>

#include "hot_reload_test_state.h"

static void hot_reload_v2_ready(dcc_client_t *client, const dcc_event_t *event, void *user_data) {
    (void)client;
    (void)event;
    if (user_data != NULL) {
        hot_reload_shared_state_t *shared = (hot_reload_shared_state_t *)user_data;
        shared->ready_count++;
        if (shared->test_state != NULL) {
            atomic_fetch_add_explicit(&shared->test_state->ready_v2, 1U, memory_order_acq_rel);
            atomic_store_explicit(&shared->test_state->shared_ready_after_v2, shared->ready_count, memory_order_release);
        }
    }
}

static dcc_status_t hot_reload_v2_load(dcc_bot_module_ctx_t *ctx, void *user_data) {
    hot_reload_shared_state_t *shared = (hot_reload_shared_state_t *)user_data;
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)dcc_bot_module_user_data(ctx);
    if (state == NULL ||
        shared == NULL ||
        dcc_bot_module_client(ctx) == NULL ||
        dcc_bot_module_generation(ctx) == 0) {
        return DCC_ERR_INVALID_ARG;
    }
    if (shared->marker != HOT_RELOAD_SHARED_MARKER) {
        return DCC_ERR_STATE;
    }
    shared->test_state = state;
    atomic_fetch_add_explicit(&state->init_v2, 1U, memory_order_acq_rel);
    atomic_store_explicit(&state->shared_seen_v2, shared->ready_count, memory_order_release);
    return DCC_OK;
}

static void hot_reload_v2_unload(dcc_bot_module_ctx_t *ctx, void *user_data) {
    (void)user_data;
    hot_reload_test_state_t *state = (hot_reload_test_state_t *)dcc_bot_module_user_data(ctx);
    if (state != NULL) {
        atomic_fetch_add_explicit(&state->deinit_v2, 1U, memory_order_acq_rel);
    }
}

static const dcc_bot_module_handler_t hot_reload_v2_handlers[] = {
    DCC_BOT_MODULE_ON(DCC_EVENT_READY, hot_reload_v2_ready),
};

DCC_BOT_MODULE_WITH_STATE(
    hot_reload_shared_state_t,
    hot_reload_v2_handlers,
    hot_reload_v2_load,
    hot_reload_v2_unload
)
