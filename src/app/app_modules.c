#include "internal/app/dcc_app_internal.h"

#include <stdlib.h>
#include <string.h>

static dcc_status_t dcc_app_modules_reserve(dcc_app_t *app, size_t need) {
    if (need <= app->module_cap) {
        return DCC_OK;
    }
    size_t next_cap = app->module_cap != 0U ? app->module_cap : 4U;
    while (next_cap < need) {
        if (next_cap > SIZE_MAX / 2U) {
            return DCC_ERR_NOMEM;
        }
        next_cap *= 2U;
    }
    if (next_cap > SIZE_MAX / sizeof(*app->modules)) {
        return DCC_ERR_NOMEM;
    }
    dcc_app_module_entry_t *next =
        (dcc_app_module_entry_t *)realloc(app->modules, next_cap * sizeof(*app->modules));
    if (next == NULL) {
        return DCC_ERR_NOMEM;
    }
    app->modules = next;
    app->module_cap = next_cap;
    return DCC_OK;
}

static uint8_t dcc_app_module_has_field(
    const dcc_app_module_t *module,
    size_t offset,
    size_t field_size
) {
    return module != NULL &&
        module->size >= offset &&
        field_size <= module->size - offset;
}

static uint8_t dcc_app_module_name_taken(const dcc_app_t *app, const char *name) {
    if (app == NULL || name == NULL || name[0] == '\0') {
        return 0U;
    }
    for (size_t i = 0; i < app->module_count; ++i) {
        if (app->modules[i].name != NULL && strcmp(app->modules[i].name, name) == 0) {
            return 1U;
        }
    }
    return 0U;
}

dcc_status_t dcc_app_module_register(dcc_app_t *app, const dcc_app_module_t *module) {
    if (app == NULL || module == NULL ||
        !dcc_app_module_has_field(module, offsetof(dcc_app_module_t, setup), sizeof(module->setup)) ||
        module->setup == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    const char *name = NULL;
    if (dcc_app_module_has_field(module, offsetof(dcc_app_module_t, name), sizeof(module->name))) {
        name = module->name;
    }
    if (dcc_app_module_name_taken(app, name)) {
        return DCC_ERR_STATE;
    }
    if (app->module_count == SIZE_MAX) {
        return DCC_ERR_NOMEM;
    }
    dcc_status_t status = dcc_app_modules_reserve(app, app->module_count + 1U);
    if (status != DCC_OK) {
        return status;
    }

    void *user_data = NULL;
    dcc_app_cleanup_fn cleanup = NULL;
    if (dcc_app_module_has_field(module, offsetof(dcc_app_module_t, user_data), sizeof(module->user_data))) {
        user_data = module->user_data;
    }
    if (dcc_app_module_has_field(module, offsetof(dcc_app_module_t, cleanup), sizeof(module->cleanup))) {
        cleanup = module->cleanup;
    }

    status = module->setup(app, user_data);
    if (status != DCC_OK) {
        if (cleanup != NULL) {
            cleanup(user_data);
        }
        return status;
    }

    dcc_app_module_entry_t *entry = &app->modules[app->module_count++];
    entry->name = name;
    entry->user_data = user_data;
    entry->cleanup = cleanup;
    return DCC_OK;
}

size_t dcc_app_module_count(const dcc_app_t *app) {
    return app != NULL ? app->module_count : 0U;
}
