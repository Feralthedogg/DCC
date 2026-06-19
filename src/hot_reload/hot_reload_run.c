#include "internal/hot_reload/dcc_hot_reload_internal.h"

#include <string.h>

dcc_status_t dcc_hot_reload_run_simple(
    const char *library_path,
    const char *token,
    dcc_intents_t intents
) {
    dcc_hot_reload_run_options_t options;
    memset(&options, 0, sizeof(options));
    options.size = sizeof(options);
    options.client_options.size = sizeof(options.client_options);
    options.client_options.token = token;
    options.client_options.intents = intents != 0 ? intents : DCC_INTENT_GUILDS;
    options.library_path = library_path;
    return dcc_hot_reload_run(&options);
}

dcc_status_t dcc_hot_reload_run_simple_isolated(
    const char *library_path,
    const char *worker_path,
    const char *token,
    dcc_intents_t intents
) {
    worker_path = dcc_hot_reload_run_worker_path_or_default(worker_path);

    dcc_hot_reload_run_options_t options;
    memset(&options, 0, sizeof(options));
    options.size = sizeof(options);
    options.client_options.size = sizeof(options.client_options);
    options.client_options.token = token;
    options.client_options.intents = intents != 0 ? intents : DCC_INTENT_GUILDS;
    options.library_path = library_path;
    options.hot_reload_options.size = sizeof(options.hot_reload_options);
    options.hot_reload_options.backend = DCC_HOT_RELOAD_BACKEND_ISOLATED_WORKER;
    options.hot_reload_options.worker_path = worker_path;
    return dcc_hot_reload_run(&options);
}

dcc_status_t dcc_hot_reload_run_env(dcc_intents_t intents) {
    return dcc_hot_reload_run_simple(NULL, NULL, intents);
}

dcc_status_t dcc_hot_reload_run_env_isolated(dcc_intents_t intents) {
    return dcc_hot_reload_run_simple_isolated(NULL, NULL, NULL, intents);
}

dcc_status_t dcc_hot_reload_run(const dcc_hot_reload_run_options_t *options) {
    if (options == NULL || options->size < sizeof(options->size)) {
        return DCC_ERR_INVALID_ARG;
    }

    dcc_client_options_t client_options;
    const char *library_path = NULL;
    dcc_hot_reload_options_t hot_reload_options;
    const dcc_hot_reload_options_t *hot_reload_options_ptr = NULL;

    dcc_status_t status = dcc_hot_reload_run_resolve_options(
        options,
        &client_options,
        &library_path,
        &hot_reload_options,
        &hot_reload_options_ptr
    );
    if (status != DCC_OK) {
        return status;
    }

    dcc_client_t *client = NULL;
    status = dcc_client_create(&client_options, &client);
    if (status != DCC_OK) {
        return status;
    }

    uint8_t client_started = 0U;
    dcc_hot_reload_t *hot_reload = NULL;
    status = dcc_client_start(client);
    if (status == DCC_OK) {
        client_started = 1U;
        status = dcc_hot_reload_attach(client, library_path, hot_reload_options_ptr, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_client_wait(client);
    }

    dcc_hot_reload_destroy(hot_reload);
    if (client_started != 0U) {
        (void)dcc_client_stop(client);
    }
    dcc_client_destroy(client);
    return status;
}
