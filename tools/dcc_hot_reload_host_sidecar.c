#include "internal/dcc_hot_reload_host.h"

#include <string.h>

void dcc_hot_reload_host_sidecar_init(dcc_hot_reload_host_sidecar_t *sidecar) {
    if (sidecar != NULL) {
        memset(sidecar, 0, sizeof(*sidecar));
    }
}

dcc_status_t dcc_hot_reload_host_sidecar_start(
    const dcc_hot_reload_host_options_t *host_options,
    dcc_hot_reload_t *hot_reload,
    dcc_hot_reload_host_sidecar_t *sidecar
) {
    if (host_options == NULL || hot_reload == NULL || sidecar == NULL) {
        return DCC_ERR_INVALID_ARG;
    }
    if (!host_options->health_enabled) {
        return DCC_OK;
    }

    static const char default_public_key[] =
        "0000000000000000000000000000000000000000000000000000000000000000";
    dcc_interaction_server_options_t server_options;
    dcc_status_t status;

    memset(&server_options, 0, sizeof(server_options));
    server_options.size = sizeof(server_options);
    server_options.public_key_hex = default_public_key;
    server_options.address = host_options->health_address;
    server_options.port = host_options->health_port;

    status = dcc_interaction_server_create(&server_options, &sidecar->server);
    if (status == DCC_OK) {
        status = dcc_hot_reload_add_health_route(sidecar->server, "/hot-reloadz", hot_reload);
    }
    if (status == DCC_OK) {
        status = dcc_interaction_server_start(sidecar->server);
    }
    if (status != DCC_OK) {
        dcc_interaction_server_destroy(sidecar->server);
        sidecar->server = NULL;
        return status;
    }

    char health_url[256];
    (void)snprintf(
        health_url,
        sizeof(health_url),
        "http://%s:%u/hot-reloadz",
        host_options->health_address != NULL ? host_options->health_address : "127.0.0.1",
        (unsigned int)dcc_interaction_server_port(sidecar->server)
    );
    printf("health_sidecar=enabled\n");
    printf("health_url=%s\n", health_url);
    return DCC_OK;
}

void dcc_hot_reload_host_sidecar_stop(dcc_hot_reload_host_sidecar_t *sidecar) {
    if (sidecar == NULL || sidecar->server == NULL) {
        return;
    }
    (void)dcc_interaction_server_stop(sidecar->server);
    dcc_interaction_server_destroy(sidecar->server);
    sidecar->server = NULL;
}
