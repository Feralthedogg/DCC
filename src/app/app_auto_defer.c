#include "internal/app/dcc_app_internal.h"

#include "internal/client/dcc_client_state_internal.h"
#include "internal/dcc_core_internal.h"
#include "internal/runtime/dcc_runtime_internal.h"

#include <llam/runtime.h>

#include <stdlib.h>

static void dcc_app_auto_defer_release(dcc_app_auto_defer_t *state) {
    if (state == NULL) {
        return;
    }
    if (atomic_fetch_sub_explicit(&state->refs, 1U, memory_order_acq_rel) == 1U) {
        free(state->token);
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
        int expected = DCC_APP_RESPONSE_READY;
        if (atomic_compare_exchange_strong_explicit(
                &state->response_state,
                &expected,
                DCC_APP_RESPONSE_DEFERRED,
                memory_order_acq_rel,
                memory_order_acquire
            )) {
            dcc_status_t status = state->ephemeral
                ? dcc_interaction_defer_ephemeral(state->client, &state->interaction, NULL, NULL)
                : dcc_interaction_defer(state->client, &state->interaction, NULL, NULL);
            if (status != DCC_OK) {
                atomic_store_explicit(&state->response_state, DCC_APP_RESPONSE_FAILED, memory_order_release);
            }
        }
    }

    dcc_app_auto_defer_release(state);
}

dcc_status_t dcc_app_auto_defer_start(dcc_ctx_t *ctx) {
    if (ctx == NULL || ctx->app == NULL || ctx->app->auto_defer_after_ms == 0U) {
        return DCC_OK;
    }
    if (ctx->client == NULL || ctx->interaction == NULL ||
        ctx->interaction->id == 0U || ctx->interaction->token == NULL) {
        return DCC_OK;
    }

    dcc_app_auto_defer_t *state = (dcc_app_auto_defer_t *)calloc(1U, sizeof(*state));
    if (state == NULL) {
        return DCC_ERR_NOMEM;
    }
    state->token = dcc_strdup(ctx->interaction->token);
    if (state->token == NULL) {
        free(state);
        return DCC_ERR_NOMEM;
    }

    state->client = ctx->client;
    state->interaction = *ctx->interaction;
    state->interaction.token = state->token;
    atomic_init(&state->refs, 2U);
    atomic_init(&state->done, false);
    atomic_init(&state->response_state, DCC_APP_RESPONSE_READY);
    state->after_ms = ctx->app->auto_defer_after_ms;
    state->ephemeral = ctx->app->auto_defer_ephemeral != 0U;

    dcc_status_t status = dcc_runtime_spawn(&ctx->client->runtime, dcc_app_auto_defer_task, state);
    if (status != DCC_OK) {
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

dcc_app_response_state_t dcc_app_auto_defer_response_state(const dcc_ctx_t *ctx) {
    if (ctx == NULL || ctx->auto_defer == NULL) {
        return DCC_APP_RESPONSE_READY;
    }
    return (dcc_app_response_state_t)atomic_load_explicit(
        &ctx->auto_defer->response_state,
        memory_order_acquire
    );
}

dcc_status_t dcc_app_auto_defer_claim_initial(dcc_ctx_t *ctx, dcc_app_response_state_t state) {
    if (ctx == NULL || ctx->auto_defer == NULL) {
        return DCC_OK;
    }
    int expected = DCC_APP_RESPONSE_READY;
    return atomic_compare_exchange_strong_explicit(
        &ctx->auto_defer->response_state,
        &expected,
        state,
        memory_order_acq_rel,
        memory_order_acquire
    ) ? DCC_OK : DCC_ERR_STATE;
}

void dcc_app_auto_defer_mark(dcc_ctx_t *ctx, dcc_app_response_state_t state, dcc_status_t status) {
    if (ctx == NULL || ctx->auto_defer == NULL) {
        return;
    }
    atomic_store_explicit(
        &ctx->auto_defer->response_state,
        status == DCC_OK ? state : DCC_APP_RESPONSE_FAILED,
        memory_order_release
    );
}
