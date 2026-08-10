#include "internal/app/dcc_app_internal.h"

#include "internal/rest/dcc_rest_error_observer_internal.h"

#include <string.h>

static const char dcc_app_generic_error_text[] =
    "Something went wrong. Please try again.";

typedef struct dcc_app_error_snapshot {
    dcc_app_error_fn handler;
    void *user_data;
    uint8_t default_responses;
} dcc_app_error_snapshot_t;

static dcc_app_error_snapshot_t dcc_app_error_snapshot(dcc_app_t *app) {
    dcc_app_error_snapshot_t snapshot;
    memset(&snapshot, 0, sizeof(snapshot));
    dcc_app_listener_lock(app);
    snapshot.handler = app->error_handler;
    snapshot.user_data = app->error_user_data;
    snapshot.default_responses = app->default_error_responses;
    dcc_app_listener_unlock(app);
    return snapshot;
}

dcc_status_t dcc_app_on_error(
    dcc_app_t *app,
    dcc_app_error_fn handler,
    void *user_data
) {
    if (app == NULL || (handler == NULL && user_data != NULL)) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_lock(app);
    app->error_handler = handler;
    app->error_user_data = user_data;
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

dcc_status_t dcc_app_use_default_error_responses(dcc_app_t *app) {
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_app_listener_lock(app);
    app->default_error_responses = 1U;
    dcc_app_listener_unlock(app);
    return DCC_OK;
}

void dcc_app_notify_error(dcc_app_t *app, const dcc_error_t *error) {
    if (app == NULL || error == NULL || error->size < sizeof(*error) ||
        error->version != DCC_ERROR_VERSION) {
        return;
    }
    dcc_app_error_snapshot_t snapshot = dcc_app_error_snapshot(app);
    if (snapshot.handler != NULL) {
        dcc_app_callback_frame_t frame;
        dcc_app_callback_frame_enter(&frame, app, NULL);
        snapshot.handler(app, error, snapshot.user_data);
        dcc_app_callback_frame_leave(&frame);
    }
}

static void dcc_app_client_error_sink(
    dcc_client_t *client,
    const dcc_error_t *error,
    void *user_data
) {
    (void)client;
    dcc_app_notify_error((dcc_app_t *)user_data, error);
}

dcc_status_t dcc_app_attach_error_sink(dcc_app_t *app) {
    return app != NULL && app->client != NULL
        ? dcc_rest_set_app_error_sink(app->client, dcc_app_client_error_sink, app)
        : DCC_ERR_INVALID_ARG;
}

void dcc_app_detach_error_sink(dcc_app_t *app) {
    if (app != NULL && app->client != NULL) {
        dcc_rest_clear_app_error_sink_and_wait(app->client);
    }
}

dcc_status_t dcc_app_report_handler_error(
    dcc_app_t *app,
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *operation,
    const char *message
) {
    if (status == DCC_OK) {
        return DCC_OK;
    }
    if (app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    dcc_error_t error = {
        .size = sizeof(error),
        .version = DCC_ERROR_VERSION,
        .origin = DCC_ERROR_HANDLER,
        .status = status,
        .operation = operation,
        .message = message,
    };
    dcc_app_error_snapshot_t snapshot = dcc_app_error_snapshot(app);
    if (snapshot.handler != NULL) {
        dcc_app_callback_frame_t frame;
        dcc_app_callback_frame_enter(&frame, app, NULL);
        snapshot.handler(app, &error, snapshot.user_data);
        dcc_app_callback_frame_leave(&frame);
    }
    if (ctx != NULL && snapshot.default_responses &&
        !dcc_ctx_initial_response_admitted(ctx)) {
        (void)dcc_ctx_reply_ephemeral_text(
            ctx,
            dcc_app_generic_error_text,
            NULL,
            NULL
        );
    }
    return status;
}

dcc_status_t dcc_ctx_handle_error(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message
) {
    if (status == DCC_OK) {
        return DCC_OK;
    }
    if (ctx == NULL || ctx->app == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *operation = "interaction handler";
    if (ctx->interaction != NULL && ctx->interaction->name != NULL &&
        ctx->interaction->name[0] != '\0') {
        operation = ctx->interaction->name;
    } else if (ctx->interaction != NULL && ctx->interaction->custom_id != NULL &&
               ctx->interaction->custom_id[0] != '\0') {
        operation = ctx->interaction->custom_id;
    } else if (ctx->event != NULL) {
        operation = dcc_event_type_name(dcc_event_type(ctx->event));
    }
    return dcc_app_report_handler_error(
        ctx->app,
        ctx,
        status,
        operation,
        message
    );
}
