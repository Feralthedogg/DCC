#ifndef DCC_APP_LIFECYCLE_H
#define DCC_APP_LIFECYCLE_H

#include <dcc/app/base.h>
#include <dcc/app/options.h>
#include <dcc/client.h>
#include <dcc/export.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Creates an application that owns its client until destruction. */
DCC_API dcc_status_t dcc_app_create(const dcc_app_options_t *options, dcc_app_t **out);

/**
 * Stops and destroys an application; null is accepted.
 *
 * Returns `DCC_ERR_STATE` without mutation when called from an App-owned
 * callback, cleanup, or any REST terminal callback running on the App's
 * client. Callbacks should request `dcc_app_stop()` and let the owner destroy
 * the application after dispatch returns.
 *
 * `DCC_OK` means the application was consumed. Any non-OK result leaves the
 * application alive and owned by the caller so destruction may be retried.
 */
DCC_API dcc_status_t dcc_app_destroy(dcc_app_t *app);

/** Returns the client borrowed from the application. */
DCC_API dcc_client_t *dcc_app_client(dcc_app_t *app);

/** Starts the application runtime and registered schedules. */
DCC_API dcc_status_t dcc_app_start(dcc_app_t *app);

/**
 * Requests application shutdown without waiting for callbacks or schedules.
 *
 * This operation is idempotent and may be called from any App-owned callback,
 * including scheduled task callbacks. The owner must subsequently call
 * `dcc_app_wait()` or `dcc_app_destroy()` to join and release schedule workers.
 */
DCC_API dcc_status_t dcc_app_stop(dcc_app_t *app);

/**
 * Waits for a started application to stop and reaps schedule workers.
 * Returns `DCC_ERR_STATE` without mutation from an App-owned callback or any
 * REST terminal callback running on the App's client.
 */
DCC_API dcc_status_t dcc_app_wait(dcc_app_t *app);

/** Starts and waits for an application. */
DCC_API dcc_status_t dcc_app_run(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
