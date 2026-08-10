#ifndef DCC_APP_STORE_H
#define DCC_APP_STORE_H

#include <dcc/app/base.h>
#include <dcc/export.h>
#include <dcc/store.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Returns the open store borrowed from the application, or null. */
DCC_API dcc_store_t *dcc_app_store(dcc_app_t *app);

/** Opens a file-backed store owned by the application. */
DCC_API dcc_status_t dcc_app_store_open_file(dcc_app_t *app, const char *path);

/** Closes the application-owned store, if open. */
DCC_API void dcc_app_store_close(dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
