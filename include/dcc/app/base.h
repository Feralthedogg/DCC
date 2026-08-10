#ifndef DCC_APP_BASE_H
#define DCC_APP_BASE_H

#include <dcc/events.h>
#include <dcc/export.h>
#include <dcc/message.h>
#include <dcc/error.h>
#include <dcc/error_details.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_app dcc_app_t;
typedef struct dcc_ctx dcc_ctx_t;

typedef dcc_status_t (*dcc_app_handler_fn)(dcc_ctx_t *ctx, void *user_data);
typedef dcc_status_t (*dcc_app_typed_handler_fn)(dcc_ctx_t *ctx, void *args, void *user_data);
typedef dcc_status_t (*dcc_app_event_fn)(dcc_app_t *app, const dcc_event_t *event, void *user_data);
typedef dcc_status_t (*dcc_app_ready_fn)(dcc_app_t *app, const dcc_ready_event_t *ready, void *user_data);
typedef dcc_status_t (*dcc_app_message_fn)(
    dcc_app_t *app,
    const dcc_message_t *message,
    const dcc_event_t *event,
    void *user_data
);
typedef dcc_status_t (*dcc_app_message_command_fn)(
    dcc_app_t *app,
    const dcc_message_t *message,
    const char *args,
    const dcc_event_t *event,
    void *user_data
);
typedef dcc_status_t (*dcc_app_task_fn)(dcc_app_t *app, void *user_data);
typedef void (*dcc_app_cleanup_fn)(void *user_data);
typedef dcc_status_t (*dcc_app_middleware_fn)(dcc_ctx_t *ctx, void *user_data);
typedef dcc_status_t (*dcc_app_check_fn)(dcc_ctx_t *ctx, void *user_data);
typedef dcc_status_t (*dcc_app_module_setup_fn)(dcc_app_t *app, void *user_data);

/**
 * Receives one borrowed structured App error.
 *
 * The view and all pointed-to data remain valid only until the callback
 * returns. Replacing or clearing an observer does not revoke a callback that
 * another thread already copied, so old user data must outlive in-flight
 * calls. The observer may clear or replace itself. App destruction from this
 * callback is rejected; request stop and destroy from an owner thread.
 */
typedef void (*dcc_app_error_fn)(
    dcc_app_t *app,
    const dcc_error_t *error,
    void *user_data
);

/**
 * Installs or clears the App structured error observer.
 *
 * `(NULL, NULL)` clears the observer. A NULL callback with non-NULL user data
 * is invalid. Telemetry registration is independent from default responses.
 */
DCC_API dcc_status_t dcc_app_on_error(
    dcc_app_t *app,
    dcc_app_error_fn handler,
    void *user_data
);

/**
 * Enables the safe default interaction failure response policy.
 *
 * Before a response starts, handler failure queues one fixed generic
 * ephemeral reply. It never exposes handler, Discord, body, or token details.
 */
DCC_API dcc_status_t dcc_app_use_default_error_responses(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
