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
 * callback or cleanup. Callbacks should request `dcc_app_stop()` and let the
 * owner destroy the application after dispatch returns.
 */
DCC_API dcc_status_t dcc_app_destroy(dcc_app_t *app);

/** Returns the client borrowed from the application. */
DCC_API dcc_client_t *dcc_app_client(dcc_app_t *app);

/** Starts the application runtime and registered schedules. */
DCC_API dcc_status_t dcc_app_start(dcc_app_t *app);

/** Requests application shutdown and stops registered schedules. */
DCC_API dcc_status_t dcc_app_stop(dcc_app_t *app);

/** Waits for a started application to stop. */
DCC_API dcc_status_t dcc_app_wait(dcc_app_t *app);

/** Starts and waits for an application. */
DCC_API dcc_status_t dcc_app_run(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
