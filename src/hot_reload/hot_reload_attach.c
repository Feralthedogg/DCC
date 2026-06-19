#include "internal/hot_reload/dcc_hot_reload_internal.h"

dcc_status_t dcc_hot_reload_attach(
    dcc_client_t *client,
    const char *library_path,
    const dcc_hot_reload_options_t *options,
    dcc_hot_reload_t **out
) {
    if (out == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    *out = NULL;

    dcc_hot_reload_t *hot_reload = NULL;
    dcc_status_t status = dcc_hot_reload_create(client, library_path, options, &hot_reload);
    if (status == DCC_OK) {
        status = dcc_hot_reload_reload(hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_start(hot_reload);
    }
    if (status != DCC_OK) {
        dcc_hot_reload_destroy(hot_reload);
        return status;
    }

    *out = hot_reload;
    return DCC_OK;
}
