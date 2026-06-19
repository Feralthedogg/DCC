#include "internal/dcc_hot_reload_host.h"

static dcc_status_t dcc_hot_reload_host_run(const dcc_hot_reload_host_options_t *options) {
    dcc_client_options_t client_options;
    dcc_hot_reload_options_t hot_reload_options;
    dcc_client_t *client = NULL;
    dcc_hot_reload_t *hot_reload = NULL;
    dcc_hot_reload_host_sidecar_t sidecar;
    dcc_status_t status;
    int client_started = 0;

    dcc_hot_reload_host_build_client_options(options, &client_options);
    dcc_hot_reload_host_build_hot_reload_options(options, &hot_reload_options);
    dcc_hot_reload_host_sidecar_init(&sidecar);

    status = dcc_client_create(&client_options, &client);
    if (status == DCC_OK) {
        status = dcc_client_start(client);
    }
    if (status == DCC_OK) {
        client_started = 1;
        status = dcc_hot_reload_attach(client, options->module_path, &hot_reload_options, &hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_hot_reload_host_sidecar_start(options, hot_reload, &sidecar);
    }
    if (status == DCC_OK) {
        status = dcc_client_wait(client);
    }

    dcc_hot_reload_host_sidecar_stop(&sidecar);
    dcc_hot_reload_destroy(hot_reload);
    if (client_started) {
        (void)dcc_client_stop(client);
    }
    dcc_client_destroy(client);
    return status;
}

int main(int argc, char **argv) {
    dcc_hot_reload_host_options_t options;
    int parse_status;
    dcc_status_t status;

    dcc_hot_reload_host_load_env_file(".env");
    parse_status = dcc_hot_reload_host_parse_options(argc, argv, &options);
    if (parse_status > 0) {
        return 0;
    }
    if (parse_status < 0) {
        dcc_hot_reload_host_usage(stderr, argv[0]);
        return 2;
    }

    if (options.check_only) {
        return dcc_hot_reload_host_check_config(&options);
    }

    status = dcc_hot_reload_host_run(&options);
    if (status != DCC_OK && status != DCC_ERR_CANCELED) {
        fprintf(
            stderr,
            "hot reload host failed: %s\n"
            "usage: BOT_TOKEN=... %s [module.so] [worker]\n"
            "or set DCC_BOT_MODULE; DCC_HOT_RELOAD_WORKER overrides the default worker name\n"
            "set DCC_HOT_RELOAD_INTENTS or --intents for non-slash-command events\n"
            "set DCC_HOT_RELOAD_HEALTH_PORT or --health-port to expose /hot-reloadz\n",
            dcc_status_string(status),
            argv[0]
        );
        return 1;
    }
    return 0;
}
