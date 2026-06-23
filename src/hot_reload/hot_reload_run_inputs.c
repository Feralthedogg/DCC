#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <dcc/app/env.h>

#include <stddef.h>
#include <string.h>

int dcc_hot_reload_run_options_has(
    const dcc_hot_reload_run_options_t *options,
    size_t offset,
    size_t size
) {
    return options != NULL && options->size >= offset && options->size - offset >= size;
}

dcc_status_t dcc_hot_reload_run_resolve_client_options(
    const dcc_hot_reload_run_options_t *options,
    dcc_client_options_t *out
) {
    memset(out, 0, sizeof(*out));
    if (dcc_hot_reload_run_options_has(
            options,
            offsetof(dcc_hot_reload_run_options_t, client_options),
            sizeof(options->client_options)
        )) {
        *out = options->client_options;
    }
    out->size = sizeof(*out);
    if (out->token == NULL || out->token[0] == '\0') {
        const char *token = NULL;
        if (dcc_app_env_get_token(NULL, &token) == DCC_OK) {
            out->token = token;
        }
    }
    return out->token == NULL || out->token[0] == '\0' ? DCC_ERR_INVALID_ARG : DCC_OK;
}

dcc_status_t dcc_hot_reload_run_resolve_library_path(
    const dcc_hot_reload_run_options_t *options,
    const char **out
) {
    const char *library_path = NULL;
    if (dcc_hot_reload_run_options_has(
            options,
            offsetof(dcc_hot_reload_run_options_t, library_path),
            sizeof(options->library_path)
        )) {
        library_path = options->library_path;
    }
    if (library_path == NULL || library_path[0] == '\0') {
        library_path = dcc_hot_reload_env_nonempty("DCC_BOT_MODULE");
    }
    if (library_path == NULL || library_path[0] == '\0') {
        return DCC_ERR_INVALID_ARG;
    }
    *out = library_path;
    return DCC_OK;
}
