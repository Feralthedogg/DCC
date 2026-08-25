#include "internal/app/dcc_app_internal.h"

#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/app/dcc_app_interaction_queue_internal.h"
#include "internal/interaction_flow/dcc_interaction_flow_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

#include <stdlib.h>

static void dcc_app_auto_defer_release(dcc_app_auto_defer_t *state) {
    if (state == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&state->refs, 1U, memory_order_acq_rel) == 1U) {
        free(state);
    }
}

static void dcc_app_auto_defer_task(void *arg) {
    dcc_app_auto_defer_t *state = (dcc_app_auto_defer_t *)arg;
    if (state == NULL) {
        return;
    }

    (void)llam_sleep_ns(state->after_ms * UINT64_C(1000000));
    if (!atomic_load_explicit(&state->done, memory_order_acquire)) {
        (void)(state->ephemeral
                   ? dcc_flow_defer_ephemeral(state->flow, NULL, NULL)
                   : dcc_flow_defer(state->flow, NULL, NULL));
    }

    dcc_app_interaction_queue_release_timer(state->flow->queue);
    dcc_app_auto_defer_release(state);
}

dcc_status_t dcc_app_auto_defer_start(dcc_ctx_t *ctx) {
  if (ctx == NULL || ctx->app == NULL || ctx->app->auto_defer_after_ms == 0U) {
        return DCC_OK;
  }
  if (ctx->app->auto_defer_after_ms > UINT64_MAX / UINT64_C(1000000)) {
    return DCC_ERR_INVALID_ARG;
  }
    if (ctx->client == NULL || ctx->interaction == NULL || ctx->flow == NULL ||
        ctx->interaction->id == 0U || ctx->interaction->token == NULL) {
        return DCC_OK;
    }

    dcc_app_auto_defer_t *state = (dcc_app_auto_defer_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status =
        dcc_app_interaction_queue_retain_timer(ctx->flow->queue);
    if (status != DCC_OK) {
        free(state);
        return status;
    }

    state->flow = ctx->flow;
    atomic_init(&state->refs, 2U);
    atomic_init(&state->done, false);
    state->after_ms = ctx->app->auto_defer_after_ms;
    state->ephemeral = ctx->app->auto_defer_ephemeral != 0U;

    status = dcc_runtime_spawn(&ctx->client->runtime, dcc_app_auto_defer_task, state);
    if (status != DCC_OK) {
        dcc_app_interaction_queue_release_timer(ctx->flow->queue);
        dcc_app_auto_defer_release(state);
        dcc_app_auto_defer_release(state);
        return status;
    }
    ctx->auto_defer = state;
    return DCC_OK;
}

void dcc_app_auto_defer_finish(dcc_ctx_t *ctx) {
    if (ctx == NULL || ctx->auto_defer == NULL) {
        return;
    }
    atomic_store_explicit(&ctx->auto_defer->done, true, memory_order_release);
    dcc_app_auto_defer_release(ctx->auto_defer);
    ctx->auto_defer = NULL;
}
