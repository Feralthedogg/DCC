#include "internal/hot_reload/dcc_hot_reload_internal.h"
#include "internal/hot_reload/dcc_hot_reload_loader_file_internal.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

dcc_status_t dcc_hot_reload_open_library(
    dcc_hot_reload_t *hot_reload,
    uint64_t generation,
    dcc_hot_reload_module_t *module
) {
    char *temp_path = dcc_hot_reload_temp_path(hot_reload->path, generation);
    if (temp_path == NULL) {
        return DCC_ERR_NOMEM;
    }

    dcc_status_t status = dcc_hot_reload_copy_file(hot_reload->path, temp_path);
    if (status != DCC_OK) {
        free(temp_path);
        return status;
    }

#if defined(_WIN32)
    HMODULE handle = LoadLibraryA(temp_path);
    if (handle == NULL) {
        dcc_hot_reload_lock(hot_reload);
        snprintf(hot_reload->last_error, sizeof(hot_reload->last_error), "LoadLibrary failed for %s", temp_path);
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        remove(temp_path);
        free(temp_path);
        return DCC_ERR_RUNTIME;
    }
    module->handle = handle;
    module->loaded_path = temp_path;
#else
    void *handle = dlopen(temp_path, RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL) {
        const char *error = dlerror();
        dcc_hot_reload_lock(hot_reload);
        snprintf(
            hot_reload->last_error,
            sizeof(hot_reload->last_error),
            "dlopen failed for %s: %s",
            temp_path,
            error != NULL ? error : "unknown error"
        );
        dcc_hot_reload_broadcast(hot_reload);
        dcc_hot_reload_unlock(hot_reload);
        remove(temp_path);
        free(temp_path);
        return DCC_ERR_RUNTIME;
    }
    module->handle = handle;
    module->loaded_path = temp_path;
#endif
    return DCC_OK;
}

dcc_hot_reload_symbol_fn dcc_hot_reload_symbol(dcc_hot_reload_module_t *module, const char *name) {
    if (module == NULL || module->handle == NULL || name == NULL) {
        return NULL;
    }
#if defined(_WIN32)
    union {
        FARPROC raw;
        dcc_hot_reload_symbol_fn symbol;
    } cast;
    cast.raw = GetProcAddress(module->handle, name);
    return cast.symbol;
#else
    union {
        void *raw;
        dcc_hot_reload_symbol_fn symbol;
    } cast;
    cast.raw = dlsym(module->handle, name);
    return cast.symbol;
#endif
}

void dcc_hot_reload_close_library(dcc_hot_reload_module_t *module) {
    if (module == NULL || module->handle == NULL) {
        return;
    }
#if defined(_WIN32)
    (void)FreeLibrary(module->handle);
    if (module->loaded_path != NULL) {
        remove(module->loaded_path);
    }
#else
    (void)dlclose(module->handle);
    if (module->loaded_path != NULL) {
        remove(module->loaded_path);
    }
#endif
    module->handle = NULL;
}
