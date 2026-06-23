#include "internal/app/dcc_app_internal.h"

dcc_status_t dcc_app_set_state(
    dcc_app_t *app,
    void *state,
    dcc_app_cleanup_fn cleanup
) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (app->state != NULL && app->state_cleanup != NULL && app->state != state) {
        app->state_cleanup(app->state);
    }
    app->state = state;
    app->state_cleanup = cleanup;
    return DCC_OK;
}

void *dcc_app_state(const dcc_app_t *app) {
    return app != NULL ? app->state : NULL;
}

void dcc_app_clear_state(dcc_app_t *app) {
    if (app == NULL) {
        return;
    }
    if (app->state != NULL && app->state_cleanup != NULL) {
        app->state_cleanup(app->state);
    }
    app->state = NULL;
    app->state_cleanup = NULL;
}

void *dcc_ctx_state(const dcc_ctx_t *ctx) {
    return ctx != NULL ? dcc_app_state(ctx->app) : NULL;
}
