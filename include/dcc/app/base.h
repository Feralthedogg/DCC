#ifndef DCC_APP_BASE_H
#define DCC_APP_BASE_H

#include <dcc/events.h>
#include <dcc/export.h>
#include <dcc/message.h>
#include <dcc/error.h>

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

/* Temporary DCC 1-style observer; Task 3 replaces it with structured errors. */
typedef void (*dcc_app_error_fn)(
    dcc_ctx_t *ctx,
    dcc_status_t status,
    const char *message,
    void *user_data
);

/** Installs the temporary application error observer. */
DCC_API dcc_status_t dcc_app_on_error(
    dcc_app_t *app,
    dcc_app_error_fn handler,
    void *user_data
);

/** Installs the default temporary interaction error observer. */
DCC_API dcc_status_t dcc_app_use_default_error_responses(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
