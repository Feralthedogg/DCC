#ifndef DCC_APP_MODULES_H
#define DCC_APP_MODULES_H

#include <dcc/app/base.h>
#include <dcc/export.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dcc_app_module {
    size_t size;
    const char *name;
    dcc_app_module_setup_fn setup;
    void *user_data;
    dcc_app_cleanup_fn cleanup;
} dcc_app_module_t;

/** Registers and immediately sets up one module. */
DCC_API dcc_status_t dcc_app_module_register(dcc_app_t *app, const dcc_app_module_t *module);

/** Returns the number of successfully registered modules. */
DCC_API size_t dcc_app_module_count(const dcc_app_t *app);

#ifdef __cplusplus
}
#endif

#endif
